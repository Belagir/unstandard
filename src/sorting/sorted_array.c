
#include <ustd/sorting.h>

#include <stdlib.h>

#include <math.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
u32
sorted_array_find_in(void* restrict haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *needle, size_t *out_position)
{
    i32 beggining = 0u;
    i32 end = 0u;
    i32 index = 0u;
    i32 comp_result = 2;

    beggining = 0u;
    end = (i32) length - 1;

    while ((beggining <= end) && (comp_result != 0))
    {
        index = (i32) ceilf(((f32) (beggining + end)) / 2.0f);

        comp_result = comparator(needle, (void *) ((uintptr_t) haystack + (uintptr_t) (index * (i32) size)));

        if (comp_result == 1)
        {
            beggining = index + 1;
        }
        else if (comp_result == -1)
        {
            end = index - 1;
        }
    }

    while ((index > 0) && (comparator(needle, (void *) ((uintptr_t) haystack + (uintptr_t) ((index - 1) * (i32) size))) == 0))
    {
        index -= 1;
        comp_result = comparator(needle, (void *) ((uintptr_t) haystack + (uintptr_t) (index * (i32) size)));
    }

    if (out_position != NULL)
    {
        if ((comp_result == 0) || (comp_result == -1) || (length == 0u))
        {
            *out_position = (size_t) index;
        }
        else
        {
            *out_position = (size_t) (index + comp_result);
        }
    }

    return (comp_result == 0);
}

// -------------------------------------------------------------------------------------------------
size_t
sorted_array_remove_from(void* restrict haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *needle)
{
    u32 found = 0u;
    size_t found_pos = 0u;

    if (!haystack || !comparator || !needle)
    {
        return length;
    }

    found = sorted_array_find_in(haystack, size, length, comparator, needle, &found_pos);

    if (found)
    {
        bytewise_copy((void *) ((uintptr_t) haystack + (uintptr_t) (found_pos * size)),
                      (void *) ((uintptr_t) haystack + (uintptr_t) ((found_pos + 1u) * size)), (length - found_pos) * size);
    }
}

// -------------------------------------------------------------------------------------------------
size_t
sorted_array_insert_in(void* restrict haystack, size_t size, size_t length, i32 (*comparator)(const void*, const void*), void *inserted_needle)
{
    size_t theorical_position = 0u;

    if (!haystack || !comparator || !inserted_needle)
    {
        return length;
    }

    (void) sorted_array_find_in(haystack, size, length, comparator, inserted_needle, &theorical_position);

    for (size_t i = length ; i > theorical_position ; i -= 1u)
    {
        bytewise_copy((void *) ((uintptr_t) haystack + (uintptr_t) (i * size)),
                        (void *) ((uintptr_t) haystack + (uintptr_t) ((i-1u) * size)), size);
    }
    bytewise_copy((void *) ((uintptr_t) haystack + (uintptr_t) (theorical_position * size)), inserted_needle, size);

    return theorical_position;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

i32 test_u32_comparator(const void *v1, const void *v2)
{
    u32 val1 = *((u32 *) v1);
    u32 val2 = *((u32 *) v2);

    return (val1 > val2) - (val1 < val2);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_u32_find,
        {
            u32 array[20u];
            u32 needle;

            size_t expected_position;
            size_t expect_success;
        },
        {
            size_t theorical_pos = 0u;
            size_t found = sorted_array_find_in((void *) data->array, sizeof(*data->array), 20u, &test_u32_comparator, (void *) &data->needle, &theorical_pos);
            tst_assert_equal(data->expected_position, theorical_pos, "position %d");
            tst_assert((data->expect_success && (found)) || (!data->expect_success), "element was %sfound",
                        (data->expect_success)? "not " : "");
        }
)

tst_CREATE_TEST_CASE(sorted_array_u32_find_nominal, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_nominal_2, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle            = 14u,
        .expected_position = 14u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_at_start, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle            = 0u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_at_end, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle            = 19u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 301u,
        .expected_position = 12u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_start, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 8u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_end, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 5000u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 390u,
        .expected_position = 16u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_2, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 388u,
        .expected_position = 15u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_before, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 2u,
        .expected_position = 0u,
        .expect_success    = 0u)


tst_CREATE_TEST_CASE(sorted_array_u32_find_other_array_not_found_after, sorted_array_u32_find,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u },
        .needle            = 5001u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_u32_not_found, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle            = 89u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(sorted_array_find_first_occ_adjacent, sorted_array_u32_find,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 13u, 13u, 13u, 17u, 18u, 19u },
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_remove_element,
        {
            u32 array[20u];
            u32 needle;

            u32 expected_array[20u];
            u32 expect_deletion;
        },
        {
            size_t deletion_pos = sorted_array_remove_from((void *) data->array, sizeof(*data->array), 20u, &test_u32_comparator, (void *) &data->needle);

            tst_assert(((data->expect_deletion && (deletion_pos != 20u)) || (!data->expect_deletion)), "element was %sdeleted",
                        (data->expect_deletion)? "not " : "");

            for (size_t i = 0u ; i < (20u - data->expect_deletion) ; i++)
            {
                tst_assert(data->array[i] == data->expected_array[i], "element at position %d did not match : expected %d , got %d",
                            i, data->array[i], data->expected_array[i]);
            }
        }
)

tst_CREATE_TEST_CASE(sorted_array_remove_element_nominal, sorted_array_remove_element,
        .array           = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle          = 15u,
        .expected_array  = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 16u, 17u, 18u, 19u, 0u },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_nominal_2, sorted_array_remove_element,
        .array           = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle          = 14u,
        .expected_array  = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 15u, 16u, 17u, 18u, 19u, 0u },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_at_start, sorted_array_remove_element,
        .array           = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle          = 0u,
        .expected_array  = { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 0u },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_at_end, sorted_array_remove_element,
        .array           = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle          = 19u,
        .expected_array  = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 0u },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(sorted_array_remove_element_not_found, sorted_array_remove_element,
        .array           = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .needle          = 20u,
        .expected_array  = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u },
        .expect_deletion = 0u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(sorted_array_insert,
        {
            u32 array[20u];
            size_t array_length;
            u32 inserted_element;

            u32 expected_position;
            u32 expected_array[20u];
        },
        {
            size_t insertion_pos = sorted_array_insert_in((void *) data->array, sizeof(*data->array), data->array_length, test_u32_comparator, (void *) &data->inserted_element);

            tst_assert_equal(data->expected_position, insertion_pos, "position %d");

            for (size_t i = 0u ; i < (data->array_length + 1u) ; i++)
            {
                tst_assert(data->array[i] == data->expected_array[i], "element at position %u did not match : expected %u , got %u",
                            i,  data->expected_array[i], data->array[i]);
            }
        }
)

tst_CREATE_TEST_CASE(sorted_array_insert_nominal, sorted_array_insert,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u },
        .array_length      = 15u,
        .inserted_element  = 11u,
        .expected_position = 11u,
        .expected_array    = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u })
tst_CREATE_TEST_CASE(sorted_array_insert_at_beginning, sorted_array_insert,
        .array             = { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u },
        .array_length      = 15u,
        .inserted_element  = 0u,
        .expected_position = 0u,
        .expected_array    = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u })
tst_CREATE_TEST_CASE(sorted_array_insert_second_position, sorted_array_insert,
        .array             = { 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
        .array_length      = 1u,
        .inserted_element  = 2u,
        .expected_position = 1u,
        .expected_array    = { 1u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u })
tst_CREATE_TEST_CASE(sorted_array_insert_penultimate, sorted_array_insert,
        .array             = { 1u, 2u, 3u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
        .array_length      = 4u,
        .inserted_element  = 4u,
        .expected_position = 3u,
        .expected_array    = { 1u, 2u, 3u, 4u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u })
tst_CREATE_TEST_CASE(sorted_array_insert_at_end, sorted_array_insert,
        .array             = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 0u, 0u, 0u, 0u, 0u },
        .array_length      = 15u,
        .inserted_element  = 15u,
        .expected_position = 15u,
        .expected_array    = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u })
tst_CREATE_TEST_CASE(sorted_array_u32_insert_other, sorted_array_insert,
        .array             = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 0u },
        .array_length      = 19u,
        .inserted_element  = 390u,
        .expected_position = 16u,
        .expected_array    = { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 390u, 395u, 400u, 4025u })
tst_CREATE_TEST_CASE(sorted_array_u32_insert_in_empty, sorted_array_insert,
        .array             = { 0u },
        .array_length      = 0u,
        .inserted_element  = 42u,
        .expected_position = 0u,
        .expected_array    = { 42u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void
sorted_array_execute_unittests(void)
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

    tst_run_test_case(sorted_array_insert_nominal);
    tst_run_test_case(sorted_array_insert_at_beginning);
    tst_run_test_case(sorted_array_insert_second_position);
    tst_run_test_case(sorted_array_insert_penultimate);
    tst_run_test_case(sorted_array_insert_at_end);
    tst_run_test_case(sorted_array_u32_insert_other);
    tst_run_test_case(sorted_array_u32_insert_in_empty);
}

#endif
