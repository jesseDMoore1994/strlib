#include "strlib.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct strlib_str_t {
  size_t length;
  size_t capacity;
  char *chars;
};

strlib_result_t strlib_str_init(strlib_str_t **s) {

  // create space for opaque pointer
  (*s) = (strlib_str_t *)malloc(sizeof(strlib_str_t));

  // error if space for opaque pointer cannot be allocated
  if ((*s) == NULL)
    return (strlib_result_t){
      .code = STRLIB_E_NO_MEMORY,
    };

  // initialize components
  (*s)->length = 0;
  (*s)->capacity = 256;
  (*s)->chars = (char *)calloc((*s)->capacity, sizeof(char));

  // error if space for char array isn't allocated
  if ((*s)->chars == NULL)
    return (strlib_result_t){
      .code = STRLIB_E_NO_MEMORY,
    };

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_insert_char(strlib_str_t *s, const char c,
                                       const size_t position) {
  assert(s);

  // error if inserting past length
  if (position >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // increase capacity for char if needed
  if ((s->length + 1) == s->capacity) {
    s->capacity++;
    s->chars = (char *)realloc(s->chars, s->capacity);
  }

  // move chars to make room for insert
  for (size_t i = s->length; i > position; i--) {
    s->chars[i] = s->chars[i - 1];
  }

  // insert char and bump length
  s->chars[position] = c;
  s->length++;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_insert_chars(strlib_str_t *s, const char *cs,
                                        const size_t position) {
  assert(s);

  // error if inserting past length
  if (position >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // allocate capacity for additional chars if needed
  if ((s->length + strlen(cs)) >= s->capacity) {
    s->capacity += strlen(cs);
    s->chars = (char *)realloc(s->chars, s->capacity);
  }

  // move chars to make room for insert
  for (size_t i = s->length + strlen(cs); i > position; i--) {
    s->chars[i] = s->chars[i - strlen(cs)];
  }

  // write cs into position
  for (size_t i = 0; i < strlen(cs); i++) {
    s->chars[i + position] = cs[i];
  }

  // update length for additional substr length
  s->length += strlen(cs);

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_get(const strlib_str_t *s, char *buf,
                               const size_t size) {
  assert(s);

  // error if the buffer is to small
  if (size < (s->length) + 1)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_SIZE,
    };

  // copy chars to buffer
  strncpy(buf, s->chars, size);

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_get_char(const strlib_str_t *s, char *c,
                                    const size_t position) {
  assert(s);

  // error if trying to get position outside of string
  if (position >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // store char at address
  *c = s->chars[position];

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_get_chars(const strlib_str_t *s, char *buf,
                                     const size_t size, const size_t start_pos,
                                     const size_t end_pos) {
  assert(s);

  // error if trying to get position outside of string
  if (start_pos >= s->length || end_pos >= s->length || start_pos == end_pos)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // error if buffer is not large enough to hold slice
  size_t req_length = end_pos > start_pos ? (end_pos - start_pos) + 1
                                          : (start_pos - end_pos) + 1;
  size_t req_capacity = req_length + 1;
  if (size < req_capacity)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_SIZE,
    };

  // copy the slice from start to finish
  size_t idx = start_pos;
  for (size_t i = 0; i <= req_length; i++) {
    // copy into buffer, update index depending on slice direction
    buf[i] = s->chars[idx];
    idx = (end_pos > start_pos) ? idx + 1 : idx - 1;
  }
  buf[req_capacity] = '\0';

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_get_length(const strlib_str_t *s, size_t *length) {
  assert(s);
  *length = s->length;
  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_get_capacity(const strlib_str_t *s,
                                        size_t *capacity) {
  assert(s);
  *capacity = s->capacity;
  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_replace_char(strlib_str_t *s, const char c,
                                        const size_t position) {
  assert(s);

  // error if trying to replace position outside of string
  if (position >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // replace char in chars
  s->chars[position] = c;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_remove_char(strlib_str_t *s, const size_t position) {
  assert(s);

  // error if trying to remove position outside of string
  if (position >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // copy all trailing characters from position forward
  for (size_t i = position; i < (s->length); i++) {
    s->chars[i] = s->chars[i + 1];
  }

  // replace end character with null terminator and bump length
  s->chars[s->length] = '\0';
  s->length--;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_remove_chars(strlib_str_t *s, const size_t start_pos,
                                        const size_t end_pos) {
  assert(s);

  //airbud
  //012345
  //  s e

  // error if trying to remove position outside of string
  if (start_pos >= s->length || end_pos >= s->length)
    return (strlib_result_t){
      .code = STRLIB_E_BAD_INDEX,
    };

  // since we are removing, direction does not matter
  size_t st = (start_pos < end_pos) ? start_pos : end_pos;
  size_t e = (start_pos < end_pos) ? end_pos : start_pos;
  size_t d = e - st;

  // copy all trailing characters from position forward
  for (size_t i = st; i <= (s->length - 1 - d); i++) {
    s->chars[i] = s->chars[i + 1 + d];
  }

  // replace end characters with null terminator and bump length
  for (size_t i = s->length - 1; i >= s->length - 1 - d; i--)
      s->chars[i] = '\0';
  s->length = s->length - 1 - d;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_set(strlib_str_t *s, const char *buf,
                               const size_t size) {
  assert(s);

  // allocate more capacity if needed
  if (size > s->capacity) {
    s->capacity = size;
    s->chars = (char *)realloc(s->chars, s->capacity);
  }

  // copy buffer into chars and set length excluding null terminator
  strncpy(s->chars, buf, s->capacity);
  s->length = size - 1;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_free(strlib_str_t *s) {
  assert(s);

  // free internal chars
  free(s->chars);
  // free structure
  free(s);
  // undangle pointer
  s = NULL;

  return (strlib_result_t){
    .code = STRLIB_E_SUCCESS,
  };
}
