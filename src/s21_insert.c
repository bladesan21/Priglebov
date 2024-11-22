#include "s21_string.h"

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  int check = 0;
  char *temp = s21_NULL;
  if (src && str) {
    temp =
        (char *)malloc((s21_strlen(src) + s21_strlen(str)) * sizeof(char) + 1);
    if (temp && s21_strlen(src) >= start_index) {
      s21_size_t i = 0;
      while (*src) {
        if (i == start_index) {
          while (*str) {
            temp[i++] = *str++;
          }
        }
        temp[i++] = *src++;
      }
      temp[i] = '\0';
      check = 1;
    } else {
      if (temp) free(temp);
      temp = s21_NULL;
    }
  }

  return check ? temp : s21_NULL;
}
