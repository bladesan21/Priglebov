SHELL := /bin/bash
FLAG = -Wall -Wextra -Werror
FILE = grep_1.txt grep_2.txt
CHECK = 2>&1 valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all
ERROR = ERROR

all: s21_grep

s21_grep: s21_grep.c
	gcc $(FLAG) s21_grep.c -o s21_grep

clean:
	rm -rf *.out s21_grep

rebuild: clean all

tests:
	-diff <(./s21_grep He $(FILE)) <(grep He $(FILE))
	-diff <(./s21_grep -v He $(FILE)) <(grep -v He $(FILE))
	-diff <(./s21_grep -i He $(FILE)) <(grep -i He $(FILE))
	-diff <(./s21_grep -iv He $(FILE)) <(grep -iv He $(FILE))
	-diff <(./s21_grep -in He $(FILE)) <(grep -in He $(FILE))
	-diff <(./s21_grep -ie "^he" $(FILE)) <(grep -ie "^he" $(FILE))
	-diff <(./s21_grep -ince "^he" $(FILE)) <(grep -ince "^he" $(FILE))
	-diff <(./s21_grep -lh se $(FILE)) <(grep -lh se $(FILE))
	-diff <(./s21_grep -hc e $(FILE)) <(grep -hc e $(FILE))
	-diff <(./s21_grep -e He -e he $(FILE)) <(grep -e He -e he $(FILE))
	-diff <(./s21_grep -f $(PATTERNS) $(FILE)) <(grep -f $(PATTERNS) $(FILE))
	-diff <(./s21_grep -ohf $(PATTERNS) $(FILE)) <(grep -ohf $(PATTERNS) $(FILE))