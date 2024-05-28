#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "strlib.h"

struct strlib_str_t {
    int length;
    int capacity;
    char* chars;
};

int strlib_str_init(strlib_str_t** s) {
    (*s) = (strlib_str_t*) malloc(sizeof(strlib_str_t));
    if((*s) == NULL) {
        return -1;
    }
    (*s)->length = 0;
    (*s)->capacity = 256;
    (*s)->chars = (char*) calloc((*s)->capacity, sizeof(char));
    if((*s)->chars == NULL) {
        return -1;
    }
    return 0;
}

int strlib_str_insert_char(strlib_str_t* s, char c, size_t position) {
    assert(s);
    if(position > (s->length)) {
        return -1;
    }
    if((s->length+1) == s->capacity) {
        s->capacity++;
        s->chars = (char*) realloc(s->chars, s->capacity);
    }
    for(int i = s->length; i>position; i--){
        s->chars[i] = s->chars[i-1];
    }
    s->chars[position] = c;
    s->length++;
    return 0;
}

int strlib_str_get(const strlib_str_t* s, char* buf, size_t size) {
    assert(s);
    if(size < (s->length)+1) {
        return -1;
    }
    strncpy(buf, s->chars, size);
    return 0;
}

int strlib_str_get_char(const strlib_str_t* s, char* c, size_t position) {
    assert(s);
    if (position > s->length) {
        return -1;
    }
    *c = s->chars[position];
    return 0;
}

int strlib_str_get_length(const strlib_str_t* s, size_t* length) {
    assert(s);
    *length = (*s).length;
    return 0;
}

int strlib_str_get_capacity(const strlib_str_t* s, size_t* capacity) {
    assert(s);
    *capacity = (*s).capacity;
    return 0;
}

int strlib_str_replace_char(strlib_str_t* s, char c, size_t position) {
    assert(s);
    if (position > s->length) {
        return -1;
    }
    s->chars[position] = c;
    return 0;
}

int strlib_str_remove_char(strlib_str_t* s, size_t position) {
    assert(s);
    if(position > (s->length)) {
        return -1;
    }
    for(int i = position; i < (s->length); i++){
        s->chars[i] = s->chars[i+1];
    }
    s->chars[s->length] = '\0';
    s->length--;
    return 0;
}

int strlib_str_set(strlib_str_t* s, const char* buf, size_t size) {
    assert(s);
    if(size > s->capacity) {
        s->capacity = size;
        s->chars = (char*) realloc(s->chars, s->capacity);
    }
    strncpy(s->chars, buf, s->capacity);
    s->length = size - 1;
    return 0;
}

int strlib_str_free(strlib_str_t* s) {
    assert(s);
    free(s->chars);
    free(s);
    s = NULL;
    return 0;
}
