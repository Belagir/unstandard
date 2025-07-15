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

/**
 * @brief Creates an array of some size by directly allocating its memory.
 *
 * @param[in] alloc allocator to use for the operation
 * @param[in] size_element size, in bytes, of a single element
 * @param[in] nb_elements_max maximum number of elements the array will hold
 * @return void* the array created
 */
void *array_create(allocator alloc, u32 size_element, size_t nb_elements_max);

/**
 * @brief Frees an array from the allocator it was created with.
 * The pointer to the array given in argument will be set to NULL.
 *
 * @param[in] alloc allocator that was used to create the array
 * @param[inout] array pointer to the freed array
 */
void array_destroy(allocator alloc, void **array);

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
bool array_insert_value(void *array, size_t index, const void *value);

/**
 * @brief Pushes a value (by shallow copy of whatever is behind the second pointer) at the end of an array if some space can be found for it.
 * If the value was successfully inserted, true is returned, false otherwise.
 *
 * @param[inout] array target array
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the array did not have space
 */
bool array_push(void *array, const void *value);

/**
 * @brief Removes an element from an array by index.
 *
 * @param[inout] array target array
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool array_remove(void *array, size_t index);

/**
 * @brief Removes the last element in an array.
 *
 * @param[inout] array target array
 * @return true if the tail element was removed
 * @return false if the array was empty
 */
bool array_pop(void *array);

/**
 * @brief Clears an array of all its content.
 *
 * @param[inout] array cleared array
 */
void array_clear(void *array);

/**
 * @brief Copy the content of an array at some index and checks bounds.
 *
 * @param[inout] array target array
 * @param[in] index index fetched
 * @param[out] out_value pointer to some memory where the value at index is copied ; can be NULL
 * @return true if the index is valid, even if out_value is NULL.
 * @return false if the index is out of bounds.
 */
bool array_get(void *array, size_t index, void *out_value);

/**
 * @brief Returns the current length of an array.
 *
 * @param[in] array target array
 * @return size_t number of elements in the array.
 */
size_t array_length(const void *array);

/**
 * @brief Returns the current capacity of an array.
 *
 * @param[in] array target array
 * @return size_t maximum number of elements in the array.
 */
size_t array_capacity(const void *array);

/**
 * @brief Re-allocates an array if it needs extra space to store additional elements.
 *
 * @param[in] alloc allocator used to create the array
 * @param[inout] array target array
 * @param[in] additional_capacity number of supplemental elements the array should be able to hold
 */
void array_ensure_capacity(allocator alloc, void **array, size_t additional_capacity);

#ifdef UNITTESTING
void array_execute_unittests(void);
#endif

#endif
