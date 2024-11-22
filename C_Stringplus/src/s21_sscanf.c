#include "s21_sscanf.h"

int s21_sscanf(const char *inputStr, const char *format, ...) {
  int argCount = -1;  // количество успешно считанных аргументов
  char *string = (char *)inputStr;

  if (*inputStr) {
    argCount++;
    token tokens[BUFF_SIZE] = {0};
    int tokenCount = 0;

    char *formatString = (char *)format;
    parseFormatString(
        formatString, tokens,
        &tokenCount);  // разбираем форматную строку и создаем токены

    // Инициализируем список аргументов
    va_list ap;
    va_start(ap, format);

    // Обрабатываем токены и считываем значения из строки
    processTokens(tokens, tokenCount, string, &argCount, ap);
    va_end(ap);
  }
  return argCount;
}

void processTokens(token *tokens, int tokenCount, char *string, int *argCount,
                   va_list ap) {
  int flag = 1;  // успешность обработки токенов
  s21_size_t string_len = s21_strlen(string);

  // проходим по каждому токену
  for (int i = 0; i < tokenCount && flag; i++) {
    while (flag && tokens[i].str != s21_NULL && tokens[i].prevStrLen != 0 &&
           *string != '\0') {
      if (!isSpace(*string)) {
        // текущий символ совпадает с ожидаемым символом в токене
        if (*string == *(tokens[i].str)) {
          string++;
          tokens[i].str++;
          tokens[i].prevStrLen--;
          while (isSpace(*string)) {
            string++;
          }
        } else {
          // символ не совпадает, устанавливаем флаг в 0 и прерываем цикл
          flag = 0;
        }
      } else {
        // текущий символ является пробелом, пропускаем его и переходим к
        // следующему
        string++;
      }
    }
    if (flag) {
      // флаг равен 1 (токен успешно обработан), выполняем дальнейшую обработку
      while ((isSpace(*string) &&
              (tokens[i].wasSpace == 1 || !(tokens[i].type == 'c'))) ||
             (*string == '%' && tokens[i].skipPercent == 1)) {
        // пропускаем пробелы или символы '%', если это указано в токене
        string++;
      }
      if (flag && tokens[i].type != 0) {
        processToken(tokens, i, &string, ap, argCount, &flag, string_len);
      }
    }
  }
}

void processToken(token *tokens, int tokenIndex, char **string, va_list ap,
                  int *argCount, int *flag, s21_size_t string_len) {
  switch (tokens[tokenIndex].type) {
    case 'd':
      processDecimalToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'i':
      processIntegerToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'u':
      processUnsignedToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'X':
    case 'x':
      processHexToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'o':
      processOctalToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'c':
      processCharToken(tokens, tokenIndex, string, ap, argCount);
      break;
    case 's':
      processStringToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'p':
      processPointerToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    case 'n':
      processTokenCount(string, ap, string_len);
      break;
    case 'e':
    case 'E':
    case 'g':
    case 'G':
    case 'f':
      processFloatToken(tokens, tokenIndex, string, ap, argCount, flag);
      break;
    default:
      break;
  }
}

void processCharToken(token *tokens, int i, char **string, va_list ap,
                      int *argCount) {
  if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
  if (tokens[i].widthType == WIDTH_STAR) {
    // пропускаем один символ в строке, если указана ширина через звездочку
    (*string)++;
  } else if (tokens[i].widthType == WIDTH_NUMBER) {
    if (tokens[i].width == 0) {
      tokens[i].width = 1;
    }
    if (tokens[i].lengthType == LENGTH_NONE) {
      // считываем символы из строки и сохраняем их в аргументе типа char
      char *arg = va_arg(ap, char *);
      s21_strncpy(arg, *string, tokens[i].width);
      arg[tokens[i].width] = '\0';
      (*string) += tokens[i].width;
    } else if (tokens[i].lengthType == LENGTH_LONG_INT) {
      // считываем символы из строки и сохраняем их в аргументе типа wchar_t
      // (широкий символ)
      wchar_t *arg = va_arg(ap, wchar_t *);
      mbstowcs(arg, *string, tokens[i].width);
      arg[tokens[i].width] = '\0';
      (*string) += tokens[i].width;
    }
  } else {
    if (tokens[i].lengthType == LENGTH_NONE) {
      // считываем один символ из строки и сохраняем его в аргументе типа char
      char *arg = va_arg(ap, char *);
      *arg = **string;
      (*string)++;
    } else if (tokens[i].lengthType == LENGTH_LONG_INT) {
      // считываем один символ из строки и сохраняем его в аргументе типа
      // wchar_t (широкий символ)
      wchar_t *arg = va_arg(ap, wchar_t *);
      mbstowcs(arg, *string, 1);
      (*string)++;
    }
  }
}

void processStringToken(token *tokens, int i, char **string, va_list ap,
                        int *argCount, int *flag) {
  if (**string == '\0') *flag = 0;
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    if (tokens[i].widthType == WIDTH_STAR) {
      // пропускаем символы в строке до первого пробела или конца строки
      while (**string != '\0' && !isSpace(**string)) {
        (*string)++;
      }
    } else {
      if (tokens[i].lengthType == LENGTH_NONE) {
        // считываем символы из строки и сохраняем их в аргументе типа char
        char *arg = va_arg(ap, char *);
        unsigned int j = 0;
        while ((j < tokens[i].width || tokens[i].widthType == WIDTH_NONE) &&
               **string != '\0' && !isSpace(**string)) {
          arg[j] = **string;
          j++;
          (*string)++;
        }
        arg[j] = '\0';
      } else if (tokens[i].lengthType == LENGTH_LONG_INT) {
        // считываем символы из строки и сохраняем их в аргументе типа wchar_t
        // (широкий символ)
        wchar_t *arg = va_arg(ap, wchar_t *);
        unsigned int j = 0;
        while ((j < tokens[i].width || tokens[i].widthType == WIDTH_NONE) &&
               !isSpace(**string) && **string != '\0') {
          mbstowcs(&arg[j], *string, 1);
          j++;
          (*string)++;
        }
        arg[j] = '\0';
      }
    }
    // пропускаем пробелы в строке
    while (**string == ' ') {
      (*string)++;
    }
  }
}

void processPointerToken(token *tokens, int i, char **string, va_list ap,
                         int *argCount, int *flag) {
  // является ли форматный спецификатор указателя ('p') допустимым для
  // шестнадцатеричного значения
  *flag = isValidHexFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) {
      (*argCount)++;
    }
    if (tokens[i].widthType == WIDTH_STAR) {
      // пропускаем шестнадцатеричное число, если указана ширина через
      // звездочку
      skipHexNumber(string, 0);
    } else {
      char *tmp = (char *)malloc((s21_strlen(*string) + 3) * sizeof(char));
      tmp[0] = '0';
      tmp[1] = 'x';
      tmp[2] = '\0';
      s21_strcpy(tmp + 2, *string);
      // разбираем беззнаковое значение из строки, интерпретируя его как
      // шестнадцатеричное число
      parseUnsignedFromString(tokens[i], string, ap, 16);
      free(tmp);
    }
  }
}

void processFloatToken(token *tokens, int i, char **string, va_list ap,
                       int *argCount, int *flag) {
  // проверяем, является ли форматный спецификатор допустимым для числа с
  // плавающей точкой
  *flag = isValidFloatFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType == WIDTH_STAR) {
      // пропускаем число, если указана ширина через звездочку
      *string = skipFloatNumber(*string, tokens[i].width);
    } else {
      (*argCount)++;
      void *arg = va_arg(ap, void *);
      char *tmp = s21_NULL;
      if (tokens[i].widthType == WIDTH_NUMBER) {
        // читаем число с плавающей точкой указанной ширины
        tmp = (char *)malloc((tokens[i].width + 1) * sizeof(char));
        s21_strncpy(tmp, *string,
                    tokens[i].width > s21_strlen(*string) ? s21_strlen(*string)
                                                          : tokens[i].width);
        tmp[tokens[i].width > s21_strlen(*string) ? s21_strlen(*string)
                                                  : tokens[i].width] = '\0';
      } else {
        // читаем число с плавающей точкой без указания ширины
        tmp = (char *)malloc((s21_strlen(*string) + 1) * sizeof(char));
        s21_strcpy(tmp, *string);
      }
      if (tokens[i].lengthType == LENGTH_LONG_DOUBLE)
        // сохраняем число long double
        *((long double *)arg) = (long double)s21_atold(tmp);
      else if (tokens[i].lengthType == LENGTH_LONG_INT)
        // сохраняем число double
        *((double *)arg) = (double)s21_atold(tmp);
      else
        // сохраняем число float
        *((float *)arg) = (float)s21_atold(tmp);
      if (tmp) free(tmp);
      *string = skipFloatNumber(*string, tokens[i].width);
    }
  }
}

char *skipFloatNumber(char *str, int width) {
  int flag = 0, point_flag = 0;
  if (width == 0) width--;

  str = skipSignAndPoint(str, &width, &point_flag);
  str = skipDigits(str, &width, &flag);
  str = skipPointAndDigits(str, &width, &flag, &point_flag);
  str = skipDigits(str, &width, &flag);
  str = skipExponential(str, &width, &flag);
  str = skipDigits(str, &width, &flag);

  return str;
}

void parseUnsignedFromString(token token, char **string, va_list ap, int base) {
  if (token.widthType == WIDTH_STAR) {
    // пропускаем число, если указана ширина через звездочку
    skipNumber(string, base, BUFF_SIZE);
  } else {
    if (token.lengthType == LENGTH_NONE) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем беззнаковое число unsigned int, если указана
        // ширина числа
        unsigned int *arg = va_arg(ap, unsigned int *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем беззнаковое число unsigned int
        unsigned int *arg = va_arg(ap, unsigned int *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    } else if (token.lengthType == LENGTH_LONG_INT) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем беззнаковое число unsigned long, если указана
        // ширина числа
        unsigned long *arg = va_arg(ap, unsigned long *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем беззнаковое число unsigned long
        unsigned long *arg = va_arg(ap, unsigned long *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    } else if (token.lengthType == LENGTH_SHORT) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем беззнаковое число unsigned short, если указана
        // ширина числа
        unsigned short *arg = va_arg(ap, unsigned short *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем беззнаковое число типа unsigned short
        unsigned short *arg = va_arg(ap, unsigned short *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    }
  }
}

void parseNumberFromString(token token, char **string, va_list ap, int base) {
  if (token.widthType == WIDTH_STAR) {
    // пропускаем число, если указана ширина через звездочку
    skipNumber(string, base, BUFF_SIZE);
  } else {
    if (token.lengthType == LENGTH_NONE) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем число типа int, если указана ширина числа
        int *arg = va_arg(ap, int *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем число типа int
        int *arg = va_arg(ap, int *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    } else if (token.lengthType == LENGTH_LONG_INT) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем число типа long, если указана ширина числа
        long *arg = va_arg(ap, long *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем число типа long
        long *arg = va_arg(ap, long *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    } else if (token.lengthType == LENGTH_SHORT) {
      if (token.widthType == WIDTH_NUMBER) {
        // читаем и сохраняем число типа short, если указана ширина числа
        short *arg = va_arg(ap, short *);
        char *tmp = (char *)malloc((token.width + 1) * sizeof(char));
        s21_strncpy(tmp, *string, token.width);
        tmp[token.width] = '\0';
        *arg = s21_strtol(tmp, base);
        free(tmp);
        skipNumber(string, base, token.width);
      } else {
        // читаем и сохраняем число типа short
        short *arg = va_arg(ap, short *);
        *arg = s21_strtol(*string, base);
        skipNumber(string, base, BUFF_SIZE);
      }
    }
  }
}

void skipHexNumber(char **string, int width) {
  int pre_flag = 0;  // был ли обнаружен префикс '0'

  if (width == 0) width--;

  // обнаружен знак '+' или '-' перед числом
  if ((**string == '+' || **string == '-') && width != 1) {
    if ((*(*string + 1) >= '0' && *(*string + 1) <= '9') ||
        (*(*string + 1) >= 'a' && *(*string + 1) <= 'f') ||
        (*(*string + 1) >= 'A' && *(*string + 1) <= 'F')) {
      (*string)++;  // пропускаем знак
      width--;
    }
  }

  // обнаружен префикс '0'
  if (**string == '0' && width != 0) {
    (*string)++;
    pre_flag = 1;
  }

  // обнаружен символ 'x' или 'X' после префикса '0'
  if ((**string == 'x' || **string == 'X') && width != 1 && pre_flag) {
    if ((*(*string + 1) >= '0' && *(*string + 1) <= '9') ||
        (*(*string + 1) >= 'a' && *(*string + 1) <= 'f') ||
        (*(*string + 1) >= 'A' && *(*string + 1) <= 'F')) {
      (*string) += 2;
      width -= 2;
    }
  }

  // пропускаем цифры и буквы шестнадцатеричного числа
  while (((**string >= '0' && **string <= '9') ||
          (**string >= 'a' && **string <= 'f') ||
          (**string >= 'A' && **string <= 'F')) &&
         width != 0) {
    (*string)++;
    width--;
  }
}

void parseFormatString(char *formatString, token *tokens, int *tokens_count) {
  int i = 0;  // текущий символ в formatString
  int j = 0;  // текущий элемент в tokens
  int len = s21_strlen(formatString);

  while (i < len) {
    if (formatString[i] == '%') {
      i++;
      parseToken(formatString, &i, tokens, &j);
    } else {
      skipSpacesAndUpdateToken(formatString, &i, tokens, j);
      parseLiteral(formatString, &i, tokens, j, len);
    }
  }

  *tokens_count = j;
}

void parseToken(char *formatString, int *i, token *tokens, int *j) {
  if (formatString[*i] == '*') {
    tokens[*j].widthType = WIDTH_STAR;
    tokens[*j].width = 0;
    (*i)++;
  } else if (formatString[*i] >= '0' && formatString[*i] <= '9') {
    tokens[*j].widthType = WIDTH_NUMBER;
    tokens[*j].width = 0;

    while (formatString[*i] >= '0' && formatString[*i] <= '9') {
      tokens[*j].width = tokens[*j].width * 10 + (formatString[*i] - '0');
      (*i)++;
    }
  } else {
    tokens[*j].widthType = WIDTH_NONE;
    tokens[*j].width = 0;
  }

  tokens[*j].lengthType = determineLengthType(formatString, i);

  if (formatString[*i] == '%') {
    tokens[*j].skipPercent = 1;
    (*i)++;
  } else {
    tokens[*j].type = formatString[*i];
    (*i)++;
    (*j)++;
  }
}

void parseLiteral(char *formatString, int *i, token *tokens, int j, int len) {
  skipSpacesAndUpdateToken(formatString, i, tokens, j);

  if (tokens[j].type == 0 && formatString[*i] != '%') {
    tokens[j].str = &formatString[*i];
    tokens[j].prevStrLen = 1;
    (*i)++;
    tokens[j].wasSpace = 0;

    while (formatString[*i] != '%' && *i < len) {
      if (isSpace(formatString[*i])) {
        skipSpacesAndUpdateToken(formatString, i, tokens, j);
        break;
      }
      tokens[j].prevStrLen++;
      (*i)++;
    }
  }
}

int isValidFloatFormat(char *str, int width) {
  int result = 1;

  if (width == 0) {
    width--;
  }

  // ширина 1 и первый символ равен '-' или '+'
  if (width == 1 && (*str == '-' || *str == '+')) {
    result = 0;
  } else if ((*str == '-' || *str == '+') && (width > 1 || width < 0) &&
             (!(*(str + 1) >= '0' && *(str + 1) <= '9') &&
              !(*(str + 1) == '.'))) {
    // первый символ равен '-' или '+', а следующий символ не является цифрой
    // или точкой
    result = 0;
  } else if ((*str == '-' || *str == '+') && (*(str + 1) == '.')) {
    if (width == 2) {
      result = 0;
    }
    // третий символ не является цифрой, формат недопустим
    if (*(str + 2)) {
      if (!(*(str + 2) >= '0' && *(str + 2) <= '9')) {
        result = 0;
      }
    } else {
      // строка заканчивается после второго символа
      result = 0;
    }
  } else if (*str == '.' && width == 1) {
    // ширина равна 1 и первый символ равен '.'
    result = 0;
  } else if (*str == '.' && (width > 1 || width < 0) &&
             !(*(str + 1) >= '0' && *(str + 1) <= '9')) {
    // первый символ равен '.', а следующий символ не является цифрой
    result = 0;
  } else if (!(*str >= '0' && *str <= '9') && !(*str == '.') &&
             !(*str == '-') && !(*str == '+')) {
    // первый символ не является цифрой, точкой, '-' или '+'
    result = 0;
  } else if ((*str == '+' && *(str + 1) == '-') ||
             (*str == '-' && *(str + 1) == '+')) {
    // первый символ равен '+' и следующий символ равен '-' или наоборот
    result = 0;
  }

  return result;
}

long s21_strtol(const char *str,
                int radix) {  // Преобразовывает в целое число типа long int
  int sign = 1, i = 0;  // знак числа (по умолчанию положительный)
  long int res = 0;  // результат преобразования

  if (str[i] == '-') {
    sign = -1;
    i++;
  } else if (str[i] == '+') {
    i++;
  }
  // обработка знака числа
  if (str[i] == '0' && radix == 16) {
    // обработка префикса "0x" или "0X" для шестнадцатеричной системы
    if (str[i + 1]) {
      if (str[i + 1] == 'x' || str[i + 1] == 'X') i += 2;
    }
  }

  // проверка наличия префикса "0x" или "0X"
  while (str[i]) {
    if ((!(str[i] >= '0' && str[i] <= radix + 47 && radix <= 10)) &&
        (!(((str[i] >= '0' && str[i] < '9') ||
            (str[i] >= 'A' && str[i] <= radix + 54) ||
            (str[i] >= 'a' && str[i] <= radix + 86)) &&
           radix > 10 && radix < 37))) {
      break;
    }

    // проверка на допустимые символы для выбранной системы счисления
    else {
      if (str[i] >= 'A' && str[i] <= 'Z')
        res = res * radix + (str[i] - 'A' + 10);
      else if (str[i] >= 'a' && str[i] <= 'z')
        res = res * radix + (str[i] - 'a' + 10);
      else
        res = res * radix + str[i] - '0';
      // преобразование символа в число и умножение результата на основание
      i++;
    }
    // переход к следующему символу
  }
  return res * sign;
  // результат, умноженный на знак числа
}

long double s21_atold(char *str) {  // Преобразует в типа long double
  int sign = 1, i = 0;
  long double inc = 0.1L;

  long double res = 0.L;
  if (str[i] == '-') {
    sign = -1;
    i++;
  } else if (str[i] == '+') {
    i++;
  }
  while (str[i] != '.' && str[i] >= '0' && str[i] <= '9') {
    res = res * 10.L + str[i] - '0';
    i++;
  }
  if (str[i] == '.') i++;
  while (str[i] && str[i] >= '0' && str[i] <= '9') {
    res = res + (str[i] - '0') * inc;
    inc /= 10.L;
    i++;
  }
  inc = 10.L;
  if (str[i] == 'e' || str[i] == 'E') {
    i++;
    if (str[i] == '-') {
      inc = 0.1L;
      i++;
    } else if (str[i] == '+') {
      inc = 10.L;
      i++;
    }
    int count = 0;
    while (str[i] >= '0' && str[i] <= '9') {
      count = count * 10.L + str[i] - '0';
      i++;
    }

    while (count) {
      res *= inc;
      count--;
    }
  }
  return res * sign;
}

void processTokenCount(char **string, va_list ap, s21_size_t string_len) {
  int *arg = va_arg(
      ap, int *);  // извлекаем указатель на аргумент типа int* из va_list
  s21_size_t cur_len = s21_strlen(*string);
  *arg = string_len -
         cur_len;  // разница между общей длиной строки и текущей длиной
}

char *skipSignAndPoint(char *str, int *width, int *point_flag) {
  if ((*str == '+' || *str == '-') && *width != 1) {
    if (*(str + 1) >= '0' && *(str + 1) <= '9') {
      *width -= 2;
      str += 2;
    } else if (*(str + 1) == '.' && *width != 2) {
      if (*(str + 2)) {
        if (*(str + 2) >= '0' && *(str + 2) <= '9') {
          *width -= 2;
          str += 2;
          *point_flag = 1;
        }
      }
    }
  }
  return str;
}

char *skipDigits(char *str, int *width, int *flag) {
  while (*str >= '0' && *str <= '9' && *width != 0) {
    str++;
    *flag = 1;
    (*width)--;
  }
  return str;
}

char *skipPointAndDigits(char *str, int *width, int *flag, int *point_flag) {
  if (*str == '.' && !(*point_flag) && *width != 0) {
    if (*flag == 1)
      str++;
    else if (*(str + 1) >= '0' && *(str + 1) <= '9' && *width != 1)
      str++;
    (*width)--;
  }
  return str;
}

char *skipExponential(char *str, int *width, int *flag) {
  if ((*str == 'e' || *str == 'E') && *flag && *width != 1 && *width != 0) {
    if (*(str + 1) >= '0' && *(str + 1) <= '9') {
      str += 2;
      *width -= 2;
    } else if ((*(str + 1) == '-' || *(str + 1) == '+') && *width != 2) {
      if (*(str + 2)) {
        if (*(str + 2) >= '0' && *(str + 2) <= '9') {
          str += 3;
          *width -= 3;
        }
      }
    }
  }
  return str;
}

void skipOctalNumber(char **string, int width) {
  if ((**string == '+' || **string == '-') && width != 1) {
    if (*(*string + 1) >= '0' && *(*string + 1) <= '7') {
      (*string) += 2;
      width -= 2;
    }
  }
  while ((**string >= '0' && **string <= '7') && width != 0) {
    (*string)++;
    width--;
  }
}

void skipDecimalNumber(char **string, int width) {
  if ((**string == '+' || **string == '-') && width != 1) {
    if (*(*string + 1) >= '0' && *(*string + 1) <= '9') {
      (*string) += 2;
      width -= 2;
    }
  }
  while ((**string >= '0' && **string <= '9') && width != 0) {
    (*string)++;
    width--;
  }
}

int determineLengthType(char *formatString, int *i) {
  int length = 0;
  if (formatString[*i] == 'h') {
    length = LENGTH_SHORT;
    (*i)++;
  } else if (formatString[*i] == 'l') {
    length = LENGTH_LONG_INT;
    (*i)++;
  } else if (formatString[*i] == 'L') {
    length = LENGTH_LONG_DOUBLE;
    (*i)++;
  } else {
    length = LENGTH_NONE;
  }
  return length;
}

int validateIntegerBase(char *str, int width, int *base) {
  int result = 1;
  if (*str == '0') {
    if (*(str + 1) == 'x' || *(str + 1) == 'X') {
      result = isValidHexFormat(str, width);
      *base = 16;
    } else {
      result = isValidOctalFormat(str, width);
      *base = 8;
    }
  } else {
    result = isValidDecimalFormat(str, width);
    *base = 10;
  }
  return result;
}

int isValidDecimalFormat(char *str, int width) {
  int result = 1;
  if (width == 0) width--;
  if (*str == '+' || *str == '-') {
    if (width == 1)
      result = 0;
    else if (!(*(str + 1) >= '0' && *(str + 1) <= '9'))
      result = 0;
  } else if (!(*(str) >= '0' && *(str) <= '9'))
    result = 0;
  return result;
}

void skipNumber(char **string, int base, int width) {
  if (base == 10) {
    skipDecimalNumber(string, width);
  } else if (base == 16) {
    skipHexNumber(string, width);
  } else if (base == 8) {
    skipOctalNumber(string, width);
  }
}

int isValidHexFormat(char *str, int width) {
  int result = 1;
  if (width == 0) width--;
  if (*str == '-' || *str == '+') {
    if (width == 1)
      result = 0;
    else if (!((*(str + 1) >= '0' && *(str + 1) <= '9') ||
               (*(str + 1) >= 'A' && *(str + 1) <= 'F') ||
               (*(str + 1) >= 'a' && *(str + 1) <= 'f')))
      result = 0;
  } else if (!((*str >= '0' && *str <= '9') || (*str >= 'A' && *str <= 'F') ||
               (*str >= 'a' && *str <= 'f')))
    result = 0;
  return result;
}

int isValidOctalFormat(char *str, int width) {
  int result = 1;
  if (width == 0) width--;
  if (*str == '-' || *str == '+') {
    if (width == 1)
      result = 0;
    else if (!(*(str + 1) >= '0' && *(str + 1) <= '7'))
      result = 0;
  } else if (!(*str >= '0' && *str <= '7'))
    result = 0;
  return result;
}

void processDecimalToken(token *tokens, int i, char **string, va_list ap,
                         int *argCount, int *flag) {
  *flag = isValidDecimalFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    parseNumberFromString(tokens[i], string, ap, 10);
  }
}

void processIntegerToken(token *tokens, int i, char **string, va_list ap,
                         int *argCount, int *flag) {
  int base = 0;
  *flag = validateIntegerBase(*string, tokens[i].width, &base);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    parseNumberFromString(tokens[i], string, ap, base);
  }
}

void processUnsignedToken(token *tokens, int i, char **string, va_list ap,
                          int *argCount, int *flag) {
  *flag = isValidDecimalFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    parseUnsignedFromString(tokens[i], string, ap, 10);
  }
}

void processHexToken(token *tokens, int i, char **string, va_list ap,
                     int *argCount, int *flag) {
  *flag = isValidHexFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    parseUnsignedFromString(tokens[i], string, ap, 16);
  }
}

void processOctalToken(token *const tokens, int i, char **string, va_list ap,
                       int *argCount, int *flag) {
  *flag = isValidOctalFormat(*string, tokens[i].width);
  if (*flag) {
    if (tokens[i].widthType != WIDTH_STAR) (*argCount)++;
    parseUnsignedFromString(tokens[i], string, ap, 8);
  }
}

void skipSpacesAndUpdateToken(char *formatString, int *i, token *tokens,
                              int j) {
  while (isSpace(formatString[*i])) {
    (*i)++;
    tokens[j].wasSpace = 1;
  }
}

int isSpace(char c) {
  int ret = 0;
  if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
      c == '\r') {
    ret = 1;
  }
  return ret;
}
