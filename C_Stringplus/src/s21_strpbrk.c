#include "s21_string.h"

char *s21_strpbrk(const char *str1, const char *str2) {
  char *answer = s21_NULL;
  int len_str2 = s21_strlen(str2);
  while (*str1 && !answer) {
    for (int t = 0; t < len_str2; t++) {
      if (*str1 == str2[t]) {
        answer = (char *)str1;
        break;
      }
    }
    if (!answer) str1++;
  }
  return answer;
}