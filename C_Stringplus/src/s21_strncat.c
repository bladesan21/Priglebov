#include "s21_string.h"

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  s21_size_t dest_len = s21_strlen(dest);
  s21_size_t src_len = 0;
  while (src[src_len] != '\0' && src_len < n) {
    dest[dest_len + src_len] = src[src_len];
    src_len++;
  }
  dest[dest_len + src_len] = '\0';
  return dest;
}
