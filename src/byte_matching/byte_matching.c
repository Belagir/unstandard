
#include <ustd/byte_matching.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

/**
 * @brief
 *
 */
typedef enum __attribute__ ((__packed__)) unstandard_byte_matcher_axiom_flavor_t
{
    BYTE_MATCHER_AXIOM_FLAVOR_IGNORED,
    BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST,
    BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST,
} unstandard_byte_matcher_axiom_flavor_t;

/**
 * @brief
 *
 */
typedef struct unstandard_byte_matcher_axiom_t
{
    unstandard_byte_matcher_axiom_flavor_t flavor;
    u8 expected_value;
} unstandard_byte_matcher_axiom_t;

/**
 * @brief
 *
 */
typedef struct unstandard_byte_matcher_t {
    size_t size_of_filter;
    unstandard_byte_matcher_axiom_t filter[];
} unstandard_byte_matcher_t;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
void
unstandard_byte_matcher_add_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *value, unstandard_byte_matcher_axiom_flavor_t flavor);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
unstandard_byte_matcher_t *
unstandard_byte_matcher_create(void *(*malloc_function)(size_t), size_t size_of_target_type)
{
    unstandard_byte_matcher_t *new_matcher = NULL;

    if (!malloc_function)
    {
        return NULL;
    }

    new_matcher = malloc_function(sizeof(*new_matcher) + (size_of_target_type * sizeof(*(new_matcher->filter))));

    if (new_matcher == NULL)
    {
        return NULL;
    }

    new_matcher->size_of_filter = size_of_target_type;

    for (size_t i = 0u ; i < new_matcher->size_of_filter ; i++)
    {
        new_matcher->filter[i].flavor = BYTE_MATCHER_AXIOM_FLAVOR_IGNORED;
    }

    return new_matcher;
}

// -------------------------------------------------------------------------------------------------
void
unstandard_byte_matcher_add_whitelist_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *expected_value)
{
    unstandard_byte_matcher_add_rule(matcher, value_offset, value_size, expected_value, BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST);
}

// -------------------------------------------------------------------------------------------------
void
unstandard_byte_matcher_add_blacklist_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *rejected_value)
{
    unstandard_byte_matcher_add_rule(matcher, value_offset, value_size, rejected_value, BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST);
}

// -------------------------------------------------------------------------------------------------
i32
unstandard_byte_matcher_test(unstandard_byte_matcher_t *matcher, void *object_tested)
{
    i32 is_matching = 0;
    size_t pos = 0u;
    u8 *object_tested_bytes = NULL;

    if (!matcher || !object_tested)
    {
        return 0;
    }

    object_tested_bytes = (u8 *) object_tested;

    is_matching = 1;
    while ((is_matching) && (pos < matcher->size_of_filter))
    {
        if (matcher->filter[pos].flavor != BYTE_MATCHER_AXIOM_FLAVOR_IGNORED)
        {
            is_matching = ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST) && (matcher->filter[pos].expected_value == object_tested_bytes[pos]))
                    || ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST) && (matcher->filter[pos].expected_value != object_tested_bytes[pos]));
        }

        pos += 1u;
    }
}

// -------------------------------------------------------------------------------------------------
void
unstandard_byte_matcher_destroy(unstandard_byte_matcher_t **matcher, void (*free_function)(void *))
{
    if (!matcher || !*matcher || !free_function)
    {
        return;
    }

    free_function(*matcher);
    *matcher = NULL;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void
unstandard_byte_matcher_add_rule(unstandard_byte_matcher_t *matcher, size_t value_offset, size_t value_size, void *value, unstandard_byte_matcher_axiom_flavor_t flavor)
{
    u8 *value_bytes = NULL;

    if (!matcher || !value || ((value_offset + value_size) >= matcher->size_of_filter))
    {
        return;
    }

    value_bytes = (u8 *) value;

    for (size_t i = 0u ; i < value_size ; i++)
    {
        matcher->filter[i + value_offset].flavor = flavor;
        matcher->filter[i + value_offset].expected_value = value_bytes[i];
    }
}
