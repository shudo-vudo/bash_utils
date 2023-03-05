CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: s21_grep

s21_cat:
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep

clean:
	rm -rf s21_grep

check:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -i *.c
	cppcheck --enable=all --suppress=missingIncludeSystem *.c
