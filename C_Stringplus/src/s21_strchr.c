#include "s21_string.h"

char *s21_strchr(const char *str, int c) {
  char *answer = s21_NULL;
  s21_size_t size = (s21_strlen(str) + 1);
  s21_size_t i = 0;
  for (; i < size; i++) {
    if (str[i] == c) {
      answer = (char *)&str[i];
      break;
    }
  }
  return answer;
}
