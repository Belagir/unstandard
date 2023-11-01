
#include <ustd/range.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

/**
 * @brief
 *
 * @param r
 * @param index
 * @param value
 */
static void range_set(range *r, size_t index, void *value);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool range_insert(range *r, size_t index, void *value)
{
    if ((r->length == r->capacity) || (index >= r->capacity)) {
        return false;
    }

    index = min(index, r->length);

    for (size_t i = r->length ; i > index ; i--) {
        bytewise_copy(r->data + (i * r->stride), r->data + (i - 1) * r->stride, r->stride);
    }

    range_set(r, index, value);
    r->length += 1;
    return true;
}

// -------------------------------------------------------------------------------------------------
bool range_push_back(range *r, void *value)
{
    return range_insert(r, r->length, value);
}

// -------------------------------------------------------------------------------------------------
bool range_push_front(range *r, void *value)
{
    return range_insert(r, 0, value);
}

// -------------------------------------------------------------------------------------------------
void range_clear(range *r)
{
    r->length = 0;
}

// -------------------------------------------------------------------------------------------------
bool range_remove(range *r, size_t index)
{
    if (index >= r->length) {
        return false;
    }

    for (size_t i = index + 1 ; i < r->length ; i++) {
        bytewise_copy(r->data + (i - 1) * r->stride, r->data + (i * r->stride), r->stride);
    }
    r->length -= 1;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool range_pop_back(range *r)
{
    if (r->length == 0) {
        return false;
    }

    return range_remove(r, r->length - 1);
}

// -------------------------------------------------------------------------------------------------
bool range_pop_front(range *r)
{
    return range_remove(r, 0);
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_create(allocator alloc, size_t size_element, size_t nb_elements_max)
{
    range *r = alloc.malloc(alloc, sizeof(*r) + (size_element * nb_elements_max));
    if (r == nullptr) {
        return nullptr;
    }

    *r = (range) { .stride = size_element, .capacity = nb_elements_max, .length = 0 };

    return r;
}

// -------------------------------------------------------------------------------------------------
void range_dynamic_destroy(allocator alloc, range *r)
{
    alloc.free(alloc, r);
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, void *array)
{
    range *r = range_dynamic_create(alloc, size_element, nb_elements_max);
    if (r == nullptr) {
        return nullptr;
    }

    bytewise_copy(r->data, array, nb_elements * size_element);
    r->length = nb_elements;

    return r;
}

range *range_copy_of(allocator alloc, range *r)
{
    const size_t sizeof_copy = sizeof_range(r);

    range *r_copy = alloc.malloc(alloc, sizeof_copy);
    if (r_copy == nullptr) {
        return nullptr;
    }

    bytewise_copy(r_copy, r, sizeof_copy);

    return r_copy;
}

range *range_concat(allocator alloc, range *r_left, range *r_right)
{
    range *r_concat = range_dynamic_create(alloc, r_left->stride, r_left->capacity + r_right->capacity);
    if (r_concat == nullptr) {
        return nullptr;
    }

    r_concat->length = r_left->length + r_right->length;

    bytewise_copy(r_concat->data, r_left->data, r_left->stride * r_left->length);
    bytewise_copy(r_concat->data + (r_left->stride * r_left->length), r_right->data, r_right->stride * r_right->length);

    return r_concat;
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_from_resize_of(allocator alloc, range *r, size_t new_capacity)
{
    range *new_range = range_dynamic_create(alloc, r->stride, new_capacity);
    if (new_range == nullptr) {
        return r;
    }

    new_range->length = min(r->length, new_range->capacity);
    bytewise_copy(new_range->data, r->data, new_range->length * r->stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void range_set(range *r, size_t index, void *value)
{
    bytewise_copy(r->data + (index * r->stride), value, r->stride);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(range_insert,
        {
            range_static(10, u64) r;
            size_t index_insertion;
            u64 inserted_value;

            bool expect_success;
            range_static(10, u64) r_expected;
        },
        {
            bool success = range_insert((range *) &data->r, data->index_insertion, &data->inserted_value);
            tst_assert_equal(data->expect_success, success, "success of %d");

            tst_assert_equal(data->r_expected.length, data->r.length, "length of %d");

            for (size_t i = 0 ; i < data->r_expected.length ; i++) {
                tst_assert(range_at(&data->r_expected, i, u64) == range_at(&data->r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_at(&data->r_expected, i, u64), range_at(&data->r, i, u64));
            }
        })

tst_CREATE_TEST_CASE(range_insert_in_empty, range_insert,
        .r = range_static_create(10, u64),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42)
)
tst_CREATE_TEST_CASE(range_insert_in_empty_far_index, range_insert,
        .r = range_static_create(10, u64),
        .index_insertion = 6,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42)
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 6,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 42),
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end_far, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 9,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 42),
)
tst_CREATE_TEST_CASE(range_insert_in_populated_start, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42, 0, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_in_populated_middle, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 3,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 42, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_in_full, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 5,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_insert_in_full_far, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 15,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_insert_in_full_start, range_insert,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(range_remove,
        {
            range_static(10, u64) r;
            size_t index_removal;

            bool expect_success;
            range_static(10, u64) r_expected;
        },
        {
            bool success = range_remove((range *) &data->r, data->index_removal);
            tst_assert_equal(data->expect_success, success, "success of %d");

            tst_assert_equal(data->r_expected.length, data->r.length, "length of %d");
            for (size_t i = 0 ; i < data->r_expected.length ; i++) {
                tst_assert(range_at(&data->r_expected, i, u64) == range_at(&data->r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_at(&data->r_expected, i, u64), range_at(&data->r, i, u64));
            }
        })

tst_CREATE_TEST_CASE(range_remove_unique, range_remove,
        .r = range_static_create(10, u64, 42),
        .index_removal = 0,
        .expect_success = true,
        .r_expected = range_static_create(10, u64)
)
tst_CREATE_TEST_CASE(range_remove_start, range_remove,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_removal = 0,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_remove_end, range_remove,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_removal = 9,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8)
)
tst_CREATE_TEST_CASE(range_remove_middle, range_remove,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_removal = 5,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_remove_bad_index, range_remove,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_removal = 6,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_remove_empty, range_remove,
        .r = range_static_create(10, u64),
        .index_removal = 5,
        .expect_success = false,
        .r_expected = range_static_create(10, u64)
)
tst_CREATE_TEST_CASE(range_remove_empty_start, range_remove,
        .r = range_static_create(10, u64),
        .index_removal = 0,
        .expect_success = false,
        .r_expected = range_static_create(10, u64)
)

tst_CREATE_TEST_SCENARIO(range_create_from,
        {
            u64 source[10];
            size_t source_length;

            range_static(10, u64) expected;
        },
        {
            range *r = range_dynamic_from(make_system_allocator(), sizeof(*data->source), count_of(data->source), data->source_length, data->source);

            tst_assert_equal(data->source_length, r->length, "length of %d");
            for (size_t i = 0 ; i < data->expected.length ; i++) {
                tst_assert(range_at(&data->expected, i, u64) == range_at(r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_at(&data->expected, i, u64), range_at(r, i, u64));
            }

            range_dynamic_destroy(make_system_allocator(), r);
        }
)

tst_CREATE_TEST_CASE(range_create_from_full, range_create_from,
        .source = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        .source_length = 10,
        .expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 )
)
tst_CREATE_TEST_CASE(range_create_from_zero, range_create_from,
        .source = { },
        .source_length = 10,
        .expected = range_static_create(10, u64)
)
tst_CREATE_TEST_CASE(range_create_from_empty, range_create_from,
        .source = { },
        .source_length = 0,
        .expected = range_static_create(10, u64)
)

tst_CREATE_TEST_SCENARIO(range_resize,
        {
            range_static(10, u32) original_range;
            size_t resize_to;
        },
        {
            range *r = range_dynamic_from_resize_of(make_system_allocator(), (range *) &data->original_range, data->resize_to);

            tst_assert_equal(data->resize_to, r->capacity, "capacity of %d");

            if (data->resize_to < data->original_range.length) {
                tst_assert_equal(data->resize_to, r->length, "capacity of %d");
            } else {
                tst_assert_equal(data->original_range.length, r->length, "capacity of %d");
            }

            for (size_t i = 0 ; i < r->length ; i++) {
                tst_assert(range_at(&data->original_range, i, u32) == range_at(r, i, u32), "data at index %d mismatch : expected %d, got %d",
                        i, range_at(&data->original_range, i, u32), range_at(r, i, u32));
            }

            range_dynamic_destroy(make_system_allocator(), r);
        }
)

tst_CREATE_TEST_CASE(range_resize_bigger, range_resize,
        .original_range = range_static_create(10, u32, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11),
        .resize_to = 15
)
tst_CREATE_TEST_CASE(range_resize_bigger_from_empty, range_resize,
        .original_range = range_static_create(0, u32),
        .resize_to = 15
)
tst_CREATE_TEST_CASE(range_resize_same_size, range_resize,
        .original_range = range_static_create(10, u32, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11),
        .resize_to = 10
)
tst_CREATE_TEST_CASE(range_resize_smaller, range_resize,
        .original_range = range_static_create(10, u32, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11),
        .resize_to = 6
)
tst_CREATE_TEST_CASE(range_resize_to_empty, range_resize,
        .original_range = range_static_create(10, u32, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11),
        .resize_to = 0
)

void range_execute_unittests(void)
{
    tst_run_test_case(range_insert_in_empty);
    tst_run_test_case(range_insert_in_empty_far_index);
    tst_run_test_case(range_insert_in_populated_end);
    tst_run_test_case(range_insert_in_populated_end_far);
    tst_run_test_case(range_insert_in_populated_start);
    tst_run_test_case(range_insert_in_populated_middle);
    tst_run_test_case(range_insert_in_full);
    tst_run_test_case(range_insert_in_full_far);
    tst_run_test_case(range_insert_in_full_start);

    tst_run_test_case(range_remove_unique);
    tst_run_test_case(range_remove_start);
    tst_run_test_case(range_remove_end);
    tst_run_test_case(range_remove_middle);
    tst_run_test_case(range_remove_bad_index);
    tst_run_test_case(range_remove_empty);
    tst_run_test_case(range_remove_empty_start);

    tst_run_test_case(range_create_from_full);
    tst_run_test_case(range_create_from_zero);
    tst_run_test_case(range_create_from_empty);

    tst_run_test_case(range_resize_bigger);
    tst_run_test_case(range_resize_bigger_from_empty);
    tst_run_test_case(range_resize_same_size);
    tst_run_test_case(range_resize_smaller);
    tst_run_test_case(range_resize_to_empty);
}

#endif