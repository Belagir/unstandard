
#ifndef __BYTE_MATCHING_H__
#define __BYTE_MATCHING_H__

#include <ustd/common.h>
#include <ustd/allocation.h>

/**
 * @brief Byte matcher opaque type. A byte matcher can store an arbitrary filter and test memory against it.
 *
 */
typedef struct byte_matcher byte_matcher;

/**
 * @brief Creates an empty matcher on the heap that can compare against values of a certain byte size.
 *
 * @param[in] alloc
 * @param[in] size_of_target_type
 * @return byte_matcher*
 */
byte_matcher * byte_matcher_create(allocator alloc, size_t size_of_target_type);

/**
 * @brief Adds a whitelist rule to a matcher. The matcher will then return 1 if the value at offset value_offset of size value_size is bytewise equal to the given expected_value.
 *
 * @param[inout] matcher
 * @param[in] value_offset
 * @param[in] value_size
 * @param[in] expected_value
 */
void byte_matcher_add_whitelist_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *expected_value);

/**
 * @brief Adds a whitelist rule to a matcher. The matcher will then return 1 if the value at offset value_offset of size value_size is not bytewise equal to the given expected_value.
 *
 * @param[inout] matcher
 * @param[in] value_offset
 * @param[in] value_size
 * @param[in] rejected_value
 */
void byte_matcher_add_blacklist_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *rejected_value);

/**
 * @brief Tests a memory object against a currently set filter inside a matcher.
 *
 * @param[in] matcher
 * @param[in] object_tested
 * @return i32
 */
i32 byte_matcher_test(byte_matcher *matcher, void *object_tested);

/**
 * @brief Releases resources held by a matcher.
 *
 * @param[inout] matcher
 * @param[in] free_function
 */
void byte_matcher_destroy(byte_matcher **matcher);

#endif
