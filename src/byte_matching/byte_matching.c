
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
    byte_matcher *new_matcher = { nullptr };

    new_matcher = alloc.malloc(sizeof(*new_matcher) + (size_of_target_type * sizeof(*(new_matcher->filter))));

    if (new_matcher == nullptr) {
        return nullptr;
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
bool byte_matcher_test(byte_matcher *matcher, void *object_tested)
{
    i32 is_matching = { false };
    size_t pos = { 0u };
    u8 *object_tested_bytes = { nullptr };

    if (!matcher || !object_tested) {
        return false;
    }

    object_tested_bytes = (u8 *) object_tested;

    is_matching = true;
    while ((is_matching) && (pos < matcher->size_of_filter)) {
        if (matcher->filter[pos].flavor != BYTE_MATCHER_AXIOM_FLAVOR_IGNORED) {
            is_matching = ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_WHITELIST) && (matcher->filter[pos].expected_value == object_tested_bytes[pos]))
                    || ((matcher->filter[pos].flavor == BYTE_MATCHER_AXIOM_FLAVOR_BLACKLIST) && (matcher->filter[pos].expected_value != object_tested_bytes[pos]));
        }
        pos += 1u;
    }

    return is_matching;
}

// -------------------------------------------------------------------------------------------------
void byte_matcher_destroy(byte_matcher **matcher)
{
    if (!matcher || !*matcher) {
        return;
    }

    (*matcher)->alloc.free(*matcher);
    *matcher = nullptr;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void byte_matcher_add_rule(byte_matcher *matcher, size_t value_offset, size_t value_size, void *value, byte_matcher_axiom_flavor flavor)
{
    u8 *value_bytes = { nullptr };

    if (!matcher || !value || ((value_offset + value_size) > matcher->size_of_filter)) {
        return;
    }

    value_bytes = (u8 *) value;

    for (size_t i = 0u ; i < value_size ; i++) {
        matcher->filter[i + value_offset].flavor = flavor;
        matcher->filter[i + value_offset].expected_value = value_bytes[i];
    }
}

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(byte_match_empty,
        {
            size_t sizeof_type;
        },
        {
            byte_matcher *bm = byte_matcher_create(allocator_system(), data->sizeof_type);
            tst_assert(!byte_matcher_test(bm, nullptr), "nominal test failed");
            byte_matcher_destroy(&bm);
        }
)
tst_CREATE_TEST_CASE(byte_match_empty_test, byte_match_empty, sizeof(long))

tst_CREATE_TEST_SCENARIO(byte_match_blacklist,
        {
            size_t value_offset;
            u8 mask;

            unsigned long tested_value;
            bool expect_success;
        },
        {
            byte_matcher *bm = byte_matcher_create(allocator_system(), sizeof(long));
            byte_matcher_add_blacklist_rule(bm, data->value_offset, sizeof(data->mask), &data->mask);

            tst_assert_equal(data->expect_success, byte_matcher_test(bm, &data->tested_value), "test return of %d");

            byte_matcher_destroy(&bm);
        }
)
tst_CREATE_TEST_CASE(byte_match_blacklist_zero_vs_zero, byte_match_blacklist,       1, 0x00, 0x0000000000000000, false)
tst_CREATE_TEST_CASE(byte_match_blacklist_all_set_vs_zero, byte_match_blacklist,    1, 0xFF, 0x0000000000000000, true)
tst_CREATE_TEST_CASE(byte_match_blacklist_all_set_vs_meh, byte_match_blacklist,     1, 0xFF, 0xFFFFFFFFFFFF00FF, true)
tst_CREATE_TEST_CASE(byte_match_blacklist_all_set_vs_all_set, byte_match_blacklist, 1, 0xFF, 0xFFFFFFFFFFFFFFFF, false)

tst_CREATE_TEST_CASE(byte_match_blacklist_test_n1, byte_match_blacklist,        7, 0x55, 0x5500000000000000, false)
tst_CREATE_TEST_CASE(byte_match_blacklist_test_n2, byte_match_blacklist,        0, 0xB6, 0x00000000000000B6, false)
tst_CREATE_TEST_CASE(byte_match_blacklist_test_n3, byte_match_blacklist,        4, 0x80, 0x000000007F000000, true)
tst_CREATE_TEST_CASE(byte_match_blacklist_test_n4, byte_match_blacklist,        2, 0x01, 0xFFFFFFFFFF07FFFF, false)

void byte_matcher_execute_unittests(void)
{
    tst_run_test_case(byte_match_empty_test);

    tst_run_test_case(byte_match_blacklist_zero_vs_zero);
    tst_run_test_case(byte_match_blacklist_all_set_vs_zero);
    tst_run_test_case(byte_match_blacklist_all_set_vs_meh);
    tst_run_test_case(byte_match_blacklist_all_set_vs_all_set);

    tst_run_test_case(byte_match_blacklist_test_n1);
    tst_run_test_case(byte_match_blacklist_test_n2);
    tst_run_test_case(byte_match_blacklist_test_n3);
    tst_run_test_case(byte_match_blacklist_test_n4);
}

#endif
