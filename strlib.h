/*
** This module provides high level string operations using an opaque pointer
** type referred to as `strlib_str_t`. The implementation is hidden from the
** end user to provide a level of encapsulation that abstracts the details
** away from the operations. From here on objects of this type are referred
** to as strlib strings.
**
** The library provides sets of operations that work with:
**   1) a single strlib sting and primitive characters and/or character arrays
**   2) operations between multiple strlib strings
**
** Modification operations are always guaranteed to not modify arguments which
** are passed to update the string. For instance, if you are inserting an
** array of characters into a string, the library will not modify the string
** which is passed to it.
**
** Meanwhile, operations which are supposed to get something from the string
** expect to be provided with adequate storage for the type of thing which
** you are retrieving. For example, if you want to get the character array
** representation of the string it is expected that you provide an array
** of adequate size. Values are returned explicitly via out parameters.
**
** In order to provide simple error handling, all functions return an integer
** return code which can be checked to make sure that the operation was
** successful.
*/

#ifndef STRLIB_H
#define STRLIB_H

/*******************************************************************************/

/*
** Required definitions used to interface with the library.
*/

// Must use standard c types.
#include <stdbool.h>
#include <stddef.h>

/*
** Type definitions reserved by the library.
*/

// Opaque structure type for strings. The implementation of which is managed
// internally.
typedef struct strlib_str_t strlib_str_t;

// A pair of size_t that are used to define slices.
typedef struct {
  size_t start;  // Beginning index.
  size_t end;    // Ending index.
} strlib_slice_t;

// Result codes returned in the result type. Useful for operation validation.
typedef enum {
  STRLIB_E_SUCCESS,    // Code for success.
  STRLIB_E_NO_MEMORY,  // Code for out of memory.
  STRLIB_E_BAD_SIZE,   // Code for size mismatch.
  STRLIB_E_BAD_INDEX,  // Code for bad index into string.
} strlib_result_code_t;

// Result type for the libary that provides an error code.
// (Stored in a structure for future extension.)
typedef struct {
  strlib_result_code_t code;
} strlib_result_t;

/*******************************************************************************/

/*
** Function definitions which provide operations to the user.
** Section outline is defined as follows.
** Description: A high level description of the function.
** Parameters: A description of each parameter.
** Results: A description of possible results.
** Side Effects: Any side effects that occur during function execution.
*/

/* Description: Initializes a strlib string `s`.
** Parameters:
**     s - A pointer to the memory address where the strlib string is to be
**             held. Note that this is a reference to the ADDRESS where the
**             strlib string will be held, and not a reference to the strlib
**             string itself.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_NO_MEMORY - When the function fails to allocate memory.
** Side Effects:
**     1) An strlib string at the the address stored in the pointer `s`.
*/
strlib_result_t strlib_init(strlib_str_t **s);

/* Description: Finds character `c` in strlib string `s` and stores indicies
**     into array `position`.
** Parameters:
**     s             - A pointer to where the strlib string is to be held.
**     positions     - The indicies where the character should be found.
**     num_positions - The number of positions found.
**     positons_size - The maximum number of positions that can be stored.
**     c             - The character to be found.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_SIZE  - When the positions buffer would be overrun.
** Side Effects:
**     1) The size_t array `positions` is updated with the indicies where
**         `c` can be found.
**     1) The size_t value pointed to `num_positions` is updated with the
**         number of `c` characters that were found.
*/
strlib_result_t strlib_find_char(strlib_str_t *s, size_t *positions,
                                 size_t *num_positions,
                                 const size_t positions_size, const char c);

/* Description: Finds sub-string `substr` in strlib string `s` and stores
**     indicies into array `position`.
** Parameters:
**     s             - A pointer to where the strlib string is to be held.
**     slices        - The slices where the characters should be found.
**     num_positions - The number of positions found.
**     positons_size - The maximum number of positions that can be stored.
**     substr        - the subsequence of chars to be found.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_SIZE  - When the positions buffer would be overrun.
** Side Effects:
**     1) The strlib_slice_t array `slices` is updated with the slices
**         where `substr` can be found.
**     1) The size_t value pointed to `num_positions` is updated with the
**         number of occurences of `substr` that were found.
*/
strlib_result_t strlib_find_substr(strlib_str_t *s, strlib_slice_t *slices,
                                   size_t *num_positions,
                                   const size_t positions_size,
                                   const char *substr);

/* Description: Inserts character `c` into strlib string `s` at index
**     `position`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     c        - The character to be inserted.
**     position - The index where the character should be inserted.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The strlib string `s` is updated with the value `c` appropriately.
*/
strlib_result_t strlib_insert_char(strlib_str_t *s, const char c,
                                   const size_t position);

/* Description: Inserts characters `cs` into strlib string `s` at index
**     `position`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     cs       - The characters to be inserted.
**     slice     - The slice defining the chars to be retieved the string.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The strlib string `s` is updated with the value `cs` appropriately.
*/
strlib_result_t strlib_insert_chars(strlib_str_t *s, const char *cs,
                                    const size_t len_cs, const size_t position,
                                    const bool reversed);

/* Description: Copies the contents of the strlib string `s` into
**     the character array `buf`, up to the size of `size`.
** Parameters:
**     s    - A pointer to where the strlib string is to be held.
**     buf  - The character array location to store the string contents.
**     size - The maximum size of the buffer.
** Results:
**     STRLIB_E_SUCCESS  - When the function exits successfully.
**     STRLIB_E_BAD_SIZE - When the buffer to copy into is too small.
** Side Effects:
**     1) The contents of strlib string `s` are placed into `buf`.
*/
strlib_result_t strlib_get(const strlib_str_t *s, char *buf, const size_t size);

/* Description: Copies the character of the strlib string `s` at
**     position `position` to the character `c`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     c        - The character location to store the string contents.
**     position - The index where the character should be retrieved.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The charcter in position `position` of strlib string `s`
**         is placed into `c`.
*/
strlib_result_t strlib_get_char(const strlib_str_t *s, char *c,
                                const size_t position);

/* Description: Copies the characters of the strlib string `s` from
**     position `start_pos` to position `end_pos` into the character
**     array `buf`, up to size `size`.
** Parameters:
**     s         - A pointer to where the strlib string is to be held.
**     buf       - The character array location to store the string contents.
**     size      - The maximum size of the buffer.
**     slice     - The slice defining the chars to be retieved the string.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
**     STRLIB_E_BAD_SIZE  - When the function finds the target buffer too
**                           small.
** Side Effects:
**     1) The charcters from position `pos.start` to `pos.end` of strlib string
**         `s` are placed into `buf`.
*/
strlib_result_t strlib_get_slice(const strlib_str_t *s, char *buf,
                                 const size_t size, const strlib_slice_t slice);

/* Description: Stores the length of the strlib string `s` in `length`.
** Parameters:
**     s      - A pointer to where the strlib string is to be held.
**     length - The size_t location where length is stored.
** Results:
**     STRLIB_E_SUCCESS - When the function exits successfully.
** Side Effects:
**     1) The length of strlib string `s` is placed into `length`.
*/
strlib_result_t strlib_get_length(const strlib_str_t *s, size_t *length);

/* Description: Stores the capacity of the strlib string `s` in `capacity`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     capacity - The size_t location where capacity is stored.
** Results:
**     STRLIB_E_SUCCESS - When the function exits successfully.
** Side Effects:
**     1) The capacity of strlib string `s` is placed into `capacity`.
*/
strlib_result_t strlib_get_capacity(const strlib_str_t *s, size_t *capacity);

/* Description: Replaces the character of the strlib string `s` at
**     position `position` with the character `c`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     c        - The value to replace with in the strlib string.
**     position - The index where the character should be replaced.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The charcter in position `position` of strlib string `s`
**         is replaced with `c`.
*/
strlib_result_t strlib_replace_char(strlib_str_t *s, const char c,
                                    const size_t position);

/* Description: Replaces the characters of the strlib string `s` at
**     position `position` with the characters `cs`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     cs       - The characters to replace with in the strlib string.
**     slice    - The slice defining the chars to be retieved the string.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The charcter in position `position` of strlib string `s`
**         is replaced with `c`.
*/
strlib_result_t strlib_replace_slice(strlib_str_t *s, const char *cs,
                                     const strlib_slice_t slice);

/* Description: Replaces the characters of the strlib string `s` that
**     match sub-string `substr` with the characters `cs`.
** Parameters:
**     s      - A pointer to where the strlib string is to be held.
**     substr - The characters to replace in the strlib string.
**     cs     - The characters to replace with in the strlib string.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
** Side Effects:
**     1) The charcters matching `substr` in strlib string `s`
**         are replaced with `cs`.
*/
strlib_result_t strlib_replace_substr(strlib_str_t *s, const char *substr,
                                      const char *cs);

/* Description: Remove the character of the strlib string `s` at
**     position `position`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     position - The index where the character should be replaced.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The charcter in position `position` of strlib string `s`
**         is removed.
*/
strlib_result_t strlib_remove_char(strlib_str_t *s, const size_t position);

/* Description: Remove the characters of the strlib string `s` between
**     start position `start_pos` and end position `end_pos`.
** Parameters:
**     s        - A pointer to where the strlib string is to be held.
**     slice    - The slice defining the chars to be retieved the string.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
** Side Effects:
**     1) The charcters from position `start_pos` to `end_pos` of strlib string
**         `s` are removed.
*/
strlib_result_t strlib_remove_slice(strlib_str_t *s,
                                    const strlib_slice_t slice);

/* Description: Removes sub-string `substr` in strlib string `s`.
** Parameters:
**     s             - A pointer to where the strlib string is to be held.
**     substr        - the subsequence of chars to be removed.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
**     STRLIB_E_BAD_SIZE  - When the positions buffer would be overrun.
**                              (propagates from strlib_str_find_substr)
**     STRLIB_E_BAD_INDEX - When the function finds index out of bounds.
**                              (propagates from strlib_str_remove_slice)
** Side Effects:
**     1) The strlib string `s` has occurences of `substr` removed.
*/
strlib_result_t strlib_remove_substr(strlib_str_t *s, const char *substr);

/* Description: Sets the contents of the strlib string `s` using
**     the character array `buf`, up to the size of `size`.
** Parameters:
**     s    - A pointer to where the strlib string to hold the characters.
**     buf  - The character array location with incoming contents.
**     size - The size of the incoming buffer.
** Results:
**     STRLIB_E_SUCCESS  - When the function exits successfully.
** Side Effects:
**     1) The contents of `buf` are placed into strlib string `s`.
*/
strlib_result_t strlib_set(strlib_str_t *s, const char *buf, const size_t size);

/* Description: Destructs a strlib string `s`.
** Parameters:
**     s - A pointer to the memory address where the strlib string is to be
**             held. Note that this is a reference to the ADDRESS where the
**             strlib string will be held, and not a reference to the strlib
**             string itself.
** Results:
**     STRLIB_E_SUCCESS   - When the function exits successfully.
** Side Effects:
**     1) The strlib string at the the address stored in the pointer `s` is
**        destroyed and the pointer is nullified.
*/
strlib_result_t strlib_free(strlib_str_t *s);

#endif  // #ifndef STRLIB_H

/* TODO
** Things on the list for feature development:
** 2) optimize implementations for array inputs
** 3) provide handling with multiple strlib strings as input
*/
