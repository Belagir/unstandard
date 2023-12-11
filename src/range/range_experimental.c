
#include <ustd/experimental/range.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param target
 * @param index
 * @param value
 */
static void rrange_set(rrange_any target, size_t index, const void *value);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool rrange_insert_value(rrange_any target, size_t index, const void *value)
{
    if (target.range_impl->length + 1 > target.range_impl->capacity) {
        return false;
    }

    index = min(index, target.range_impl->length);

    for (size_t i = target.range_impl->length ; i > index ; i--) {
        bytewise_copy(target.range_impl->data + (i * target.stride), target.range_impl->data + (i - 1) * target.stride, target.stride);
    }

    rrange_set(target, index, value);
    target.range_impl->length += 1;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool rrange_insert_range(rrange_any target, size_t index, const rrange_any other)
{
    if (((target.range_impl->length + other.range_impl->length) > target.range_impl->capacity) || (target.stride != other.stride)) {
        return false;
    }

    index = min(index, target.range_impl->length);

    for (size_t i = target.range_impl->length ; i > index ; i--) {
        bytewise_copy(target.range_impl->data + ((i + (other.range_impl->length - 1)) * target.stride), target.range_impl->data + (i - 1) * target.stride, target.stride);
    }

    for (size_t i = 0 ; i < other.range_impl->length ; i++) {
        rrange_set(target, index + i, other.range_impl->data + (i * other.stride));
    }
    target.range_impl->length += other.range_impl->length;

    return true;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void rrange_set(rrange_any target, size_t index, const void *value)
{
    bytewise_copy(target.range_impl->data + (index * target.stride), value, target.stride);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(rrange_insert_value,
        {
            rrange(u32, 10u) array;
            u32 inserted_value;
            u32 insertion_pos;

            rrange(u32, 10u) expected_array;
            bool expect_insertion;
        },
        {
            bool success = rrange_insert_value(rrange_to_any(&data->array), data->insertion_pos, &data->inserted_value);
            tst_assert_equal(success, data->expect_insertion, "insertion success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(rrange_insert_in_empty, rrange_insert_value,
        .array              = rrange_create_static(u32, 10),
        .inserted_value     = 99,
        .insertion_pos      = 5,
        .expected_array     = rrange_create_static(u32, 10, 99),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_in_populated_end, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_value     = 99,
        .insertion_pos      = 6,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 99),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_in_populated_end_far, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_value     = 99,
        .insertion_pos      = 9,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 99),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_in_populated_start, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_value     = 99,
        .insertion_pos      = 0,
        .expected_array     = rrange_create_static(u32, 10, 99, 0, 1, 2, 3, 4, 5),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_in_populated_middle, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_value     = 99,
        .insertion_pos      = 3,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 99, 3, 4, 5),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_in_full, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .inserted_value     = 99,
        .insertion_pos      = 8,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(rrange_insert_in_full_far, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .inserted_value     = 99,
        .insertion_pos      = 15,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(rrange_insert_in_full_start, rrange_insert_value,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .inserted_value     = 99,
        .insertion_pos      = 0,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_insertion   = false,
)

tst_CREATE_TEST_SCENARIO(rrange_insert_other_range,
        {
            rrange(u32, 10u) array;
            rrange(u32, 10u) inserted_range;
            u32 insertion_pos;

            rrange(u32, 10u) expected_array;
            bool expect_insertion;
        },
        {
            bool success = rrange_insert_range(rrange_to_any(&data->array), data->insertion_pos, rrange_to_any(&data->inserted_range));
            tst_assert_equal(data->expect_insertion, success, "insertion success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(rrange_insert_other_simple, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10, 0, 1, 2),
        .inserted_range     = rrange_create_static(u32, 10, 3, 4, 5),
        .insertion_pos      = 3,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_other_in_empty, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10),
        .inserted_range     = rrange_create_static(u32, 10, 3, 4, 5),
        .insertion_pos      = 3,
        .expected_array     = rrange_create_static(u32, 10, 3, 4, 5),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_other_too_large, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10, 1),
        .inserted_range     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .insertion_pos      = 0,
        .expected_array     = rrange_create_static(u32, 10, 1),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(rrange_insert_other_empty, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10, 1, 2, 3),
        .inserted_range     = rrange_create_static(u32, 10),
        .insertion_pos      = 0,
        .expected_array     = rrange_create_static(u32, 10, 1, 2, 3),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_other_at_start, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_range     = rrange_create_static(u32, 10, 6, 7, 8, 9),
        .insertion_pos      = 0,
        .expected_array     = rrange_create_static(u32, 10, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(rrange_insert_other_at_middle, rrange_insert_other_range,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
        .inserted_range     = rrange_create_static(u32, 10, 6, 7, 8, 9),
        .insertion_pos      = 3,
        .expected_array     = rrange_create_static(u32, 10, 0, 1, 2, 6, 7, 8, 9, 3, 4, 5),
        .expect_insertion   = true,
)

void rrange_experimental_execute_unittests(void)
{
    tst_run_test_case(rrange_insert_in_empty);
    tst_run_test_case(rrange_insert_in_populated_end);
    tst_run_test_case(rrange_insert_in_populated_end_far);
    tst_run_test_case(rrange_insert_in_populated_start);
    tst_run_test_case(rrange_insert_in_populated_middle);
    tst_run_test_case(rrange_insert_in_full);
    tst_run_test_case(rrange_insert_in_full_far);
    tst_run_test_case(rrange_insert_in_full_start);

    tst_run_test_case(rrange_insert_other_simple);
    tst_run_test_case(rrange_insert_other_in_empty);
    tst_run_test_case(rrange_insert_other_too_large);
    tst_run_test_case(rrange_insert_other_empty);
    tst_run_test_case(rrange_insert_other_at_start);
    tst_run_test_case(rrange_insert_other_at_middle);
}

#endif
