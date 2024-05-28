#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "strlib.h"

int main() {
    strlib_str_t* s = NULL;
    char buf[256] = {0};
    char c;
    int ret;
    size_t x;

    ret = strlib_str_init(&s);
    assert(ret == 0);

    ret = strlib_str_get_length(s, &x);
    assert(ret == 0);
    assert(x == 0);

    ret = strlib_str_get_capacity(s, &x);
    assert(ret == 0);
    assert(x == 256);

    ret = strlib_str_set(s, "buf", 4);
    assert(ret == 0);

    ret = strlib_str_get(s, buf, 256);
    assert(ret == 0);
    assert(strcmp(buf, "buf") == 0);

    ret = strlib_str_get_char(s, &c, 2);
    assert(ret == 0);
    assert(c == 'f');

    ret = strlib_str_replace_char(s, 'd', 2);
    assert(ret == 0);
    ret = strlib_str_get(s, buf, 256);
    assert(ret == 0);
    assert(strcmp(buf, "bud") == 0);

    ret = strlib_str_get_length(s, &x);
    assert(ret == 0);
    assert(x == 3);

    ret = strlib_str_get_capacity(s, &x);
    assert(ret == 0);
    assert(x == 256);

    ret = strlib_str_insert_char(s, 'r', 1);
    assert(ret == 0);
    ret = strlib_str_get(s, buf, 256);
    assert(ret == 0);
    assert(strcmp(buf, "brud") == 0);
    ret = strlib_str_get_length(s, &x);
    assert(ret == 0);
    assert(x == 4);

    ret = strlib_str_remove_char(s, 1);
    assert(ret == 0);
    ret = strlib_str_get(s, buf, 256);
    assert(ret == 0);
    assert(strcmp(buf, "bud") == 0);
    ret = strlib_str_get_length(s, &x);
    assert(ret == 0);
    assert(x == 3);

    ret = strlib_str_free(s);
    assert(ret == 0);
}
