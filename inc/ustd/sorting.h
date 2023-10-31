
#ifndef __SORTING_H__
#define __SORTING_H__

#include <ustd/common.h>

/**
 * @brief Sorts an array of data through the heaps sort. Not stable, but in place.
 *
 * @param[inout] data pointer to the array to sort.
 * @param[in] size size in bytes of the type stored in the array.
 * @param[in] length number of elements in the array.
 * @param[in] comparator a comparison function for the type of the element.
 */
void
heapsort_sort(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*));

/**
 * @brief Returns wether a section of memory is sorted (depending on info given by the user).
 *
 * @param[in] data pointer to an array of data.
 * @param[in] size size in bytes of the type stored in the array.
 * @param[in] length number of elements in the array.
 * @param[in] comparator a comparison function for the type of the element.
 * @return i32
 */
i32
is_sorted(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*));

/**
 * @brief Find the position of an element (needle) in an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[in] haystack pointer to the array.
 * @param[in] size size in bytes of the type stored in the array.
 * @param[in] length number of elements in the array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle  element equal to the one to find.
 * @param[out] out_position theorical (when not found) or real (when found) position of the needle.
 * @return u32 1 if the elment was found, 0 otherwise.
 */
u32
sorted_array_find_in(void *haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *needle, size_t *out_position);

/**
 * @brief Removes the first occurence of an element (needle) from an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[inout] haystack pointer to the array to modify.
 * @param[in] size size in bytes of the type stored in the array.
 * @param[in] length number of elements in the array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element equal to the one to remove.
 * @return size_t The position of the deleted element, or the length of the array if not found.
 */
size_t
sorted_array_remove_from(void *haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *needle);

/**
 * @brief Adds an element (needle) in an anonymous array (haystack) that is assumed to be sorted and to have enough free space after it for one more element.
 *
 * @param[inout] data pointer to the array to modify.
 * @param[in] size size in bytes of the type stored in the array.
 * @param[in] length number of elements in the array.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element to add (by shallow copy).
 * @return size_t The new position of the element.
 */
size_t
sorted_array_insert_in(void *haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *inserted_needle);

#ifdef UNITTESTING
void
sorted_array_execute_unittests(void);
#endif


#endif
