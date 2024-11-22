CC = gcc
FLAGS = -Wall -Wextra -Werror -std=c11
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
TEST = test

OS := $(shell uname)
UBUNTU := $(shell hostnamectl | grep -o 'Ubuntu')
MAC_X86 := $(shell uname -a | grep -o _X86_64)
MAC_M1 := $(shell uname -a | grep -o _ARM64)

ifeq ($(OS), Linux)
	TEST_LIBS = -lcheck -pthread -lrt -lm -lsubunit
	MATH = -lm
	OPEN = xdg-open
else ifeq ($(UBUNTU), Ubuntu)
	TEST_FLAGS=-lpthread -lcheck -Wno-nonnull -Wno-memset-transposed-args -lm -lsubunit
	OPEN = open
endif

ifeq ($(MAC_M1), _ARM64)
	TEST_LIBS := -I/opt/homebrew/opt/check/include -L/opt/homebrew/opt/check/lib -lcheck
	OPEN = open
endif
ifeq ($(MAC_X86), _X86_64)
	TEST_LIBS = $(shell pkg-config --libs --cflags check) -lm
	OPEN = open
endif


all: s21_decimal.a

build: all

test: s21_decimal.a 
	$(CC) $(FLAGS) tests/*.c *.c s21_decimal.a $(TEST_LIBS) -o test
	./$(TEST)

retest: clean test

www_flag:
	$(eval CC += $(FLAGS))

s21_decimal.a: www_flag $(OBJECTS)
	ar rcs s21_decimal.a *.o
	ranlib s21_decimal.a
	rm -rf *.o

gcov_flag:
	$(eval FLAGS += --coverage)
gcov_report: clean gcov_flag test
	@mkdir -p report_lcov
	@lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory report_lcov
	rm -rf *.gcno *.gcda *.gcov *.info
	$(OPEN) report_lcov/index.html

style_check:
	cp ../materials/linters/.clang-format ../src/.clang-format
	clang-format -i *.c tests/*.c *.h tests/*.h

debug_help:
	cp ./debug/main.c ./main.c
	$(CC) -g main.c s21_arithmetic.c s21_helper.c s21_comparison.c s21_another.c s21_convertors.c

clean:
	rm -rf *.o *.a *.gcno *.gcda *.info *.html *.css *.png *.out.dSYM main.c *.out
	rm -rf ./tests/*.o ./tests/*.gcda ./tests/*.gcno ./tests/*.html
	rm -rf test src test_build report_lcov build report

rebuild: clean all


git:
	git add *.c
	git add *.h
	git add Makefile
	git commit -m "add from make"
	git push
