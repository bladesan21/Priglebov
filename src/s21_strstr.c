#include "s21_string.h"

char *s21_strstr(const char *haystack, const char *needle) {
  char *answer = s21_NULL;
  s21_size_t str1_len = s21_strlen(haystack);
  s21_size_t str2_len = s21_strlen(needle);
  if (str1_len >= str2_len) {
    for (s21_size_t i = 0; i <= str1_len - str2_len; i++) {
      int found = 1;
      for (s21_size_t j = i, t = 0; needle[t]; t++, j++) {
        if (haystack[j] != needle[t]) {
          found = 0;
          break;
        }
      }
      if (found) {
        answer = (char *)haystack + i;
        break;
      }
    }
  }
  return answer;
}
