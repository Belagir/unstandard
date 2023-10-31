
#ifndef __BYTE_MATCHING_H__
#define __BYTE_MATCHING_H__

#include <ustd/common.h>

typedef struct unstandard_byte_matcher_t unstandard_byte_matcher_t;

/**
 * @brief Creates an empty matcher on the heap that can compare against values of a certain byte size.
 *
 * @param[in] malloc_function
 * @param[in] size_of_target_type
 * @return unstandard_byte_matcher_t*
 */
unstandard_byte_matcher_t *
unstandard_byte_matcher_create(void *(*malloc_function)(size_t), size_t size_of_target_type);

/**
 * @brief Adds a whitelist rule to a matcher. The matcher will then return 1 if the value at offset value_offset of size value_size is bytewise equal to the given expected_value.
 *
 * @param[inout] matcher
 * @param[in] value_offset
 * @param[in] value_size
 * @param[in] expected_value
 */
void
unstandard_byte_matcher_add_whitelist_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *expected_value);

/**
 * @brief Adds a whitelist rule to a matcher. The matcher will then return 1 if the value at offset value_offset of size value_size is not bytewise equal to the given expected_value.
 *
 * @param[inout] matcher
 * @param[in] value_offset
 * @param[in] value_size
 * @param[in] rejected_value
 */
void
unstandard_byte_matcher_add_blacklist_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *rejected_value);

/**
 * @brief Tests a memory object against a currently set filter inside a matcher.
 *
 * @param[in] matcher
 * @param[in] object_tested
 * @return i32
 */
i32
unstandard_byte_matcher_test(unstandard_byte_matcher_t *matcher, void *object_tested);

/**
 * @brief Releases resources held by a matcher.
 *
 * @param[inout] matcher
 * @param[in] free_function
 */
void
unstandard_byte_matcher_destroy(unstandard_byte_matcher_t **matcher, void (*free_function)(void *));


#endif
