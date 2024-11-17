SHELL := /bin/bash
FLAG = -Wall -Wextra -Werror
FILE = cat_test.txt
CHECK = 2>&1 valgrind --tool=memcheck --leak-check=yes

all: s21_cat

s21_cat: s21_cat.c
	gcc $(FLAG) s21_cat.c -o s21_cat

clean: 
	rm -rf *.out s21_cat

rebuild: clean all

tests:
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -e $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -t $(FILE))
	-diff <(./s21_cat -s $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat --squeeze-blank $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -etsb $(FILE)) <(cat -etsb $(FILE))

