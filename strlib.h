#ifndef STRLIB_H
#define STRLIB_H
#include <stddef.h>

typedef struct strlib_str_t strlib_str_t;

int strlib_str_init(strlib_str_t** s);
int strlib_str_insert_char(strlib_str_t* s, char c, size_t position);
int strlib_str_get(const strlib_str_t* s, char* buf, size_t size);
int strlib_str_get_char(const strlib_str_t* s, char* c, size_t position);
int strlib_str_get_length(const strlib_str_t* s, size_t* length);
int strlib_str_get_capacity(const strlib_str_t* s, size_t* capacity);
int strlib_str_replace_char(strlib_str_t* s, char c, size_t position);
int strlib_str_remove_char(strlib_str_t* s, size_t position);
int strlib_str_set(strlib_str_t* s, const char* buf, size_t size);
int strlib_str_free(strlib_str_t*);

#endif
