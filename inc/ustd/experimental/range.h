
#ifndef __RRANGE_H__
#define __RRANGE_H__

#include <ustd/common.h>

/**
 * @brief Type definition of a range holding contiguous, typed values.
 */
#define rrange(__type, ...) struct { size_t length; size_t capacity; __type data[__VA_ARGS__]; }

typedef rrange(byte) rrange_of_bytes;

/**
 * @brief Declaration of a range of a certain size that will live in the scope it was created in.
 */
#define rrange_create_static(__type, __capacity, ...) { .length = count_of(((__type[]) {__VA_ARGS__})), .capacity = __capacity, .data = { __VA_ARGS__ } }

/**
 * @brief Declaration of a range of a size determined by the number of passed elements that will live in the scope it was created in.
 */
#define rrange_create_static_fit(__type, ...) { .length = count_of(((__type[]) {__VA_ARGS__})), .capacity = count_of(((__type[]) {__VA_ARGS__})), .data = { __VA_ARGS__ } }

/**
 * @brief Any kind of range can be represented by this data structure.
 * This provides an abastraction layer for all operations on ranges this module provides.
 */
typedef struct {
    /// actual target range as an incomplete range of bytes.
    rrange_of_bytes *range_impl;
    /// size of the original type. Change this to create bugs.
    size_t stride;
} rrange_any;

/**
 * @brief Converts a range into a data structure that can be passed to this module's methods. The value created lives on the scope of creation.
 */
#define rrange_to_any(__rrange) (rrange_any) { .range_impl = (rrange_of_bytes *) __rrange, .stride = sizeof(*(__rrange)->data) }

typedef i32 (*rrange_comparator)(const void *, const void *);

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
bool rrange_insert_value(rrange_any target, size_t index, const void *value);

/**
 * @brief Inserts the values held in a range in another range. Both range should be of the same stride, and the first range should have enough space to store all the values in the second one. Like inserting a value, already present values after the insertion index are shifted to the right.
 *
 * @param[inout] target target range
 * @param[in] index insertion index
 * @param[in] other pointer to the other range
 * @return true if the range's values could be inserted
 * @return false if the target range did not have enough space
 */
bool rrange_insert_range(rrange_any target, size_t index, const rrange_any other);

/**
 * @brief Removes an element from a range by index.
 *
 * @param[inout] target target range
 * @param[in] index deletion index
 * @return true if an element was removed at the index
 * @return false if the index was out of bounds
 */
bool rrange_remove(rrange_any target, size_t index);

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
bool rrange_remove_interval(rrange_any target, size_t from, size_t to);

#ifdef UNITTESTING
void rrange_experimental_execute_unittests(void);
#endif

#endif
