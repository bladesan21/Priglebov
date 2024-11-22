#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <limits.h>  // UINT_MAX
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINUS 0b10000000000000000000000000000000
#define EXPONENTA 0b111111110000000000000000

typedef struct {
  uint32_t bits[4];
} s21_decimal;

typedef struct {
  uint64_t bits[8];
  uint16_t exp;
} s21_big_decimal;

// for converting from float to decimal
typedef union {
  int uin_t;
  float floa_t;
} s21_float_bits;

// ARITHMETIC -------------------------------------------------------------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// The functions return the error code:
// - 0 - OK
// - 1 - the number is too large or equal to infinity
// - 2 - the number is too small or equal to negative infinity
// - 3 - division by 0

// COMPARISON -------------------------------------------------------------
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);

// Return value:
// - 0 - FALSE
// - 1 - TRUE

// CONVERTORS/PARSERS ----------------------------------------------------
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// Return value - code error:
// - 0 - OK
// - 1 - convertation error

// OTHER FUNCTIONS -------------------------------------------------------
// Округляет указанное Decimal число до ближайшего целого числа в сторону
// отрицательной бесконечности
int s21_floor(s21_decimal value, s21_decimal *result);

// Округляет Decimal до ближайшего целого числа.
int s21_round(s21_decimal value, s21_decimal *result);

// Возвращает целые цифры указанного Decimal числа; любые дробные цифры
// отбрасываются, включая конечные нули.
int s21_truncate(s21_decimal value, s21_decimal *result);

// Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal *result);

// Return value - code error:
// - 0 - OK
// - 1 - calculation error

// HELPERS -------------------------------------------------------

// Проверяет мантиссу на 0
bool s21_is_zero(s21_decimal src);

// Степень
bool s21_check_exponent(s21_decimal src);
int s21_get_exponent(s21_decimal src);
void s21_set_exponent(s21_decimal *src, uint16_t exp);

// Знак
bool s21_get_sign(s21_decimal src);
void s21_set_sign(s21_decimal *src, bool sign);

// бит с 0 по 31
bool s21_get_bit(unsigned int src, unsigned int index);
int find_bit(s21_decimal src, int index);

// из decimal в big_decimal
s21_big_decimal s21_transfer_to_big(s21_decimal src);
// Из big_decimal в s21_decimal
s21_decimal s21_transfer_to_decimal(s21_big_decimal src);

// Проверка на переполнение
bool s21_check_overflow(s21_big_decimal *src);
bool s21_is_overflow(s21_big_decimal src);

// Преведение к одной степени
bool s21_normalization(s21_big_decimal *value_1, s21_big_decimal *value_2);

// сдвиг влево
bool s21_point_left(s21_big_decimal *src);

// сдвиг вправо
uint32_t s21_point_right(s21_big_decimal *src);

// Big decimal +
bool s21_big_add(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result);
// Big decimal -
void s21_big_sub(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result);

// Big decimal огругление
// Если есть непустые биты старше 3 то /10. Если деление не удалось, возвращаем
// 1
bool s21_big_rounding(s21_big_decimal *src);

// Банковское округление
uint32_t s21_exact_round(uint32_t src, uint16_t num[], uint16_t index);

void s21_set_bigbit(s21_big_decimal *dec, int pos, int n);
int s21_get_bit_big(s21_big_decimal dec, int pos);


// DIVISION HELPERS (working with bigger numbers)
typedef struct {
  bool bits[192];
  bool sign;
  int exp;
} s21_decimal_bv;

int s21_div_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result);
void init_decimal(s21_decimal *decimal);
bool s21_get_bit_int(unsigned int num, int pos);
bool s21_get_sign_std(s21_decimal dec);
s21_decimal_bv s21_div_with_modul(s21_decimal_bv bv_value_1,
                                  s21_decimal_bv bv_value_2,
                                  s21_decimal_bv *bv_result);
void s21_rescale_bv_to_zero(s21_decimal_bv *bv_value_1,
                            s21_decimal_bv *bv_value_2);
bool s21_is_null(s21_decimal_bv bv);
void s21_null_decimal_bv(s21_decimal_bv *bv);
bool s21_compare_bits(s21_decimal_bv bv1, s21_decimal_bv bv2);
int s21_last_bit(s21_decimal_bv bv);
void s21_align(s21_decimal_bv *bv_value_1, s21_decimal_bv *bv_value_2);
int s21_division_by_ten(s21_decimal_bv *bv);
void s21_exact_rounding(s21_decimal_bv *bv, int mod);
void s21_right_shift(s21_decimal_bv *bv);
void s21_left_shift(s21_decimal_bv *bv);
s21_decimal_bv s21_convert_std_to_bv(s21_decimal std);
s21_decimal s21_convert_bv_to_std(s21_decimal_bv bv);
int s21_mul_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result);
int s21_add_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result);
int s21_sub_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result);
 

#endif
