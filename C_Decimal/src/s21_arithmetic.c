#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;

  // Проверка степени
  bool s21_check_exponent_1 = s21_check_exponent(value_1);
  bool s21_check_exponent_2 = s21_check_exponent(value_2);

  if (s21_check_exponent_1 || s21_check_exponent_2) return 2;

  // Проверка на 0
  bool check_1 = s21_is_zero(value_1);
  bool check_2 = s21_is_zero(value_2);

  // Если оба децимала 0
  if (check_1 && check_2) {
    *result = value_1;
  } else if (check_1 || check_2) {  // Если один из децималов 0
    *result = check_1 ? value_2 : value_1;
  } else {
    // Получение знака
    bool sign_1 = s21_get_sign(value_1);
    bool sign_2 = s21_get_sign(value_2);

    s21_big_decimal transfer_1 = {0};
    s21_big_decimal transfer_2 = {0};
    s21_big_decimal buf = {0};

    transfer_1 = s21_transfer_to_big(value_1);
    transfer_2 = s21_transfer_to_big(value_2);

    // Приведение к общему множителю
    res = s21_normalization(&transfer_1, &transfer_2);

    // Если оба числа отрицательные или положительные
    if (sign_1 == sign_2) {
      res = s21_big_add(transfer_1, transfer_2, &buf);
    } else {
      s21_decimal temp_1 = {0};
      s21_decimal temp_2 = {0};

      // Делаем число положительным
      if (sign_1) {
        s21_negate(value_1, &temp_1);
        temp_2 = value_2;
      } else {
        s21_negate(value_2, &temp_2);
        temp_1 = value_1;
      }

      // Проверка на то, что бы вычесть из большего меньшее
      if (s21_is_less(temp_1, temp_2)) {
        s21_big_sub(transfer_2, transfer_1, &buf);
        sign_1 = 0;
      } else {
        s21_big_sub(transfer_1, transfer_2, &buf);
        sign_2 = 0;
      }
    }
    // Установка знака
    *result = s21_transfer_to_decimal(buf);
    bool sign = sign_1 | sign_2;
    s21_set_sign(result, sign);
  }
  if (res == 1 && s21_get_sign(*result) == 1) {
    res = 2;
  }
  return res;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;
  s21_decimal copy = {0};

  // Получение знака
  bool sign_1 = s21_get_sign(value_1);
  bool sign_2 = s21_get_sign(value_2);

  // Если оба числа отрицательные
  if (sign_1 == sign_2) {
    s21_negate(value_2, &copy);
    res = s21_add(value_1, copy, result);
  } else {
    s21_decimal temp_1 = {0};
    s21_decimal temp_2 = {0};

    // Делаем число положительным
    if (sign_1) {
      s21_negate(value_1, &temp_1);
      temp_2 = value_2;
    } else {
      s21_negate(value_2, &temp_2);
      temp_1 = value_1;
    }

    // Сложение
    res = s21_add(temp_1, temp_2, result);
    s21_set_sign(result, sign_1);
  }

  return res;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;
  s21_decimal_bv bv_value_1 = s21_convert_std_to_bv(value_1);
  s21_decimal_bv bv_value_2 = s21_convert_std_to_bv(value_2);
  s21_decimal_bv bv_result;
  s21_null_decimal_bv(&bv_result);
  res = s21_mul_bv(bv_value_1, bv_value_2, &bv_result);
  *result = s21_convert_bv_to_std(bv_result);
  return res;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;
  if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    res = 3;
  } else if (value_1.bits[0] == 0 && value_1.bits[1] == 0 &&
             value_1.bits[2] == 0) {
    init_decimal(result);
  } else {
    s21_decimal_bv bv_value_1 = s21_convert_std_to_bv(value_1);
    s21_decimal_bv bv_value_2 = s21_convert_std_to_bv(value_2);
    s21_decimal_bv bv_result;
    s21_null_decimal_bv(&bv_result);
    while (bv_value_1.bits[0] == 0 && bv_value_2.bits[0] == 0) {
      s21_right_shift(&bv_value_1);
      s21_right_shift(&bv_value_2);
    }
    res = s21_div_bv(bv_value_1, bv_value_2, &bv_result);
    *result = s21_convert_bv_to_std(bv_result);
  }
  return res;
}
