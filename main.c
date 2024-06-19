#include "strlib.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void test_init(void) {
  strlib_str_t *s = NULL;
  strlib_result_t ret1;
  size_t x;

  ret1 = strlib_str_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 0);

  ret1 = strlib_str_get_capacity(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 256);

  ret1 = strlib_str_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

static void test_set_get(void) {
  strlib_str_t *s = NULL;
  char buf[256] = {0};
  strlib_result_t ret1;

  ret1 = strlib_str_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_str_set(s, "buf", 4);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "buf") == 0);

  ret1 = strlib_str_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

static void test_char_operations(void) {
  strlib_str_t *s = NULL;
  char buf[256] = {0};
  char c;
  strlib_result_t ret1;
  size_t x;
  size_t size_t_positions[256] = {(size_t)-1};
  strlib_index_pair_t pair_t_positions[256] = {0};

  ret1 = strlib_str_init(&s);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_str_set(s, "buf", 4);
  assert(ret1.code == STRLIB_E_SUCCESS);

  ret1 = strlib_str_get_char(s, &c, 2);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(c == 'f');

  ret1 = strlib_str_get_chars(s, buf, 4,
                              (strlib_index_pair_t){.start = 2, .end = 0});
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "fub") == 0);

  ret1 = strlib_str_replace_char(s, 'd', 2);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "bud") == 0);

  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 3);

  ret1 = strlib_str_get_capacity(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 256);

  ret1 = strlib_str_insert_char(s, 'r', 1);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "brud") == 0);
  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 4);

  ret1 = strlib_str_remove_char(s, 1);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "bud") == 0);
  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 3);

  ret1 = strlib_str_insert_chars(s, "air", 0);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "airbud") == 0);
  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 6);

  ret1 =
      strlib_str_remove_chars(s, (strlib_index_pair_t){.start = 0, .end = 4});
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_get(s, buf, 256);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(strcmp(buf, "d") == 0);
  ret1 = strlib_str_get_length(s, &x);
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 1);

  ret1 = strlib_str_set(s, "buffer", 7);
  assert(ret1.code == STRLIB_E_SUCCESS);
  ret1 = strlib_str_find_char(s, size_t_positions, &x, 256, 'f');
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 2);
  assert(size_t_positions[0] == 2);
  assert(size_t_positions[1] == 3);
  ret1 = strlib_str_find_substr(s, pair_t_positions, &x, 256, "ff");
  assert(ret1.code == STRLIB_E_SUCCESS);
  assert(x == 1);
  assert(pair_t_positions[0].start == 2);
  assert(pair_t_positions[0].end == 3);

  ret1 = strlib_str_free(s);
  assert(ret1.code == STRLIB_E_SUCCESS);
}

int main(void) {
  int x[100] = {0};
  printf("x[100] - x[0]: %d\n", (&x[100] - &x[0]));
  test_init();
  printf("test_init() passed!\n");
  test_set_get();
  printf("test_set_get() passed!\n");
  test_char_operations();
  printf("test_char_operations() passed!\n");
  return 0;
}
