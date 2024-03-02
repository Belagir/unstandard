/**
 * @file range.h
 * @author gabriel
 * @brief Create and manipulate statically or dynamically allocated ranges as anonymous structs with an unifying interface.
 *
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __RRANGE_H__
#define __RRANGE_H__

#include <ustd/allocation.h>
#include <ustd/common.h>

/**
 * @brief Type definition of a range holding contiguous, typed values.
 */
#define range(__type, ...) struct { size_t length; size_t capacity; __type data[__VA_ARGS__]; }

/**
 * @brief Initializer of a range of a certain size that will live in the scope it was created in.
 */
#define range_create_static(__type, __capacity, ...) { .length = count_of(((__type[]) __VA_ARGS__)), .capacity = __capacity, .data = __VA_ARGS__ }

/**
 * @brief Initializer of a range of a size determined by the number of passed elements that will live in the scope it was created in.
 */
#define range_create_static_fit(__type, ...) { .length = count_of(((__type[]) __VA_ARGS__)), .capacity = count_of(((__type[]) __VA_ARGS__)), .data = __VA_ARGS__ }

/**
 * @brief Anonymous range used for the methods' abstraction layer.
 *
 */
typedef struct range_anonymous range_anonymous;

/**
 * @brief Any kind of range can be represented by this data structure.
 * This provides an abastraction layer for all operations on ranges this module provides.
 */
typedef struct {
    /// actual target range as an incomplete range of bytes.
    range_anonymous *const r;
    /// size of the original type.
    const size_t stride;
} range_any;

/**
 * @brief Converts a range into a data structure that can be passed to this module's methods. The value created lives on the scope of creation.
 */
#define range_to_any(__range) (range_any) { .r = (range_anonymous *) __range, .stride = sizeof(*(__range)->data) }

/**
 * @brief Inserts a value by shallow copy in a range at a specified index. Values at the right of this index are shifted one stride to the right to accomodate.
 * If the index is greater than the current length of the range, the element is inserted at the back of the range at the first free space.
 * If however the range is at maximum capacity, the element is not inserted and false is returned.
 *
 * @param[inout] target target range
 * @param[in] index insertion index
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the range did not have space
 */
bool range_insert_value(range_any target, size_t index, const void *value);

/**
 * @brief Inserts the values held in a range in another range. Both range should be of the same stride, and the first range should have enough space to store all the values in the second one. Like inserting a value, already present values after the insertion index are shifted to the right.
 *
 * @param[inout] target target range
 * @param[in] index insertion index
 * @param[in] other pointer to the other range
 * @return true if the range's values could be inserted
 * @return false if the target range did not have enough space
 */
bool range_insert_range(range_any target, size_t index, const range_any other);

/**
 * @brief Removes an element from a range by index.
 *
 * @param[inout] target target range
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool range_remove(range_any target, size_t index);

/**
 * @brief Removes several elements fom a range, from the index `from` (included) to the index `to` (excluded).
 * The function either removes all elements specified or none.
 *
 * @param[inout] target target range
 * @param[in] from index from which to start removing
 * @param[in] to index up to which the removal goes
 * @return true if all elements were removed
 * @return false if the bounds provided were invalid
 */
bool range_remove_interval(range_any target, size_t from, size_t to);

/**
 * @brief Clears a range of all its content.
 *
 * @param[inout] r cleared range
 */
void range_clear(range_any target);

/**
 * @brief Search for an element in the range haystack and returns an index to it. If the element is not found, then the length of the range is returned.
 *
 * @param[in] haystack range searched
 * @param[in] comparator traditional comparator function for the elements of the range
 * @param[in] needle pointer to an element that can compare to an element inside the range
 * @param[in] from index from which to search for the element
 * @return size_t index of the element if found, length of the range otherwise
 */
size_t range_index_of(const range_any haystack, comparator_f comparator, const void *needle, size_t from);

/**
 * @brief Counts the number of times the comparator function identify an element in the range that is equal to the needle.
 *
 * @param[in] haystack range searched
 * @param[in] comparator traditional comparator function for the elements of the range
 * @param[in] needle pointer to an element that can compare to an element inside the range
 * @param[in] from index from which to search for the element
 * @return size_tnumber of occurences of the needle
 */
size_t range_count(const range_any haystack, comparator_f comparator, const void *needle, size_t from);

/**
 * @brief Creates a range of a certain size dynamically with an allocator.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] size_element size, in bytes, of a single element
 * @param[in] nb_elements_max maximum number of elements the range will hold
 * @return range* created range
 */
[[nodiscard]]
void *range_create_dynamic(allocator alloc, size_t size_element, size_t nb_elements_max);

/**
 * @brief Frees a range from the allocator it was created with.
 *
 * @param[inout] alloc allocator that was used to create the range
 * @param[inout] r freed range
 */
void range_destroy_dynamic(allocator alloc, range_any *target);

/**
 * @brief Creates a range from existing data. The array is assumed to hold enough elements of the right size in memory to be copied into the range.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] size_element size, in bytes, of a single element
 * @param[in] nb_elements_max maximum number of elements the range will hold
 * @param[in] nb_elements number of elements in the array
 * @param[in] array source array to be copied into the range
 * @return range* created range containing the values in the array
 */
[[nodiscard]]
void *range_create_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, const void *array);

/**
 * @brief Creates a range from an existing range, but assigns a new size to it. This size can be greater or smaller than the original.
 * If the new capacity is smaller than the current length of the source range, the values are truncated.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] target source range
 * @param[in] new_capacity new capacity of the range
 * @return range* created range of a new capacity holding the same values as the original range.
 */
[[nodiscard]]
void *range_create_dynamic_from_resize_of(allocator alloc, const range_any target, size_t new_capacity);

/**
 * @brief Creates a range from the concatenation of two other ranges. Both ranges should be of the same stride.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] r_left range to compose the start of the returned range
 * @param[in] r_right range  to compose the end of the returned range
 * @return range* concatenation of the two ranges
 */
[[nodiscard]]
void *range_create_dynamic_from_concat(allocator alloc, const range_any r_left, const range_any r_right);

/**
 * @brief Returns a copy of the supplied range.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] target source range
 * @return range* copy of the given range
 */
[[nodiscard]]
void *range_create_dynamic_from_copy_of(allocator alloc, const range_any target);

/**
 * @brief Creates a new range from part of another range. Indexes are brought back in bounds if they are beyond the length of the range.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] r target range
 * @param[in] start_index index from which to copy values, included
 * @param[in] end_index  index to which values are copied, excluded
 * @return range* created subrange
 */
[[nodiscard]]
void *range_create_dynamic_from_subrange_of(allocator alloc, const range_any target, size_t start_index, size_t end_index);

/**
 * @brief
 *
 * TODO : unit tests
 *
 * @param range_lhs
 * @param range_rhs
 * @return
 */
i32 range_compare(const range_any *range_lhs, const range_any *range_rhs, comparator_f comp_f);


/**
 * @brief
 *
 * @param alloc
 * @param range
 * @return
 */
void *range_ensure_capacity(allocator alloc, range_any range, size_t additional_capacity);

#ifdef UNITTESTING
void range_experimental_execute_unittests(void);
#endif

#endif
