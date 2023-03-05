CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: s21_cat

s21_cat:
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat

clean:
	rm -rf s21_cat

check:
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -i *.c
	cppcheck --enable=all --suppress=missingIncludeSystem *.c

test:
	$(CC) $(CFLAGS) s21_cat.c -o ./test_cat/s21_cat
	sh ./test_cat/test_func_cat.sh
	$(CC) $(CFLAGS) s21_cat.c -o ./test_cat/s21_cat
	sh ./test_cat/test_leak_cat.sh
