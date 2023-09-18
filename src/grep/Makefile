all:	s21_grep
s21_grep:
	gcc -Wall -Werror -Wextra ../grep/s21_grep.c -o ../grep/s21_grep -lpcre
clean:
	rm s21_grep
rebuild:
	$(MAKE) clean
	$(MAKE) s21_grep