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
 * @brief Inserts a value by shallow copy in an array at a specified index. Values at the right of this index are shifted one stride to the right to accomodate.
 * If the index is greater than the current length of the arary, the element is inserted at the back of the array at the first free space.
 * If however the array is at maximum capacity, the element is not inserted and false is returned.
 *
 * @param[inout] array target array
 * @param[in] index insertion index
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the array did not have space
 */
bool array_insert_value(void *array, size_t index, const void *to_value);

/**
 * @brief
 *
 * @param array
 * @param value
 * @return true
 * @return false
 */
bool array_push(void *array, const void *value);

/**
 * @brief
 *
 * @param array
 * @param index
 * @return true
 * @return false
 */
bool array_remove(void *array, size_t index);

/**
 * @brief
 *
 * @param array
 * @return true
 * @return false
 */
bool array_clear(void *array);

/**
 * @brief
 *
 * @param array
 * @param index
 * @param out_value
 * @return true
 * @return false
 */
bool array_get(void *array, size_t index, void *out_value);

/**
 * @brief
 *
 * @param array
 * @return size_t
 */
size_t array_length(const void *array);

/**
 * @brief
 *
 * @param array
 * @return size_t
 */
size_t array_capacity(const void *array);

/**
 * @brief
 *
 * @param alloc
 * @param size_element
 * @param nb_elements_max
 * @return void*
 */
void *array_create(allocator alloc, size_t size_element, size_t nb_elements_max);

/**
 * @brief
 *
 * @param alloc
 * @param array
 */
void array_destroy(allocator alloc, void *array);

/**
 * @brief
 *
 * @param alloc
 * @param array
 * @param additional_capacity
 */
void array_ensure_capacity(allocator alloc, void **array, size_t additional_capacity);

#endif
