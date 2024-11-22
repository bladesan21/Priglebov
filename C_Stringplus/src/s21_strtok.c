#include "s21_string.h"

// Выполняет поиск первого вхождения символа c (беззнаковый тип) в строке, на
// которую указывает аргумент str.

char *s21_strtok(char *str, const char *delim) {
  static char *ptr = s21_NULL;
  if (str == s21_NULL) str = ptr;
  while ((str != NULL) && (*str != '\0') && (s21_strchr(delim, *str) != NULL)) {
    str++;
  }
  if (str && *str == '\0') {
    str = s21_NULL;
    ptr = s21_NULL;
  }
  if (str && *str != '\0') {
    ptr = str;
    while (*ptr && !(s21_strchr(delim, *ptr))) {
      ptr++;
    }
  }
  if (ptr && *ptr) {
    *ptr = '\0';
    ptr++;
  }
  /*if (ptr && *ptr == '\0') {
    *ptr = s21_NULL;
 }
 */
  return str;
}
