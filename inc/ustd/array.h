/**
 * @file array.h
 * @author gabriel
 * @brief Create and manipulate dynamically allocated arrays with a paper-thin interface.
 *
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef UNSTANDARD_ARRAY_H__
#define UNSTANDARD_ARRAY_H__

#include "allocation.h"
#include "common.h"

#define ARRAY(type_) type_ *
#define ARRAY_ANY void *

/**
 * @brief Creates an array of some size by directly allocating its memory.
 *
 * @param[in] alloc allocator to use for the operation
 * @param[in] size_element size, in bytes, of a single element
 * @param[in] nb_elements_max maximum number of elements the array will hold
 * @return void* the array created
 */
ARRAY_ANY array_create(allocator alloc, u32 size_element, size_t nb_elements_max);

/**
 * @brief Frees an array from the allocator it was created with.
 * The pointer to the array given in argument will be set to NULL.
 *
 * @param[in] alloc allocator that was used to create the array
 * @param[inout] array pointer to the freed array
 */
void array_destroy(allocator alloc, ARRAY_ANY *array);

/**
 * @brief Inserts a value by shallow copy in an array at a specified index. Values at the right of this index are shifted one stride to the right to accomodate.
 * If however the array is at maximum capacity, the element is not inserted and false is returned.
 *
 * @param[inout] array target array
 * @param[in] index insertion index
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the array did not have space
 */
bool array_insert_value(ARRAY_ANY array, size_t index, const ARRAY_ANY value);

/**
 * @brief Adds elements found in the other array to the end of the first array.
 * Elements of the second array are assumed to be of the stride of the first one.
 *
 * @param[inout] array target array
 * @param[in] other appended array
 * @return true if the array were concatenated
 * @return false if the target aray did not have space
 */
bool array_append(ARRAY_ANY array, ARRAY_ANY other);

/**
 * @brief
 *
 * @param array
 * @param memory
 * @param nb_elements
 * @return true
 * @return false
 */
bool array_append_mem(ARRAY_ANY array, const void *memory, size_t nb_elements);

/**
 * @brief Pushes a value (by shallow copy of whatever is behind the second pointer) at the end of an array if some space can be found for it.
 * If the value was successfully inserted, true is returned, false otherwise.
 *
 * @param[inout] array target array
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the array did not have space
 */
bool array_push( ARRAY_ANY array, const ARRAY_ANY value);

/**
 * @brief Removes an element from an array by index.
 *
 * @param[inout] array target array
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool array_remove(ARRAY_ANY array, size_t index);

/**
 * @brief Removes an element using the swapback startegy.
 * The last element will take the place of the removed one. Very fast,
 * but, messes indices.
 *
 * @param[inout] array target array
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool array_remove_swapback(ARRAY_ANY array, size_t index);

/**
 * @brief Removes the last element in an array.
 *
 * @param[inout] array target array
 * @return true if the tail element was removed
 * @return false if the array was empty
 */
bool array_pop(ARRAY_ANY array);

/**
 * @brief Clears an array of all its content.
 *
 * @param[inout] array cleared array
 */
void array_clear(ARRAY_ANY array);

/**
 * @brief Copy the content of an array at some index and checks bounds.
 *
 * @param[inout] array target array
 * @param[in] index index fetched
 * @param[out] out_value pointer to some memory where the value at index is copied ; can be NULL
 * @return true if the index is valid, even if out_value is NULL.
 * @return false if the index is out of bounds.
 */
bool array_get(ARRAY_ANY array, size_t index, void *out_value);

/**
 * @brief Iterates through an array and returns the first index for which
 * the comparator function returns 0 when also passed the needle.
 *
 * @param[in] haystack searched array
 * @param[in] comparator comparator function
 * @param[in] needle pointer to the comparison other operand
 * @param[out] out_position filled with the found index.
 * @return true
 * @return false
 */
bool array_find(ARRAY_ANY haystack, comparator_f comparator, void *needle, size_t *out_position);

/**
 * @brief Iterates through an array and returns the last index for which
 * the comparator function returns 0 when also passed the needle.
 *
 * @param[in] haystack searched array
 * @param[in] comparator comparator function
 * @param[in] needle pointer to the comparison other operand
 * @param[out] out_position filled with the found index.
 * @return true
 * @return false
 */
bool array_find_back(ARRAY_ANY haystack, comparator_f comparator, void *needle, size_t *out_position);


/**
 * @brief Returns the current length of an array.
 *
 * @param[in] array target array
 * @return size_t number of elements in the array.
 */
size_t array_length(const ARRAY_ANY array);

/**
 * @brief Returns the current capacity of an array.
 *
 * @param[in] array target array
 * @return size_t maximum number of elements in the array.
 */
size_t array_capacity(const ARRAY_ANY array);

/**
 * @brief Re-allocates an array if it needs extra space to store additional elements.
 *
 * @param[in] alloc allocator used to create the array
 * @param[inout] array target array
 * @param[in] additional_capacity number of supplemental elements the array should be able to hold
 */
void array_ensure_capacity(allocator alloc, ARRAY_ANY *array, size_t additional_capacity);

/**
 * @brief Sorts an array of data with heapsort. Not stable, but in place.
 *
 * @param[inout] array a valid array.
 * @param[in] comparator a comparison function for the type of the element.
 */
void array_sort(ARRAY_ANY array, comparator_f comparator);

/**
 * @brief Returns wether a section of memory is sorted (depending on info given by the user).
 *
 * @param[inout] array a valid array.
 * @param[in] comparator a comparison function for the type of the element.
 * @return i32
 */
bool array_is_sorted(ARRAY_ANY  array, comparator_f comparator);

/**
 * @brief Find the position of an element (needle) in an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[in] haystack valid array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle  element equal to the one to find.
 * @param[out] out_position theorical (when not found) or real (when found) position of the needle.
 * @return bool 1 if the elment was found, 0 otherwise.
 */
bool array_sorted_find(ARRAY_ANY haystack, comparator_f comparator, void *needle, size_t *out_position);

/**
 * @brief Removes the first occurence of an element (needle) from an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[inout] haystack valid array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element equal to the one to remove.
 * @return size_t The position of the deleted element, or the length of the array if not found.
 */
size_t array_sorted_remove(ARRAY_ANY haystack, comparator_f comparator, void *needle);


/**
 * @brief Adds an element (needle) in an anonymous array (haystack) that is assumed to be sorted and to have enough free space after it for one more element.
 *
 * @param[inout] haystack valid array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element to add (by shallow copy).
 * @return size_t The new position of the element.
 */
size_t array_sorted_insert(void *haystack, comparator_f comparator, void *inserted_needle);


#ifdef UNITTESTING
void array_execute_unittests(void);
void array_sort_execute_unittests(void);
#endif

#endif
