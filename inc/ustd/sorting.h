
#ifndef __SORTING_H__
#define __SORTING_H__

#include <ustd/common.h>
#include <ustd/range.h>

/**
 * @brief Sorts an array of data through the heaps sort. Not stable, but in place.
 *
 * @param[inout] array a valid range.
 * @param[in] comparator a comparison function for the type of the element.
 */
void heapsort_sort(range *array, range_comparator_f comparator);

/**
 * @brief Returns wether a section of memory is sorted (depending on info given by the user).
 *
 * @param[inout] array a valid range.
 * @param[in] comparator a comparison function for the type of the element.
 * @return i32
 */
bool is_sorted(range *array, range_comparator_f comparator);

#ifdef UNITTESTING
void heapsort_execute_unittests(void);
#endif

/**
 * @brief Find the position of an element (needle) in an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[in] haystack valid range.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle  element equal to the one to find.
 * @param[out] out_position theorical (when not found) or real (when found) position of the needle.
 * @return bool 1 if the elment was found, 0 otherwise.
 */
bool sorted_range_find_in(range *haystack, range_comparator_f comparator, void *needle, size_t *out_position);

/**
 * @brief Removes the first occurence of an element (needle) from an anonymous array (haystack) that is assumed to be sorted.
 *
 * @param[inout] haystack valid range.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element equal to the one to remove.
 * @return size_t The position of the deleted element, or the length of the array if not found.
 */
size_t sorted_range_remove_from(range *haystack, range_comparator_f comparator, void *needle);

/**
 * @brief Adds an element (needle) in an anonymous array (haystack) that is assumed to be sorted and to have enough free space after it for one more element.
 *
 * @param[inout] haystack valid range.
 * @param[in] comparator a comparison function for the type of the element.
 * @param[in] needle element to add (by shallow copy).
 * @return size_t The new position of the element.
 */
size_t sorted_range_insert_in(range *haystack, range_comparator_f comparator, void *inserted_needle);

#ifdef UNITTESTING
void sorted_range_execute_unittests(void);
#endif

#endif
