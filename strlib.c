#include "strlib.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************/

/*
** Definitions that are for internal use by strlib only.
*/

// This is the internal representation of the strlib_str_t
// type, which is given to primitive functions.
struct strlib_str_t {
  size_t length;
  size_t capacity;
  char *chars;
};

/*******************************************************************************/

/*
** Helper functions which are for internal use by strlib only.
*/

static strlib_result_t resize_chars(strlib_str_t *s, size_t additional_cs) {
  // allocate capacity for additional chars if needed
  if ((s->length + additional_cs) >= s->capacity) {
    s->capacity += additional_cs;
    s->chars = (char *)realloc(s->chars, s->capacity);
  }

  // error if space for char array isn't allocated
  if (s->chars == NULL) {
    return (strlib_result_t){
        .code = STRLIB_E_NO_MEMORY,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t validate_insert_position(const strlib_str_t *s,
                                                const size_t position) {
  // error if inserting past length (can insert at end)
  if (position > s->capacity) {
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t shift_chars_right_by_x_from_position(strlib_str_t *s,
                                                            size_t x,
                                                            size_t position) {
  // move chars to make room for insert
  for (size_t i = s->length + x; i >= (position + x); i--) {
    s->chars[i] = s->chars[i - x];
  }

  // update length for additional substr length
  s->length += x;

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t write_characters_to_position(strlib_str_t *s,
                                                    const char *cs,
                                                    const size_t strlen_cs,
                                                    const size_t position,
                                                    const bool reversed) {
  // write cs into position
  for (size_t i = 0; i < strlen_cs; i++) {
    s->chars[i + position] = (reversed) ? cs[strlen_cs - i] : cs[i];
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t validate_can_store_position(
    const size_t num_positions, const size_t positions_size) {
  // raise an error if we are going out of bounds on our positions array
  if (num_positions >= positions_size) {
    return (strlib_result_t){
        .code = STRLIB_E_BAD_SIZE,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t validate_str_slice(const strlib_str_t *s,
                                          const strlib_slice_t slice) {
  // error if trying to get position outside of string
  if (slice.start > s->length || slice.end > s->length) {
    return (strlib_result_t){
        .code = STRLIB_E_BAD_INDEX,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t get_slice_length_and_capacity(const strlib_slice_t slice,
                                                     size_t *length,
                                                     size_t *capacity) {
  // error if buffer is not large enough to hold slice
  size_t req_length = slice.end > slice.start ? (slice.end - slice.start) + 1
                                              : (slice.start - slice.end) + 1;
  *length = req_length;
  *capacity = req_length + 1;
  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t validate_buffer_can_hold_slice(const size_t buf_size,
                                                      const size_t slice_size) {
  // error if buffer is not large enough to hold slice
  if (buf_size < slice_size) {
    return (strlib_result_t){
        .code = STRLIB_E_BAD_SIZE,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t read_characters_from_position(const strlib_str_t *s,
                                                     char *buf,
                                                     const size_t num_to_read,
                                                     const size_t position,
                                                     const bool reversed) {
  // copy the slice from start to finish
  size_t idx = position;

  for (size_t i = 0; i < num_to_read; i++) {
    // copy into buffer, update index depending on slice direction
    buf[i] = (*s).chars[idx];
    idx = reversed ? idx - 1 : idx + 1;
  }
  // write null terminator
  buf[num_to_read] = '\0';

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

static strlib_result_t copy_chars_x_over_left_starting_at_position(
    strlib_str_t *s, const size_t x, const size_t position) {
  // copy all trailing characters from position forward
  for (size_t i = position; i <= (s->length - x); i++) {
    s->chars[i] = s->chars[i + x];
  }

  // replace end characters with null terminator and bump length
  for (size_t i = s->length - x + 1; i <= s->length; i++) s->chars[i] = '\0';
  s->length = s->length - x;

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

/*******************************************************************************/

/*
** Functions which are provided by strlib to be used by external callers.
*/

strlib_result_t strlib_init(strlib_str_t **s) {
  // create space for opaque pointer
  (*s) = (strlib_str_t *)calloc(1, sizeof(strlib_str_t));

  // error if space for opaque pointer cannot be allocated
  if ((*s) == NULL) {
    return (strlib_result_t){
        .code = STRLIB_E_NO_MEMORY,
    };
  }

  // initialize components
  (*s)->length = 0;
  (*s)->capacity = 256;
  (*s)->chars = (char *)calloc(1, sizeof(char[256]));

  // error if space for char array isn't allocated
  if ((*s)->chars == NULL) {
    return (strlib_result_t){
        .code = STRLIB_E_NO_MEMORY,
    };
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_find_char(strlib_str_t *s, size_t *positions,
                                 size_t *num_positions,
                                 const size_t positions_size, const char c) {
  strlib_slice_t *slices = calloc(positions_size, sizeof(strlib_slice_t));
  char substr[2] = {0};
  substr[0] = c;

  strlib_result_t res =
      strlib_find_substr(s, slices, num_positions, positions_size, substr);
  if (res.code != STRLIB_E_SUCCESS) {
    free(slices);
    return res;
  }

  for (size_t i = 0; i <= *num_positions; i++) {
    positions[i] = slices[i].start;
  }

  free(slices);
  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_find_substr(strlib_str_t *s, strlib_slice_t *slices,
                                   size_t *num_positions,
                                   const size_t positions_size,
                                   const char *substr) {
  // initialize substring finding
  char *substr_head = NULL;
  *num_positions = 0;
  substr_head = strstr(s->chars, substr);

  // while there are more substrings
  while (substr_head != NULL) {
    strlib_result_t res =
        validate_can_store_position(*num_positions, positions_size);
    if (res.code != STRLIB_E_SUCCESS) {
      return res;
    }

    // add found position to positions array and increment positions counter
    slices[(*num_positions)++] = (strlib_slice_t){
        .start = (size_t)(substr_head - s->chars),
        .end = (size_t)(substr_head - s->chars) + (strlen(substr) - 1)};

    // attempt to find next substring by incrementing one past current
    // substring
    substr_head = strstr(substr_head + 1, substr);
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_insert_char(strlib_str_t *s, const char c,
                                   const size_t position) {
  return strlib_insert_chars(s, &c, 1, position, false);
}

strlib_result_t strlib_insert_chars(strlib_str_t *s, const char *cs,
                                    const size_t len_cs, const size_t position,
                                    const bool reversed) {
  assert(s);

  strlib_result_t res = validate_insert_position(s, position);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = resize_chars(s, len_cs);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = shift_chars_right_by_x_from_position(s, len_cs, position);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = write_characters_to_position(s, cs, len_cs, position, reversed);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_get(const strlib_str_t *s, char *buf,
                           const size_t size) {
  return strlib_get_slice(s, buf, size,
                          (strlib_slice_t){.start = 0, .end = s->length});
}

strlib_result_t strlib_get_char(const strlib_str_t *s, char *c,
                                const size_t position) {
  // I need a null terminator for uniformity
  char cs[2] = {0};

  strlib_result_t res = strlib_get_slice(
      s, cs, 2, (strlib_slice_t){.start = position, .end = position});
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }
  *c = cs[0];
  return res;
}

strlib_result_t strlib_get_slice(const strlib_str_t *s, char *buf,
                                 const size_t size,
                                 const strlib_slice_t slice) {
  assert(s);
  size_t slice_length = 0;
  size_t slice_capacity = 0;

  strlib_result_t res =
      get_slice_length_and_capacity(slice, &slice_length, &slice_capacity);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = validate_str_slice(s, slice);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = validate_buffer_can_hold_slice(size, slice_capacity);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  res = read_characters_from_position(s, buf, slice_length, slice.start,
                                      (slice.start > slice.end));
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

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
  return strlib_replace_slice(
      s, &c, (strlib_slice_t){.start = position, .end = position});
}

strlib_result_t strlib_replace_slice(strlib_str_t *s, const char *cs,
                                     const strlib_slice_t slice) {
  assert(s);

  strlib_result_t result = strlib_remove_slice(s, slice);
  if (result.code != STRLIB_E_SUCCESS) {
    return result;
  }

  result = strlib_insert_chars(
      s, cs, strlen(cs), (slice.start > slice.end) ? slice.end : slice.start,
      (slice.start > slice.end));
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
  strlib_slice_t *slices = calloc(256, sizeof(strlib_slice_t));
  size_t len_substr = strlen(substr);
  size_t len_cs = strlen(cs);
  size_t offset =
      (len_substr <= len_cs) ? len_cs - len_substr : len_substr - len_cs;

  strlib_result_t result =
      strlib_find_substr(s, &slices[0], &num_positions, 256, substr);

  if (result.code != STRLIB_E_SUCCESS) {
    free(slices);
    return result;
  }

  while (num_positions != 0) {
    for (size_t i = 0; i <= num_positions - 1; i++) {
      // position shifts for existing removed characters
      size_t start = (len_substr <= len_cs) ? (slices[i].start) + (offset * i)
                                            : (slices[i].start) - (offset * i);
      size_t end = (len_substr <= len_cs) ? (slices[i].end) + (offset * i)
                                          : (slices[i].end) - (offset * i);

      result = strlib_replace_slice(
          s, cs, (strlib_slice_t){.start = start, .end = end});

      if (result.code != STRLIB_E_SUCCESS) {
        free(slices);
        return result;
      }
    }

    result = strlib_find_substr(s, &slices[0], &num_positions, 256, substr);
    if (result.code != STRLIB_E_SUCCESS) {
      free(slices);
      return result;
    }
  }

  free(slices);
  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_remove_char(strlib_str_t *s, const size_t position) {
  return strlib_remove_slice(
      s, (strlib_slice_t){.start = position, .end = position});
}

strlib_result_t strlib_remove_slice(strlib_str_t *s,
                                    const strlib_slice_t slice) {
  assert(s);

  strlib_result_t res = validate_str_slice(s, slice);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  size_t start = (slice.start < slice.end) ? slice.start : slice.end;
  size_t end = (slice.start < slice.end) ? slice.end : slice.start;
  size_t size = (end - start) + 1;

  res = copy_chars_x_over_left_starting_at_position(s, size, start);
  if (res.code != STRLIB_E_SUCCESS) {
    return res;
  }

  return (strlib_result_t){
      .code = STRLIB_E_SUCCESS,
  };
}

strlib_result_t strlib_remove_substr(strlib_str_t *s, const char *substr) {
  assert(s);
  size_t num_positions = 0;
  strlib_slice_t *slices = calloc(256, sizeof(strlib_slice_t));

  strlib_result_t result =
      strlib_find_substr(s, &slices[0], &num_positions, 256, substr);
  if (result.code != STRLIB_E_SUCCESS) {
    free(slices);
    return result;
  }

  while (num_positions != 0) {
    for (size_t i = 0; i <= num_positions - 1; i++) {
      // position shifts for existing removed characters
      size_t start = (slices[i].start) - (strlen(substr) * i);
      size_t end = (slices[i].end) - (strlen(substr) * i);

      result =
          strlib_remove_slice(s, (strlib_slice_t){.start = start, .end = end});
      if (result.code != STRLIB_E_SUCCESS) {
        free(slices);
        return result;
      }
    }

    result = strlib_find_substr(s, &slices[0], &num_positions, 256, substr);
    if (result.code != STRLIB_E_SUCCESS) {
      free(slices);
      return result;
    }
  }

  free(slices);
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
    // error if space for char array isn't allocated
    if (s->chars == NULL)
      return (strlib_result_t){
          .code = STRLIB_E_NO_MEMORY,
      };
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
