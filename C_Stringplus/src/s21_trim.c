#include "s21_string.h"

s21_size_t s21_strspn_r(const char *str, const char *trim_chars);

void *s21_trim(const char *src, const char *trim_chars) {
  int check = 0;
  char *temp = s21_NULL;
  if (src) {
    if (!trim_chars || s21_strlen(trim_chars) == 0) {
      temp = (char *)malloc(s21_strlen(src) + 1);
      if (temp) {
        s21_strcpy(temp, src);
        check = 1;
      }
    } else {
      s21_size_t offset = s21_strspn(src, trim_chars);
      s21_size_t endoff = s21_strspn_r(src, trim_chars);
      temp = (char *)malloc((s21_strlen(src)) * sizeof(char) + 1);
      if (temp) {
        s21_size_t i = 0;
        for (; offset <= endoff; i++, offset++) {
          temp[i] = src[offset];
        }
        check = 1;
        temp[i] = '\0';
      }
    }
  }

  return check ? temp : s21_NULL;
}

s21_size_t s21_strspn_r(const char *str, const char *trim_chars) {
  s21_size_t i = s21_strlen(str);
  for (; (int)(i - 1) >= 0; i--) {
    if (s21_strchr(trim_chars, str[i]) == s21_NULL) break;
  }
  return i;
}
