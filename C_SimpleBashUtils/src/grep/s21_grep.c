#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arguments {
  bool arg_e;
  int arg_i;
  bool arg_v;
  bool arg_c;
  bool arg_l;
  bool arg_n;
  bool arg_h;
  bool arg_s;
  bool arg_f;
  bool arg_o;
  char pattern[4096];
  int len_pattern;
} arguments;

void pattern_add(arguments *args, char *pattern);

void add_reg_from_file(arguments *ar, char *filepath) {
  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    if (!ar->arg_s) perror(filepath);
    exit(1);
  }

  char *line = NULL;
  size_t memline = 0;
  int read = getline(&line, &memline, f);

  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    pattern_add(ar, line);
    read = getline(&line, &memline, f);
  }
  free(line);
  fclose(f);
}

void pattern_add(arguments *args, char *pattern) {
  if (args->len_pattern != 0) {
    strcat(args->pattern + args->len_pattern, "|");
    args->len_pattern += 1;
  }

  args->len_pattern +=
      sprintf(args->pattern + args->len_pattern, "(%s)", pattern);
}

arguments arguments_parser(int argc, char *argv[]) {
  arguments args = {0};
  int opt;

  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      break;
      case 'e':
        args.arg_e = true;
        pattern_add(&args, optarg);

        break;
      case 'i':
        args.arg_i = REG_ICASE;

        break;
      case 'v':
        args.arg_v = true;

        break;
      case 'c':
        args.arg_c = true;

        break;
      case 'l':
        args.arg_l = true;

        break;
      case 'n':
        args.arg_n = true;

        break;
      case 'h':
        args.arg_h = true;

        break;
      case 's':
        args.arg_s = true;

        break;
      case 'f':
        args.arg_f = true;
        add_reg_from_file(&args, optarg);

        break;
      case 'o':
        args.arg_o = true;
    }
  }

  if (args.len_pattern == 0) {
    pattern_add(&args, argv[optind]);
    optind += 1;
  }

  if (argc - optind == 1) args.arg_h = 1;

  return args;
};

void output_line(char *line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_match(regex_t *re, char *line) {
  regmatch_t math;
  int offset = 0;

  while (1) {
    int result = regexec(re, line + offset, 1, &math, 0);

    if (result != 0) break;

    for (int i = math.rm_so; i < math.rm_eo; i++) {
      putchar(line[i + offset]);
    }
    putchar('\n');
    offset += math.rm_eo;
  }
}

void processFile(arguments ar, char *path, regex_t *reg) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    if (!ar.arg_s) perror(path);
    exit(1);
  }

  int line_count = 1;
  int c_count = 0;
  char *line = NULL;
  size_t memline = 0;
  int read = getline(&line, &memline, f);

  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);

    if ((result == 0 && !ar.arg_v) || (result != 0 && ar.arg_v)) {
      if (!ar.arg_c && !ar.arg_l) {
        if (!ar.arg_h) printf("%s:", path);

        if (ar.arg_n) printf("%d:", line_count);

        if (ar.arg_o)
          print_match(reg, line);

        else
          output_line(line, read);
      }
      c_count += 1;
    }

    read = getline(&line, &memline, f);

    line_count += 1;
  }

  free(line);

  if (ar.arg_c && !ar.arg_l) {
    if (!ar.arg_h) printf("%s:", path);
    printf("%d\n", c_count);
  }

  if (ar.arg_l && c_count > 0) printf("%s\n", path);
  fclose(f);
}

void output(arguments ar, int argc, char **argv) {
  regex_t re;

  int error = regcomp(&re, ar.pattern, REG_EXTENDED | ar.arg_i);

  if (error) perror("ERROR");

  for (int i = optind; i < argc; i++) {
    processFile(ar, argv[i], &re);
  }

  regfree(&re);
}

int main(int argc, char *argv[]) {
  arguments ar = arguments_parser(argc, argv);
  output(ar, argc, argv);

  return 0;
}