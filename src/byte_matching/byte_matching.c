
#include <ustd/byte_matching.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

/**
 * @brief
 *
 */
typedef enum __attribute__ ((__packed__)) byte_matcher_axiom_flavor {
    BYTE_MATCHER_AXIOM_FLAVOR_IGNORED = 0,
    BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST,
    BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST,
} byte_matcher_axiom_flavor;

/**
 * @brief
 *
 */
typedef struct byte_matcher_axiom {
    byte_matcher_axiom_flavor flavor;
    u8 expected_value;
} byte_matcher_axiom;

/**
 * @brief
 *
 */
typedef struct byte_matcher {
    size_t size_of_filter;
    allocator alloc;
    byte_matcher_axiom filter[];
} byte_matcher;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void byte_matcher_add_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *value, byte_matcher_axiom_flavor flavor);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
byte_matcher * byte_matcher_create(allocator alloc, size_t size_of_target_type)
{
    byte_matcher *new_matcher = NULL;

    new_matcher = alloc.malloc(sizeof(*new_matcher) + (size_of_target_type * sizeof(*(new_matcher->filter))));

    if (new_matcher == NULL) {
        return NULL;
    }

    *new_matcher = (byte_matcher) { .alloc = alloc, .size_of_filter = size_of_target_type };

    return new_matcher;
}

// -------------------------------------------------------------------------------------------------
void byte_matcher_add_whitelist_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *expected_value)
{
    byte_matcher_add_rule(matcher, value_offset, value_size, expected_value, BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST);
}

// -------------------------------------------------------------------------------------------------
void byte_matcher_add_blacklist_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *rejected_value)
{
    byte_matcher_add_rule(matcher, value_offset, value_size, rejected_value, BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST);
}

// -------------------------------------------------------------------------------------------------
i32 byte_matcher_test(byte_matcher *matcher, void *object_tested)
{
    i32 is_matching = 0;
    size_t pos = 0u;
    byte *object_tested_bytes = NULL;

    if (!matcher || !object_tested) {
        return 0;
    }

    object_tested_bytes = (byte *) object_tested;

    is_matching = 1;
    while ((is_matching) && (pos < matcher->size_of_filter)) {
        if (matcher->filter[pos].flavor != BYTE_MATCHER_AXIOM_FLAVOR_IGNORED) {
            is_matching = ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST) && (matcher->filter[pos].expected_value == object_tested_bytes[pos]))
                    || ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST) && (matcher->filter[pos].expected_value != object_tested_bytes[pos]));
        }

        pos += 1u;
    }
}

// -------------------------------------------------------------------------------------------------
void byte_matcher_destroy(byte_matcher **matcher)
{
    if (!matcher || !*matcher) {
        return;
    }

    (*matcher)->alloc.free(*matcher);
    *matcher = NULL;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void byte_matcher_add_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *value, byte_matcher_axiom_flavor flavor)
{
    u8 *value_bytes = NULL;

    if (!matcher || !value || ((value_offset + value_size) >= matcher->size_of_filter)) {
        return;
    }

    value_bytes = (u8 *) value;

    for (size_t i = 0u ; i < value_size ; i++) {
        matcher->filter[i + value_offset].flavor = flavor;
        matcher->filter[i + value_offset].expected_value = value_bytes[i];
    }
}

#ifdef UNITTESTING

// void byte_matcher_test

#endif
