
#include <ustd/sorting.h>

#include <math.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
bool sorted_range_find_in(range_any haystack, range_comparator comparator, void *needle, size_t *out_position)
{
    i32 beggining = 0u;
    i32 end = 0u;
    i32 index = 0u;
    i32 comp_result = 2;

    beggining = 0u;
    end = (i32) haystack.r->length - 1;

    while ((beggining <= end) && (comp_result != 0)) {
        index = (i32) ceil_div(beggining + end, 2);

        comp_result = comparator(needle, (void *) (haystack.r->data + ((size_t) index * haystack.stride)));

        if (comp_result == 1) {
            beggining = index + 1;
        }
        else if (comp_result == -1) {
            end = index - 1;
        }
    }

    while ((index > 0) && (comparator(needle, (void *) (haystack.r->data + ((size_t) (index - 1) * haystack.stride))) == 0)) {
        index -= 1;
        comp_result = comparator(needle, (void *) (haystack.r->data + ((size_t) index * haystack.stride)));
    }

    if (out_position != NULL) {
        if ((comp_result == 0) || (comp_result == -1) || (haystack.r->length == 0u)) {
            *out_position = (size_t) index;
        }
        else {
            *out_position = (size_t) (index + comp_result);
        }
    }

    return (comp_result == 0);
}

// -------------------------------------------------------------------------------------------------
size_t sorted_range_remove_from(range_any haystack, range_comparator comparator, void *needle)
{
    u32 found = 0u;
    size_t found_pos = 0u;

    if (!comparator || !needle) {
        return haystack.r->length;
    }

    found = sorted_range_find_in(haystack, comparator, needle, &found_pos);

    if (!found) {
        return haystack.r->length;
    }

    range_remove(haystack, found_pos);

    return found_pos;
}

// -------------------------------------------------------------------------------------------------
size_t sorted_range_insert_in(range_any haystack, range_comparator comparator, void *inserted_needle)
{
    size_t theorical_position = 0u;

    if (!comparator || !inserted_needle)
    {
        return haystack.r->length;
    }

    (void) sorted_range_find_in(haystack, comparator, inserted_needle, &theorical_position);
    range_insert_value(haystack, theorical_position, inserted_needle);

    return theorical_position;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

i32 test_u32_comparator(const void *v1, const void *v2) {
    u32 val1 = *((u32 *) v1);
    u32 val2 = *((u32 *) v2);

    return (val1 > val2) - (val1 < val2);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_u32_find,
        {
            range(u32, 20) array;
            u32 needle;

            size_t expected_position;
            size_t expect_success;
        },
        {
            size_t theorical_pos = 0u;
            size_t found = sorted_range_find_in(range_to_any(&data->array), &test_u32_comparator, (void *) &data->needle, &theorical_pos);

            tst_assert_equal(data->expected_position, theorical_pos, "position %d");
            tst_assert((data->expect_success && (found)) || (!data->expect_success), "element was %sfound",
                        (data->expect_success)? "not " : "");
        }
)

tst_CREATE_TEST_CASE(sorted_array_u32_find_nominal, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_nominal_2, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle            = 14u,
        .expected_position = 14u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_at_start, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle            = 0u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_at_end, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle            = 19u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 301u,
        .expected_position = 12u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_start, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 8u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_end, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 5000u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 390u,
        .expected_position = 16u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_2, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 388u,
        .expected_position = 15u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_before, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 2u,
        .expected_position = 0u,
        .expect_success    = 0u)


tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_after, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u }),
        .needle            = 5001u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_not_found, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle            = 89u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_find_first_occ_adjacent, sorted_array_u32_find,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 13u, 13u, 13u, 17u, 18u, 19u }),
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_remove_element,
        {
            range(u32, 20) array;
            u32 needle;

            range(u32, 20) expected_array;
            u32 expect_deletion;
        },
        {
            size_t deletion_pos = sorted_range_remove_from(range_to_any(&data->array), &test_u32_comparator, (void *) &data->needle);

            tst_assert(((data->expect_deletion && (deletion_pos != 20u)) || (!data->expect_deletion)), "element was %sdeleted",
                        (data->expect_deletion)? "not " : "");

            for (size_t i = 0u ; i < (20u - data->expect_deletion) ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "value of %d", "at index %d", i);
            }
        }
)

tst_CREATE_TEST_CASE(sorted_array_remove_element_nominal, sorted_array_remove_element,
        .array           = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle          = 15u,
        .expected_array  = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 16u, 17u, 18u, 19u, 0u }),
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_nominal_2, sorted_array_remove_element,
        .array           = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle          = 14u,
        .expected_array  = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 15u, 16u, 17u, 18u, 19u, 0u }),
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_at_start, sorted_array_remove_element,
        .array           = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle          = 0u,
        .expected_array  = range_create_static(u32, 20, { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 0u }),
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_at_end, sorted_array_remove_element,
        .array           = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle          = 19u,
        .expected_array  = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 0u }),
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_not_found, sorted_array_remove_element,
        .array           = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .needle          = 20u,
        .expected_array  = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u }),
        .expect_deletion = 0u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_insert,
        {
            range(u32, 20) array;
            size_t array_length;
            u32 inserted_element;

            u32 expected_position;
            range(u32, 20) expected_array;
        },
        {
            data->array.length = data->array_length;
            size_t insertion_pos = sorted_range_insert_in(range_to_any(&data->array), &test_u32_comparator, (void *) &data->inserted_element);

            tst_assert_equal(data->expected_position, insertion_pos, "position %d");

            for (size_t i = 0u ; i < (data->array.length) ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "value of %d", "at index %d", i);
            }
        }
)

tst_CREATE_TEST_CASE(sorted_array_insert_nominal, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u }),
        .array_length      = 15u,
        .inserted_element  = 11u,
        .expected_position = 11u,
        .expected_array    = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u }),
)
tst_CREATE_TEST_CASE(sorted_array_insert_at_beginning, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u }),
        .array_length      = 15u,
        .inserted_element  = 0u,
        .expected_position = 0u,
        .expected_array    = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u }),
)
tst_CREATE_TEST_CASE(sorted_array_insert_second_position, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }),
        .array_length      = 1u,
        .inserted_element  = 2u,
        .expected_position = 1u,
        .expected_array    = range_create_static(u32, 20, { 1u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }),
)
tst_CREATE_TEST_CASE(sorted_array_insert_penultimate, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 1u, 2u, 3u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }),
        .array_length      = 4u,
        .inserted_element  = 4u,
        .expected_position = 3u,
        .expected_array    = range_create_static(u32, 20, { 1u, 2u, 3u, 4u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }),
)
tst_CREATE_TEST_CASE(sorted_array_insert_at_end, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 0u, 0u, 0u, 0u, 0u }),
        .array_length      = 15u,
        .inserted_element  = 15u,
        .expected_position = 15u,
        .expected_array    = range_create_static(u32, 20, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u }),
)
tst_CREATE_TEST_CASE(sorted_array_u32_insert_other, sorted_array_insert,
        .array             = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 0u }),
        .array_length      = 19u,
        .inserted_element  = 390u,
        .expected_position = 16u,
        .expected_array    = range_create_static(u32, 20, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 390u, 395u, 400u, 4025u }),
)
tst_CREATE_TEST_CASE(sorted_array_u32_insert_in_empty, sorted_array_insert,
        .array             = range_create_static(u32, 20, { }),
        .array_length      = 0u,
        .inserted_element  = 42u,
        .expected_position = 0u,
        .expected_array    = range_create_static(u32, 20, { 42u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u }),
)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void
sorted_range_execute_unittests(void)
{
    tst_run_test_case(sorted_array_u32_find_nominal);
    tst_run_test_case(sorted_array_u32_find_nominal_2);

    tst_run_test_case(sorted_array_u32_find_at_start);
    tst_run_test_case(sorted_array_u32_find_at_end);
    tst_run_test_case(sorted_array_u32_find_other_array);

    tst_run_test_case(sorted_array_u32_not_found);
    tst_run_test_case(sorted_array_u32_find_other_array_start);
    tst_run_test_case(sorted_array_u32_find_other_array_end);

    tst_run_test_case(sorted_array_u32_find_other_array_not_found);
    tst_run_test_case(sorted_array_u32_find_other_array_not_found_2);
    tst_run_test_case(sorted_array_u32_find_other_array_not_found_before);
    tst_run_test_case(sorted_array_u32_find_other_array_not_found_after);

    tst_run_test_case(sorted_array_find_first_occ_adjacent);

    tst_run_test_case(sorted_array_remove_element_nominal);
    tst_run_test_case(sorted_array_remove_element_nominal_2);

    tst_run_test_case(sorted_array_remove_element_at_start);
    tst_run_test_case(sorted_array_remove_element_at_end);

    tst_run_test_case(sorted_array_remove_element_not_found);

#if 0
    tst_run_test_case(sorted_array_insert_nominal);
    tst_run_test_case(sorted_array_insert_at_beginning);
    tst_run_test_case(sorted_array_insert_second_position);
    tst_run_test_case(sorted_array_insert_penultimate);
    tst_run_test_case(sorted_array_insert_at_end);
    tst_run_test_case(sorted_array_u32_insert_other);
    tst_run_test_case(sorted_array_u32_insert_in_empty);
#endif
}

#endif
