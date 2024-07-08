#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "strlib.h"

static void test_init(void) {
  strlib_str_t *s = NULL;
  strlib_result_t ret1;
  size_t x;

  // test init and free
  ret1 = strlib_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 0);

  ret1 = strlib_get_capacity(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 256);

  ret1 = strlib_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

static void test_set_get(void) {
  strlib_str_t *s = NULL;
  char buf[256] = {0};
  strlib_result_t ret1;

  // test set and get
  ret1 = strlib_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_set(s, "buf", 4);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "buf") == 0);

  ret1 = strlib_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

static void test_char_operations(void) {
  strlib_str_t *s = NULL;
  char buf[256] = {0};
  char c;
  strlib_result_t ret1;
  size_t x;
  size_t size_t_positions[256] = {(size_t)-1};

  ret1 = strlib_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_set(s, "buf", 4);
  assert(ret1.code == STRLIB_E_SUCCESS);

  // test get char
  ret1 = strlib_get_char(s, &c, 2);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(c == 'f');

  // test replace char
  ret1 = strlib_replace_char(s, 'd', 2);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "bud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 3);
  ret1 = strlib_get_capacity(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 256);

  // test insert char
  ret1 = strlib_insert_char(s, 'r', 1);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "brud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 4);

  // test remove char
  ret1 = strlib_remove_char(s, 1);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "bud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 3);

  // test find char
  ret1 = strlib_set(s, "buffer", 7);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_find_char(s, size_t_positions, &x, 256, 'f');
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 2);
  assert(size_t_positions[0] == 2);
  assert(size_t_positions[1] == 3);

  ret1 = strlib_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

static void test_string_operations(void) {
  strlib_str_t *s = NULL;
  char buf[256] = {0};
  strlib_result_t ret1;
  size_t x;
  strlib_slice_t pair_t_positions[256] = {0};

  ret1 = strlib_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  // test get chars
  ret1 = strlib_set(s, "bud", 4);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get_chars(s, buf, 4, (strlib_slice_t){.start = 2, .end = 0});
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "dub") == 0);

  // test insert chars
  ret1 = strlib_insert_chars(s, "air", 0, false);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "airbud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 6);

  // test replace chars
  ret1 =
      strlib_replace_chars(s, "ddddd", (strlib_slice_t){.start = 0, .end = 3});
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 =
      strlib_replace_chars(s, "ddddd", (strlib_slice_t){.start = 0, .end = 4});
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "dddddud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 7);

  // test remove chars
  ret1 = strlib_remove_chars(s, (strlib_slice_t){.start = 0, .end = 4});
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "ud") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 2);

  // test find substring
  ret1 = strlib_set(s, "bufferbuffer", 13);
  ret1 = strlib_find_substr(s, pair_t_positions, &x, 256, "ff");
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 2);
  assert(pair_t_positions[0].start == 2);
  assert(pair_t_positions[0].end == 3);
  assert(pair_t_positions[1].start == 8);
  assert(pair_t_positions[1].end == 9);

  // test remove substring
  ret1 = strlib_remove_substr(s, "ff");
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "buerbuer") == 0);
  ret1 = strlib_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 8);

  // test replace substring
  ret1 = strlib_set(s, "foobarrrrabbarr", 16);
  ret1 = strlib_replace_substr(s, "barr", "baz");
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "foobazrrabbaz") == 0);
  ret1 = strlib_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

int main(void) {
  test_init();
  printf("test_init() passed!\n");
  test_set_get();
  printf("test_set_get() passed!\n");
  test_char_operations();
  printf("test_char_operations() passed!\n");
  test_string_operations();
  printf("test_string_operations() passed!\n");
  return 0;
}
