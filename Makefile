test:
	clang main.c strlib.c -I strlib.h -o test

.PHONY: clean
clean:
	rm test
