warns := -Weverything -Werror -Wno-unsafe-buffer-usage -Wno-padded \
	 -Wno-declaration-after-statement

test:
	clang main.c strlib.c -I strlib.h $(warns) -o test

.PHONY: format
format:
	clang-format -i *.[ch]

.PHONY: clean
clean:
	rm test
