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

strlib_result_t strlib_init(strlib_str_t **s) {
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

strlib_result_t strlib_find_char(strlib_str_t *s, size_t *positions,
                                 size_t *num_positions,
                                 const size_t positions_size, const char c) {
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

strlib_result_t strlib_find_substr(strlib_str_t *s, strlib_slice_t *positions,
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
    positions[(*num_positions)++] = (strlib_slice_t){
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

strlib_result_t strlib_insert_char(strlib_str_t *s, const char c,
                                   const size_t position) {
  assert(s);

  // error if inserting past length
  if (position > s->length)
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

strlib_result_t strlib_insert_chars(strlib_str_t *s, const char *cs,
                                    const size_t position,
                                    const bool reversed) {
  assert(s);
  size_t strlen_cs = strlen(cs);

  // error if inserting past length
  if (position > s->length)
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };

  // allocate capacity for additional chars if needed
  if ((s->length + strlen_cs) >= s->capacity) {
    s->capacity += strlen_cs;
    s->chars = (char *)realloc(s->chars, s->capacity);
  }

  // move chars to make room for insert
  for (size_t i = s->length + strlen_cs; i >= (position + strlen_cs); i--) {
    s->chars[i] = s->chars[i - strlen_cs];
  }

  // write cs into position
  for (size_t i = 0; i < strlen_cs; i++) {
    s->chars[i + position] = (reversed) ? cs[strlen_cs - i] : cs[i];
  }

  // update length for additional substr length
  s->length += strlen_cs;

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_get(const strlib_str_t *s, char *buf,
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

strlib_result_t strlib_get_char(const strlib_str_t *s, char *c,
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

strlib_result_t strlib_get_chars(const strlib_str_t *s, char *buf,
                                 const size_t size,
                                 const strlib_slice_t position) {
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

strlib_result_t strlib_get_length(const strlib_str_t *s, size_t *length) {
  assert(s);
  *length = s->length;
  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_get_capacity(const strlib_str_t *s, size_t *capacity) {
  assert(s);
  *capacity = s->capacity;
  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_replace_char(strlib_str_t *s, const char c,
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

strlib_result_t strlib_replace_chars(strlib_str_t *s, const char *cs,
                                     const strlib_slice_t position) {
  assert(s);

  bool reversed = (position.start > position.end) ? true : false;
  size_t first_idx =
      (position.start > position.end) ? position.end : position.start;

  strlib_result_t result = strlib_remove_chars(s, position);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  result = strlib_insert_chars(s, cs, first_idx, reversed);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_replace_substr(strlib_str_t *s, const char *substr,
                                      const char *cs) {
  assert(s);
  size_t num_positions = 0;
  strlib_slice_t positions[256] = {0};
  size_t len_substr = strlen(substr);
  size_t len_cs = strlen(cs);
  size_t offset =
      (len_substr <= len_cs) ? len_cs - len_substr : len_substr - len_cs;

  strlib_result_t result =
      strlib_find_substr(s, &positions[0], &num_positions, 256, substr);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  while (num_positions != 0) {
    for (size_t i = 0; i <= num_positions - 1; i++) {
      // position shifts for existing removed characters
      size_t start = (len_substr <= len_cs)
                         ? (positions[i].start) + (offset * i)
                         : (positions[i].start) - (offset * i);
      size_t end = (len_substr <= len_cs) ? (positions[i].end) + (offset * i)
                                          : (positions[i].end) - (offset * i);
      result = strlib_replace_chars(
          s, cs, (strlib_slice_t){.start = start, .end = end});
      if (result.code != STRLIB_E_SUCCESS) {
        return result;
      }
    }

    result = strlib_find_substr(s, &positions[0], &num_positions, 256, substr);
    if (result.code != STRLIB_E_SUCCESS) {
      return result;
    }
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_remove_char(strlib_str_t *s, const size_t position) {
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

strlib_result_t strlib_remove_chars(strlib_str_t *s,
                                    const strlib_slice_t position) {
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

strlib_result_t strlib_remove_substr(strlib_str_t *s, const char *substr) {
  assert(s);
  size_t num_positions = 0;
  strlib_slice_t positions[256] = {0};

  strlib_result_t result =
      strlib_find_substr(s, &positions[0], &num_positions, 256, substr);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  while (num_positions != 0) {
    for (size_t i = 0; i <= num_positions - 1; i++) {
      // position shifts for existing removed characters
      size_t start = (positions[i].start) - (strlen(substr) * i);
      size_t end = (positions[i].end) - (strlen(substr) * i);
      result =
          strlib_remove_chars(s, (strlib_slice_t){.start = start, .end = end});
      if (result.code != STRLIB_E_SUCCESS) {
        return result;
      }
    }

    result = strlib_find_substr(s, &positions[0], &num_positions, 256, substr);
    if (result.code != STRLIB_E_SUCCESS) {
      return result;
    }
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_set(strlib_str_t *s, const char *buf,
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

strlib_result_t strlib_free(strlib_str_t *s) {
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
