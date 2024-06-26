#include "strlib.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct strlib_str_t {
  size_t length;
  size_t capacity;
  char *chars;
};

strlib_result_t strlib_str_init(strlib_str_t **s) {
  // create space for opaque pointer
  (*s) = (strlib_str_t *)calloc(1, sizeof(strlib_str_t));

  // error if space for opaque pointer cannot be allocated
  if ((*s) == NULL)
    return (strlib_result_t){
        .code = STRLIB_E_NO_MEMORY,
    };

  // initialize components
  (*s)->length = 0;
  (*s)->capacity = 256;
  (*s)->chars = (char *)calloc(1, sizeof(char[256]));

  // error if space for char array isn't allocated
  if ((*s)->chars == NULL)
    return (strlib_result_t){
        .code = STRLIB_E_NO_MEMORY,
    };

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_find_char(strlib_str_t *s, size_t *positions,
                                     size_t *num_positions,
                                     const size_t positions_size,
                                     const char c) {
  // initialize num positions found and start walking the string
  *num_positions = 0;
  for (size_t i = 0; i < s->length; i++) {
    // if we find the character
    if (s->chars[i] == c) {
      // raise an error if we are going out of bounds on our positions
      // array
      if (*num_positions >= positions_size)
        return (strlib_result_t){
            .code = STRLIB_E_BAD_SIZE,
        };
      // set the position to the index where the character is found and
      // increment
      positions[(*num_positions)++] = i;
    }
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_find_substr(strlib_str_t *s,
                                       strlib_index_pair_t *positions,
                                       size_t *num_positions,
                                       const size_t positions_size,
                                       const char *substr) {
  // initialize substring finding
  char *substr_head = NULL;
  *num_positions = 0;
  substr_head = strstr(s->chars, substr);

  // while there are more substrings
  while (substr_head != NULL) {
    // raise an error if we are going out of bounds on our positions array
    if (*num_positions >= positions_size) {
      return (strlib_result_t){
          .code = STRLIB_E_BAD_SIZE,
      };
    }

    // add found position to positions array and increment positions counter
    positions[(*num_positions)++] = (strlib_index_pair_t){
        .start = (size_t)(substr_head - s->chars),
        .end = (size_t)(substr_head - s->chars) + (strlen(substr) - 1)};

    // attempt to find next substring by incrementing one past current
    // substring
    substr_head = strstr(++substr_head, substr);
  }

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
  for (size_t i = s->length + strlen(cs); i >= (position + strlen(cs)); i--) {
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
                                     const size_t size,
                                     const strlib_index_pair_t position) {
  assert(s);

  // error if trying to get position outside of string
  if (position.start >= s->length || position.end >= s->length ||
      position.start == position.end)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };

  // error if buffer is not large enough to hold slice
  size_t req_length = position.end > position.start
                          ? (position.end - position.start) + 1
                          : (position.start - position.end) + 1;
  size_t req_capacity = req_length + 1;
  if (size < req_capacity)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_SIZE,
    };

  // copy the slice from start to finish
  size_t idx = position.start;
  for (size_t i = 0; i < req_length; i++) {
    // copy into buffer, update index depending on slice direction
    buf[i] = (*s).chars[idx];
    idx = (position.end > position.start) ? idx + 1 : idx - 1;
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

strlib_result_t strlib_str_replace_chars(strlib_str_t *s, const char *cs,
                                         const strlib_index_pair_t position) {
  assert(s);

  // start and end are inclusive points on the indicies,
  // cs = ddddd, start = 1, end = 5
  // abcdefg
  // 0123456
  //  ddddd
  // strlen("ddddd") = 5
  // (start - end) = 4
  // We therefore have to add one in the comparison
  size_t start = position.start;
  size_t end = position.end;
  size_t position_distance =
      (end > start) ? (end - start) + 1 : (start - end) + 1;

  // error if number of chars to replace does not align with positions
  if (strlen(cs) != position_distance)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_SIZE,
    };

  // error if trying to get position outside of string
  if (position.start >= s->length || position.end >= s->length ||
      position.start == position.end)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };

  // replace the slice from start to finish
  size_t idx = position.start;
  for (size_t i = 0; i < strlen(cs); i++) {
    (*s).chars[idx] = cs[i];
    idx = (position.end > position.start) ? idx + 1 : idx - 1;
  }

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

strlib_result_t strlib_str_remove_chars(strlib_str_t *s,
                                        const strlib_index_pair_t position) {
  assert(s);

  // error if trying to remove position outside of string
  if (position.start >= s->length || position.end >= s->length)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };

  // since we are removing, direction does not matter
  size_t st = (position.start < position.end) ? position.start : position.end;
  size_t e = (position.start < position.end) ? position.end : position.start;
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

strlib_result_t strlib_str_remove_substr(strlib_str_t *s, const char *substr) {
  assert(s);
  size_t num_positions = 0;
  strlib_index_pair_t positions[256] = {0};

  strlib_result_t result =
      strlib_str_find_substr(s, &positions[0], &num_positions, 256, substr);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  while (num_positions != 0) {
    for (size_t i = 0; i <= num_positions - 1; i++) {
      result = strlib_str_remove_chars(
          s, (strlib_index_pair_t){
                 .start = (positions[i].start) - (strlen(substr) * i),
                 .end = (positions[i].end) - (strlen(substr) * i)});
      if (result.code != STRLIB_E_SUCCESS) {
        return result;
      }
    }

    result =
        strlib_str_find_substr(s, &positions[0], &num_positions, 256, substr);
    if (result.code != STRLIB_E_SUCCESS) {
      return result;
    }
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_str_set(strlib_str_t *s, const char *buf,
                               const size_t size) {
  assert(s);

  // set existing characters to null
  memset(s->chars, '\0', (s->capacity) * sizeof(char));

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
