#include "s21_decimal.h"

bool s21_is_zero(s21_decimal src) {
  return (src.bits[0] == 0 && src.bits[1] == 0 && src.bits[2] == 0) ? 1 : 0;
}

int s21_get_exponent(s21_decimal src) {
  return (src.bits[3] & EXPONENTA) >> 16;
}

bool s21_check_exponent(s21_decimal src) {
  return (s21_get_exponent(src) > 28) ? 1 : 0;
}

void s21_set_exponent(s21_decimal *src, uint16_t exp) {
  src->bits[3] |= (exp << 16);
}

bool s21_get_sign(s21_decimal src) { return s21_get_bit(src.bits[3], 31); }

void s21_set_sign(s21_decimal *src, bool sign) {
  src->bits[3] |= sign ? MINUS : 0;
}

bool s21_get_bit(unsigned int src, unsigned int index) {
  return (src >> index) & 1;
}

int find_bit(s21_decimal src, int index) {
  int mask = 1u << (index % 32);
  return (src.bits[index / 32] & mask) != 0;
}

s21_big_decimal s21_transfer_to_big(s21_decimal src) {
  s21_big_decimal res = {0};
  uint16_t exp = s21_get_exponent(src);
  for (int i = 0; i < 3; i++) res.bits[i] = src.bits[i] & UINT_MAX;
  res.exp = exp;
  return res;
}

s21_decimal s21_transfer_to_decimal(s21_big_decimal src) {
  s21_decimal res = {0};
  for (int i = 0; i < 3; i++) res.bits[i] = src.bits[i] & UINT_MAX;
  s21_set_exponent(&res, src.exp);
  return res;
}

bool s21_normalization(s21_big_decimal *value_1, s21_big_decimal *value_2) {
  bool res = 0;
  if (value_1->exp == value_2->exp) return res;
  if (value_1->exp > value_2->exp)
    while (value_2->exp < value_1->exp) res = s21_point_left(value_2);
  else
    while (value_1->exp < value_2->exp) res = s21_point_left(value_1);
  return res;
}

bool s21_point_left(s21_big_decimal *src) {
  bool overflow = 0;
  s21_big_decimal copy = *src;
  for (int i = 0; i < 7; i++) copy.bits[i] *= 10;
  copy.exp++;
  overflow = s21_check_overflow(&copy);
  if (!overflow && src->exp < 28) *src = copy;
  return overflow;
}

uint32_t s21_point_right(s21_big_decimal *src) {
  uint32_t temp = 0;
  if (s21_get_exponent(s21_transfer_to_decimal(*src)) > 0) {
    s21_big_decimal copy = *src;
    for (int i = 6; i >= 0; i--) {
      copy.bits[i] += (uint64_t)temp << 32;
      temp = copy.bits[i] % 10;
      copy.bits[i] /= 10;
    }
    copy.exp--;
    *src = copy;
  }
  return temp;
}

bool s21_check_overflow(s21_big_decimal *src) {
  uint32_t overflow = 0;
  for (int i = 0; i < 7; i++) {
    src->bits[i] += overflow;
    overflow = src->bits[i] >> 32;
    src->bits[i] &= UINT_MAX;
  }
  return s21_is_overflow(*src);
}

bool s21_is_overflow(s21_big_decimal src) {
  bool res = 0;
  for (int i = 3; i < 7 && !res; i++) res = src.bits[i] ? 1 : 0;
  return res;
}

bool s21_big_add(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result) {
  bool res = 0;
  s21_big_decimal copy = {{0}, 0};
  if (value_1.exp >= value_2.exp) {
    copy.exp = value_1.exp;
  } else {
    copy.exp = value_2.exp;
  }

  int rem = 0, ovf = 0;
  for (int i = 0; i < 256; i++) {
    rem = s21_get_bit_big(value_1, i) + s21_get_bit_big(value_2, i) + ovf;
    ovf = rem / 2;
    s21_set_bigbit(&copy, i, rem % 2);
  }
  res = s21_check_overflow(&copy);
  if (res) res = s21_big_rounding(&copy);
  *result = copy;

  return res;
}

void s21_big_sub(s21_big_decimal value_1, s21_big_decimal value_2,
                 s21_big_decimal *result) {
  s21_big_decimal copy = value_1;
  bool temp = 0;

  for (int i = 0; i < 3; i++) {
    copy.bits[i] += temp ? ~(value_2.bits[i] + 1) + 1 : ~value_2.bits[i] + 1;
    temp = 0;
    if (value_1.bits[i] < value_2.bits[i]) temp = 1;
  }
  *result = copy;
}

bool s21_big_rounding(s21_big_decimal *src) {
  bool res = 0;
  s21_big_decimal copy = *src;
  uint16_t temps[28] = {0};
  uint16_t i = 0;
  while ((copy.exp && s21_is_overflow(copy)) || copy.exp > 28)
    temps[i++] = s21_point_right(&copy);
  if (copy.exp == 0 && s21_is_overflow(copy)) {
    res = 1;
  } else {
    copy.bits[0] = s21_exact_round(copy.bits[0], temps, i);
    *src = copy;
  }
  return res;
}

uint32_t s21_exact_round(uint32_t src, uint16_t num[], uint16_t index) {
  uint32_t res = 0;
  if (num[index] > 5) {
    res = src + 1;
  } else if (num[index] < 5) {
    res = src;
  } else {
    while (index)
      if (num[--index]) res = src + 1;
    if (src & 0)  // если цифры равны нулю, число округляется к
                  // ближайшему четному.
      res = src;
    else
      res = src + 1;
  }
  return res;
}

void s21_set_bigbit(s21_big_decimal *dec, int pos, int n) {
  if (n == 1) {
    dec->bits[pos / 32] |= 1u << (pos % 32);
  } else {
    dec->bits[pos / 32] &= ~(1u << (pos % 32));
  }
}

int s21_get_bit_big(s21_big_decimal dec, int pos) {
  return ((dec.bits[pos / 32] >> (pos % 32)) & 1u);
}

// DIVISION HELPERS

int s21_div_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result) {
  s21_decimal_bv ten;
  s21_null_decimal_bv(&ten);
  ten.bits[1] = 1;
  ten.bits[3] = 1;
  s21_null_decimal_bv(bv_result);
  int res = 0;
  bool sign = bv_value_1.sign ^ bv_value_2.sign;
  bv_value_1.sign = 0;
  bv_value_2.sign = 0;

  s21_rescale_bv_to_zero(&bv_value_1, &bv_value_2);
  s21_decimal_bv modul;
  s21_null_decimal_bv(&modul);
  modul = s21_div_with_modul(bv_value_1, bv_value_2, &bv_value_1);
  int exp = 0;
  int status = 0;
  while (!s21_is_null(bv_value_1) || !s21_is_null(modul)) {
    exp++;
    s21_mul_bv(modul, ten, &modul);
    s21_mul_bv(*bv_result, ten, bv_result);
    status = s21_add_bv(*bv_result, bv_value_1, bv_result);
    modul = s21_div_with_modul(modul, bv_value_2, &bv_value_1);
    if (status == 1) break;
  }
  int mod;
  if (status == 1) {
    mod = s21_division_by_ten(bv_result);
    while (s21_last_bit(*bv_result) > 95 && exp > 0) {
      mod = s21_division_by_ten(bv_result);
      exp--;
    }
    s21_exact_rounding(bv_result, mod);
    exp--;
  }
  bv_result->exp = exp - 1;
  mod = 10;
  while (bv_result->exp > 28) mod = s21_division_by_ten(bv_result);
  if (mod != 10) s21_exact_rounding(bv_result, mod);
  if (s21_last_bit(*bv_result) > 95) {
    if (sign)
      res = 2;
    else
      res = 1;
  }
  bv_result->sign = sign;
  return res;
}

void init_decimal(s21_decimal *decimal) {
  for (int i = 0; i < 4; i++) {
    decimal->bits[i] = 0;
  }
}

bool s21_get_bit_int(unsigned int num, int pos) { return (num >> pos) & 1; }

bool s21_get_sign_std(s21_decimal dec) {
  return s21_get_bit_int(dec.bits[3], 31);
}

int s21_get_exponent_std(s21_decimal dec) {
  return (dec.bits[3] % 2147483648) >> 16;
}

s21_decimal_bv s21_div_with_modul(s21_decimal_bv bv_value_1,
                                  s21_decimal_bv bv_value_2,
                                  s21_decimal_bv *bv_result) {
  int denominator_left_bit = s21_last_bit(bv_value_2);
  s21_decimal_bv modul;
  s21_null_decimal_bv(&modul);
  s21_null_decimal_bv(bv_result);
  if (s21_compare_bits(bv_value_1, bv_value_2)) {
    s21_align(&bv_value_1, &bv_value_2);
  }
  for (int i = 0; i < 191; i++) {
    if (s21_compare_bits(bv_value_1, bv_value_2)) {
      s21_sub_bv(bv_value_1, bv_value_2, &bv_value_1);
      bv_result->bits[0] = 1;
    }
    if (s21_last_bit(bv_value_2) <= denominator_left_bit) break;
    if (s21_last_bit(bv_value_2) > denominator_left_bit)
      s21_right_shift(&bv_value_2);
    s21_left_shift(bv_result);
  }
  return bv_value_1;
}

void s21_rescale_bv_to_zero(s21_decimal_bv *bv_value_1,
                            s21_decimal_bv *bv_value_2) {
  s21_decimal_bv ten;
  s21_null_decimal_bv(&ten);
  ten.bits[1] = 1;
  ten.bits[3] = 1;
  if (bv_value_1->exp > bv_value_2->exp) {
    bv_value_1->exp -= bv_value_2->exp;
    bv_value_2->exp = 0;
    while (bv_value_1->exp != 0) {
      bv_value_1->exp--;
      s21_mul_bv(*bv_value_2, ten, bv_value_2);
    }
  } else {
    bv_value_2->exp -= bv_value_1->exp;
    bv_value_1->exp = 0;
    while (bv_value_2->exp != 0) {
      bv_value_2->exp--;
      s21_mul_bv(*bv_value_1, ten, bv_value_1);
    }
  }
}

bool s21_is_null(s21_decimal_bv bv) {
  bool result = 0;
  for (int i = 0; i < 192; i++) result += bv.bits[i];
  return !result;
}

void s21_null_decimal_bv(s21_decimal_bv *bv) {
  bv->sign = 0;
  bv->exp = 0;
  for (int i = 0; i < 192; i++) bv->bits[i] = 0;
}

bool s21_compare_bits(s21_decimal_bv bv1, s21_decimal_bv bv2) {
  int i = 191;
  while (i >= 0 && bv1.bits[i] == bv2.bits[i]) {
    i--;
  }
  return i == -1 ? 1 : bv1.bits[i];
}

int s21_last_bit(s21_decimal_bv bv) {
  int i = -1;
  if (!s21_is_null(bv)) {
    for (i = 191; i >= 0; i--)
      if (bv.bits[i] == 1) break;
  }
  return i;
}

void s21_align(s21_decimal_bv *bv_value_1, s21_decimal_bv *bv_value_2) {
  if (!s21_is_null(*bv_value_1) && !s21_is_null(*bv_value_2)) {
    int l1 = s21_last_bit(*bv_value_1);
    int l2 = s21_last_bit(*bv_value_2);
    while (s21_last_bit(*bv_value_1) != s21_last_bit(*bv_value_2))
      if (s21_compare_bits(*bv_value_1, *bv_value_2))
        s21_left_shift(bv_value_2);
      else
        s21_left_shift(bv_value_1);
    if (!s21_compare_bits(*bv_value_1, *bv_value_2)) s21_left_shift(bv_value_1);
    if (bv_value_1->bits[0] == 0 && bv_value_2->bits[0] == 0 &&
        s21_last_bit(*bv_value_1) > l1 && s21_last_bit(*bv_value_2) > l2) {
      s21_right_shift(bv_value_1);
      s21_right_shift(bv_value_2);
    }
  }
}

int s21_division_by_ten(s21_decimal_bv *bv) {
  s21_decimal_bv result;
  s21_null_decimal_bv(&result);
  int exp = bv->exp;
  int sign = bv->sign;
  result.sign = 0;
  result.exp = 0;
  s21_decimal_bv ten;
  s21_null_decimal_bv(&ten);
  ten.bits[1] = 1;
  ten.bits[3] = 1;
  s21_decimal_bv modul = s21_div_with_modul(*bv, ten, bv);
  int res = s21_convert_bv_to_std(modul).bits[0];
  bv->exp = exp - 1;
  bv->sign = sign;
  return res;
}

void s21_exact_rounding(s21_decimal_bv *bv, int mod) {
  if ((mod == 5 && bv->bits[0]) || mod > 5) {
    s21_decimal_bv one;
    s21_null_decimal_bv(&one);
    one.bits[0] = 1;
    one.exp = bv->exp;
    one.sign = bv->sign;
    s21_add_bv(*bv, one, bv);
    if (s21_last_bit(*bv) > 95) {
      s21_sub_bv(*bv, one, bv);
      mod = s21_division_by_ten(bv);
      s21_exact_rounding(bv, mod);
    }
  }
}

void s21_right_shift(s21_decimal_bv *bv) {
  for (int i = 0; i < 191; i++) bv->bits[i] = bv->bits[i + 1];
  bv->bits[191] = 0;
}

void s21_left_shift(s21_decimal_bv *bv) {
  for (int i = 191; i > 0; i--) bv->bits[i] = bv->bits[i - 1];
  bv->bits[0] = 0;
}

s21_decimal_bv s21_convert_std_to_bv(s21_decimal std) {
  s21_decimal_bv bv;
  s21_null_decimal_bv(&bv);
  bv.sign = s21_get_sign_std(std);
  bv.exp = s21_get_exponent_std(std);
  int i = 0;
  for (int j = 0; j < 3; j++) {
    for (int k = 0; k < 32; k++) bv.bits[i++] = s21_get_bit_int(std.bits[j], k);
  }
  return bv;
}

s21_decimal s21_convert_bv_to_std(s21_decimal_bv bv) {
  s21_decimal std;
  init_decimal(&std);
  int k = 95;
  for (int i = 2; i >= 0; i--)
    for (int j = 31; j >= 0; j--)
      std.bits[i] = (std.bits[i] << 1) + bv.bits[k--];
  std.bits[3] = bv.sign;
  std.bits[3] <<= 7;
  k = 128;
  for (int i = 7; i >= 0; i--) {
    std.bits[3] <<= 1;
    if (bv.exp >= k) {
      bv.exp -= k;
      std.bits[3]++;
    }
    k >>= 1;
  }
  std.bits[3] <<= 16;
  return std;
}

int s21_mul_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result) {
  int res = 0;
  s21_null_decimal_bv(bv_result);
  if (!s21_is_null(bv_value_1) && !(s21_is_null(bv_value_2))) {
    for (int i = 0; i < 192; i++) {
      if (bv_value_2.bits[i] == 1)
        s21_add_bv(*bv_result, bv_value_1, bv_result);
      s21_left_shift(&bv_value_1);
    }
    bv_result->exp = bv_value_1.exp + bv_value_2.exp;
    bv_result->sign = bv_value_1.sign ^ bv_value_2.sign;
    if (s21_last_bit(*bv_result) > 95 || bv_result->exp > 28) {
      int mod = 0;
      while (bv_result->exp > 0 &&
             (s21_last_bit(*bv_result) > 95 || bv_result->exp > 28)) {
        if (bv_result->exp == 0) break;
        mod = s21_division_by_ten(bv_result);
      }
      if (bv_result->exp > 0)
        s21_exact_rounding(bv_result, mod);
      else if (bv_result->sign)
        res = 2;
      else
        res = 1;
    }
  }
  return res;
}

int s21_add_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result) {
  int res = 0;
  bool t_bit = 0;
  s21_null_decimal_bv(bv_result);
  for (int i = 0; i < 192; i++) {
    bv_result->bits[i] = (bv_value_1.bits[i] ^ bv_value_2.bits[i]) ^ t_bit;
    if ((bv_value_1.bits[i] & bv_value_2.bits[i]) ||
        (bv_value_1.bits[i] & t_bit) || (bv_value_2.bits[i] & t_bit))
      t_bit = 1;
    else
      t_bit = 0;
  }
  bv_result->exp = bv_value_1.exp;
  bv_result->sign = bv_value_1.sign;
  if (s21_last_bit(*bv_result) > 95) {
    if (bv_result->exp <= 0) {
      res = 1;
    } else {
      int mod = s21_division_by_ten(bv_result);
      s21_exact_rounding(bv_result, mod);
    }
  }
  return res;
}

int s21_sub_bv(s21_decimal_bv bv_value_1, s21_decimal_bv bv_value_2,
               s21_decimal_bv *bv_result) {
  int res = 0;
  bool t_bit = 0;
  for (int i = 0; i < 192; i++) {
    bv_result->bits[i] = bv_value_1.bits[i] ^ bv_value_2.bits[i] ^ t_bit;
    if (bv_value_1.bits[i] == 0 && bv_value_2.bits[i] == 1)
      t_bit = 1;
    else if (bv_value_1.bits[i] == 1 && bv_value_2.bits[i] == 0)
      t_bit = 0;
  }
  return res;
}
