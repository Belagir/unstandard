
#ifndef __RANGE_H__
#define __RANGE_H__

#include <ustd/common.h>
#include <ustd/allocation.h>

#define range_of(__type) struct { size_t stride; size_t capacity; size_t length; __type data[]; }

/**
 * @brief Type of a fixed-size range of arbitrary values.
 */
typedef range_of(byte) range;

// accession
#define range_at(__r, __i)  ( ((byte *) (__r)->data) + ((size_t) (__i) * (__r)->stride) )
#define range_at_front(__r) ( range_at(__r, 0) )
#define range_at_back(__r)  ( range_at(__r, (__r)->length - 1) )

#define range_val(__r, __i, __t)  ( *((__t *) (range_at(__r, __i))) )
#define range_val_front(__r, __t) ( range_val(__r, 0, __t) )
#define range_val_back(__r, __t)  ( range_val(__r, (__r)->length - 1, __t) )

/**
 * @brief Size in bytes of a range.
 */
#define sizeof_range(__r) (sizeof(*(__r)) + ((__r)->stride * (__r)->capacity))

/**
 * @brief Type of a static range with a size determined at compile time.
 */
#define range_static(__capacity, __type) struct { size_t stride; size_t capacity; size_t length; __type data[__capacity]; }
/**
 * @brief Creates a static range from values. This range lives in the context it has been created in.
 */
#define range_static_create(__capacity, __type, ...) \
        { \
                .stride = sizeof(__type), \
                .capacity = __capacity,  \
                .length = min(__capacity, count_of(((__type[]) {__VA_ARGS__}))), \
                .data = {__VA_ARGS__} \
        }

#define range_from_static(__capacity, __type, ...) ((range *) &(range_static(__capacity, __type)) range_static_create(__capacity, __type, __VA_ARGS__))

/**
 * @brief Creates a static range from values. This range is created with a length equal to its capacity, which is the count of elements provided.
 */
#define range_static_create_fit(__type, ...) \
        { \
                .stride = sizeof(__type), \
                .capacity = count_of(((__type[]) {__VA_ARGS__})),  \
                .length = count_of(((__type[]) {__VA_ARGS__})), \
                .data = {__VA_ARGS__} \
        }

typedef i32 (*range_comparator)(const void *, const void *);

/**
 * @brief Inserts a value by shallow copy in a range at a specified index. Values at the right of this index are shifted one stride to the right to accomodate.
 * If the index is greater than the current length of the range, the element is inserted at the back of the range at the first free space.
 * If however the range is at maximum capacity, the element is not inserted and false is returned.
 *
 * @param[inout] r target range
 * @param[in] index insertion index
 * @param[in] value pointer to the inserted value
 * @return true if the element was inserted
 * @return false if the range did not have space
 */
bool range_insert_value(range *r, size_t index, const void *value);

/**
 * @brief Inserts the values held in a range in another range. Both range should be of the same stride, and the first range should have enough space to store all the values in the second one. Like inserting a value, already present values after the insertion index are shifted to the right.
 *
 * @param[inout] r target range
 * @param[in] index insertion index
 * @param[in] other pointer to the other range
 * @return true if the range's values could be inserted
 * @return false if the target range did not have enough space
 */
bool range_insert_range(range *r, size_t index, const range *other);

/**
 * @brief Inserts either a value or a range of values into a range.
 *
 * @param[inout] r target range
 * @param[in] index insertion index
 * @param[in] X either a range (range *) or an anonymous pointer (void *)
 * @return true
 * @return false
 */
#define range_insert(r, index, X) _Generic(X, range *: range_insert_range, void *: range_insert_value)(r, index, X)

/**
 * @brief Removes an element from a range by index.
 *
 * @param[inout] r target range
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool range_remove(range *r, size_t index);

/**
 * @brief Removes several elements fom a range, from the index `from` (included) to the index `to` (excluded).
 * The function either removes all elements specified or none.
 *
 * @param[inout] r target range
 * @param[in] from index from which to start removing
 * @param[in] to index up to which the removal goes
 * @return true if all elements were removed
 * @return false if the bounds provided were invalid
 */
bool range_remove_interval(range *r, size_t from, size_t to);

/**
 * @brief Clears a range of all its content.
 *
 * @param[inout] r cleared range
 */
void range_clear(range *r);

/**
 * @brief Creates a range of a certain size dynamically with an allocator.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] size_element size, in bytes, of a single element
 * @param[in] nb_elements_max maximum number of elements the range will hold
 * @return range* created range
 */
[[nodiscard]]
range *range_dynamic_create(allocator alloc, size_t size_element, size_t nb_elements_max);

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
range *range_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, const void *array);

/**
 * @brief Creates a range from an existing range, but assigns a new size to it. This size can be greater or smaller than the original.
 * If the new capacity is smaller than the current length of the source range, the values are truncated.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] r source range
 * @param[in] new_capacity new capacity of the range
 * @return range* created range of a new capacity holding the same values as the original range.
 */
[[nodiscard]]
range *range_dynamic_from_resize_of(allocator alloc, const range *r, size_t new_capacity);

/**
 * @brief Frees a range from the allocator it was created with.
 *
 * @param[inout] alloc allocator that was used to create the range
 * @param[inout] r freed range
 */
void range_dynamic_destroy(allocator alloc, range *r);

// ranges & ranges
/**
 * @brief Creates a range from the concatenation of two other ranges. Both ranges should be of the same stride.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] r_left range to compose the start of the returned range
 * @param[in] r_right range  to compose the end of the returned range
 * @return range* concatenation of the two ranges
 */
[[nodiscard]]
range *range_concat(allocator alloc, const range *r_left, const range *r_right);

/**
 * @brief Returns a copy of the supplied range.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[in] r source range
 * @return range* copy of the given range
 */
[[nodiscard]]
range *range_copy_of(allocator alloc, const range *r);

/**
 * @brief Returns a copy of the supplied range and frees the original. This can be used to transfer ownership explicitely betwween contexts.
 *
 * @param[inout] alloc allocator to use for the operation
 * @param[inout] r source range, freed by the operation
 * @return range* copy of the given range
 */
[[nodiscard]]
range *range_move_of(allocator alloc, range *r);

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
range *range_subrange_of(allocator alloc, const range *r, size_t start_index, size_t end_index);

/**
 * @brief Search for an element in the range haystack and returns an index to it. If the element is not found, then the length of the range is returned.
 *
 * @param[in] haystack range searched
 * @param[in] comparator traditional comparator function for the elements of the range
 * @param[in] needle pointer to an element that can compare to an element inside the range
 * @param[in] from index from which to search for the element
 * @return size_t index of the element if found, length of the range otherwise
 */
size_t range_index_of(const range *haystack, range_comparator comparator, const void *needle, size_t from);

#ifdef UNITTESTING
void range_execute_unittests(void);
#endif
#endif
