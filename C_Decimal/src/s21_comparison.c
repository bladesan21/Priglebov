#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int equal = 1;
  int check = 0;
  for (int i = 0; i < 3 && equal == 1; i++) {
    if ((value_1.bits[i] != 0) && (value_2.bits[i] != 0)) {
      equal = 0;
    }
  }

  if (equal == 0 && check == 0) {
    if (s21_get_sign(value_1) != s21_get_sign(value_2)) check = 1;
  }

  if (equal == 0 && check == 0) {
    s21_big_decimal transfer_1 = s21_transfer_to_big(value_1);
    s21_big_decimal transfer_2 = s21_transfer_to_big(value_2);
    s21_normalization(&transfer_1, &transfer_2);
    int test = 4;
    for (int i = 3; i >= 0 && test == i + 1; i--) {
      if (transfer_1.bits[i] == transfer_2.bits[i]) {
        test--;
      }
    }
    if (test == 0) equal = 1;
  }
  return equal;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !(s21_is_equal(value_1, value_2));
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int less = 1;
  if (s21_is_equal(value_1, value_2) == 1) {
    less = 0;
  } else if (s21_get_sign(value_1) == 0 && s21_get_sign(value_2) == 1) {
    less = 0;
  } else if (s21_get_sign(value_1) == 1 && s21_get_sign(value_2) == 0) {
    less = 1;
  } else {
    int sign = s21_get_sign(value_1);
    s21_big_decimal transfer_1 = s21_transfer_to_big(value_1);
    s21_big_decimal transfer_2 = s21_transfer_to_big(value_2);
    s21_normalization(&transfer_1, &transfer_2);
    int test = 0;
    for (int i = 2; sign == 1 && i >= 0 && test == 0; i--) {
      if (transfer_1.bits[i] > transfer_2.bits[i])
        test = 1;
      else if (transfer_1.bits[i] < transfer_2.bits[i])
        test = 2;
    }
    for (int i = 2; sign == 0 && i >= 0 && test == 0; i--) {
      if (transfer_1.bits[i] < transfer_2.bits[i])
        test = 1;
      else if (transfer_1.bits[i] > transfer_2.bits[i])
        test = 2;
    }
    if (test == 1)
      less = 1;
    else
      less = 0;
  }
  return less;
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int less_or_equal = 0;
  if (s21_is_equal(value_1, value_2) == 1) {
    less_or_equal = 1;
  } else {
    less_or_equal = s21_is_less(value_1, value_2);
  }
  return less_or_equal;
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int greater = 1;
  if (s21_is_equal(value_1, value_2) == 1) {
    greater = 0;
  } else if (s21_get_sign(value_1) == 0 && s21_get_sign(value_2) == 1) {
    greater = 1;
  } else if (s21_get_sign(value_1) == 1 && s21_get_sign(value_2) == 0) {
    greater = 0;
  } else {
    int sign = s21_get_sign(value_1);
    s21_big_decimal transfer_1 = s21_transfer_to_big(value_1);
    s21_big_decimal transfer_2 = s21_transfer_to_big(value_2);
    s21_normalization(&transfer_1, &transfer_2);
    int test = 0;
    for (int i = 2; sign == 1 && i >= 0 && test == 0; i--) {
      if (transfer_1.bits[i] < transfer_2.bits[i])
        test = 1;
      else if (transfer_1.bits[i] > transfer_2.bits[i])
        test = 2;
    }
    for (int i = 2; sign == 0 && i >= 0 && test == 0; i--) {
      if (transfer_1.bits[i] > transfer_2.bits[i])
        test = 1;
      else if (transfer_1.bits[i] < transfer_2.bits[i])
        test = 2;
    }
    if (test == 1)
      greater = 1;
    else
      greater = 0;
  }
  return greater;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int greater_or_equal = 1;
  if (s21_is_equal(value_1, value_2) == 1) {
    greater_or_equal = 1;
  } else {
    greater_or_equal = s21_is_greater(value_1, value_2);
  }
  return greater_or_equal;
}
