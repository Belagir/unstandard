
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
static void range_set(range *r, size_t index, const void *value);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool range_insert_value(range *r, size_t index, const void *value)
{
    if (r->length + 1 > r->capacity) {
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
bool range_insert_range(range *r, size_t index, const range *other)
{
    if (((r->length + other->length) > r->capacity) || (r->stride != r->stride)) {
        return false;
    }

    index = min(index, r->length);

    for (size_t i = r->length ; i > index ; i--) {
        bytewise_copy(r->data + ((i + (other->length - 1)) * r->stride), r->data + (i - 1) * r->stride, r->stride);
    }

    for (size_t i = 0 ; i < other->length ; i++) {
        range_set(r, index + i, range_at(other, i));
    }
    r->length += other->length;

    return true;
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
range *range_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, const void *array)
{
    range *r = range_dynamic_create(alloc, size_element, nb_elements_max);
    if (r == nullptr) {
        return nullptr;
    }

    bytewise_copy(r->data, array, nb_elements * size_element);
    r->length = nb_elements;

    return r;
}

// -------------------------------------------------------------------------------------------------
range *range_copy_of(allocator alloc, const range *r)
{
    const size_t sizeof_copy = sizeof_range(r);

    range *r_copy = alloc.malloc(alloc, sizeof_copy);
    if (r_copy == nullptr) {
        return nullptr;
    }

    bytewise_copy(r_copy, r, sizeof_copy);

    return r_copy;
}

// -------------------------------------------------------------------------------------------------
range *range_move_of(allocator alloc, range *r)
{
    range *moved_range = range_copy_of(alloc, r);
    if (moved_range == nullptr) {
        return nullptr;
    }

    alloc.free(alloc, r);

    return moved_range;
}

// -------------------------------------------------------------------------------------------------
range *range_concat(allocator alloc, const range *r_left, const range *r_right)
{
    if (r_left->stride != r_right->stride) {
        return nullptr;
    }

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
range *range_dynamic_from_resize_of(allocator alloc, const range *r, size_t new_capacity)
{
    range *new_range = range_dynamic_create(alloc, r->stride, new_capacity);
    if (new_range == nullptr) {
        return nullptr;
    }

    new_range->length = min(r->length, new_range->capacity);
    bytewise_copy(new_range->data, r->data, new_range->length * r->stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
range *range_subrange_of(allocator alloc, const range *r, size_t start_index, size_t end_index)
{
    size_t nb_copied_elements = 0;

    if ((start_index >= end_index) || (end_index > r->length)) {
        return nullptr;
    }

    nb_copied_elements = end_index - start_index + 1;
    range *sub_range = range_dynamic_from(alloc, r->stride, nb_copied_elements, nb_copied_elements, range_at(r, start_index));

    return sub_range;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void range_set(range *r, size_t index, const void *value)
{
    bytewise_copy(r->data + (index * r->stride), value, r->stride);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(range_insert_value,
        {
            range_static(10, u64) r;
            size_t index_insertion;
            u64 inserted_value;

            bool expect_success;
            range_static(10, u64) r_expected;
        },
        {
            bool success = range_insert_value((range *) &data->r, data->index_insertion, &data->inserted_value);
            tst_assert_equal(data->expect_success, success, "success of %d");

            tst_assert_equal(data->r_expected.length, data->r.length, "length of %d");

            for (size_t i = 0 ; i < data->r_expected.length ; i++) {
                tst_assert(range_val(&data->r_expected, i, u64) == range_val(&data->r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->r_expected, i, u64), range_val(&data->r, i, u64));
            }
        })

tst_CREATE_TEST_CASE(range_insert_in_empty, range_insert_value,
        .r = range_static_create(10, u64),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42)
)
tst_CREATE_TEST_CASE(range_insert_in_empty_far_index, range_insert_value,
        .r = range_static_create(10, u64),
        .index_insertion = 6,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42)
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 6,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 42),
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end_far, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 9,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 42),
)
tst_CREATE_TEST_CASE(range_insert_in_populated_start, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 42, 0, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_in_populated_middle, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5),
        .index_insertion = 3,
        .inserted_value = 42,
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 42, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_in_full, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 5,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_insert_in_full_far, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 15,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(range_insert_in_full_start, range_insert_value,
        .r = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .index_insertion = 0,
        .inserted_value = 42,
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_insert_other,
        {
            range_static(10, u64) r;
            size_t index_insertion;
            range_static(10, u64) other;

            bool expect_success;
            range_static(10, u64) r_expected;
        },
        {
            bool success = range_insert_range((range *) &data->r, data->index_insertion, (range *) &data->other);
            tst_assert_equal(data->expect_success, success, "success of %d");

            tst_assert_equal(data->r_expected.length, data->r.length, "length of %d");

            for (size_t i = 0 ; i < data->r_expected.length ; i++) {
                tst_assert(range_val(&data->r_expected, i, u64) == range_val(&data->r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->r_expected, i, u64), range_val(&data->r, i, u64));
            }
        }
)

tst_CREATE_TEST_CASE(range_insert_other_simple, range_insert_other,
        .r = range_static_create(10, u64, 0, 1, 2),
        .index_insertion = 3,
        .other = range_static_create(10, u64, 3, 4, 5),
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 0, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_other_in_empty, range_insert_other,
        .r = range_static_create(10, u64),
        .index_insertion = 0,
        .other = range_static_create(10, u64, 3, 4, 5),
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_other_too_large, range_insert_other,
        .r = range_static_create(10, u64, 1),
        .index_insertion = 0,
        .other = range_static_create(10, u64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_success = false,
        .r_expected = range_static_create(10, u64, 1)
)
tst_CREATE_TEST_CASE(range_insert_other_empty, range_insert_other,
        .r = range_static_create(10, u64, 1, 2, 3, 4, 5),
        .index_insertion = 6,
        .other = range_static_create(10, u64),
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_other_start, range_insert_other,
        .r = range_static_create(10, u64, 1, 2, 3, 4, 5),
        .index_insertion = 0,
        .other = range_static_create(10, u64, 8, 7, 6),
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 8, 7, 6, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(range_insert_other_middle, range_insert_other,
        .r = range_static_create(10, u64, 1, 2, 3, 4, 5),
        .index_insertion = 2,
        .other = range_static_create(10, u64, 8, 7, 6),
        .expect_success = true,
        .r_expected = range_static_create(10, u64, 1, 2, 8, 7, 6, 3, 4, 5)
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
                tst_assert(range_val(&data->r_expected, i, u64) == range_val(&data->r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->r_expected, i, u64), range_val(&data->r, i, u64));
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
                tst_assert(range_val(&data->expected, i, u64) == range_val(r, i, u64), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->expected, i, u64), range_val(r, i, u64));
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
                tst_assert(range_val(&data->original_range, i, u32) == range_val(r, i, u32), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->original_range, i, u32), range_val(r, i, u32));
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

tst_CREATE_TEST_SCENARIO(range_concat,
        {
            range_static(10, u32) r_left;
            range_static(10, u32) r_right;

            range_static(20, u32) r_expected;
        },
        {
            range *r = range_concat(make_system_allocator(), (range *) &data->r_left, (range *) &data->r_right);

            tst_assert_equal(data->r_expected.length, r->length, "length of %d");
            for (size_t i = 0 ; i < r->length ; i++) {
                tst_assert(range_val(&data->r_expected, i, u32) == range_val(r, i, u32), "data at index %d mismatch : expected %d, got %d",
                        i, range_val(&data->r_expected, i, u32), range_val(r, i, u32));
            }

            range_dynamic_destroy(make_system_allocator(), r);
        }
)

tst_CREATE_TEST_CASE(range_concat_full, range_concat,
        .r_left = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .r_right = range_static_create(10, u32, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
        .r_expected = range_static_create(20, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
)
tst_CREATE_TEST_CASE(range_concat_notfull, range_concat,
        .r_left = range_static_create(10, u32, 0, 1, 2, 3, 4, 5),
        .r_right = range_static_create(10, u32, 6, 7, 8, 9),
        .r_expected = range_static_create(20, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
)
tst_CREATE_TEST_CASE(range_concat_one_empty, range_concat,
        .r_left = range_static_create(10, u32, 0, 1, 2, 3, 4, 5),
        .r_right = range_static_create(10, u32),
        .r_expected = range_static_create(20, u32, 0, 1, 2, 3, 4, 5),
)
tst_CREATE_TEST_CASE(range_concat_two_empty, range_concat,
        .r_left = range_static_create(10, u32),
        .r_right = range_static_create(10, u32),
        .r_expected = range_static_create(20, u32),
)

tst_CREATE_TEST_SCENARIO(range_get_subrange,
        {
            range_static(10, u32) r_source;
            size_t begin;
            size_t end;

            bool expect_success;
            range_static(10, u32) r_expected;
        },
        {
            range *sub_rg = range_subrange_of(make_system_allocator(), (range *) &data->r_source, data->begin, data->end);

            if (data->expect_success && (sub_rg == nullptr)) {
                tst_assert(false, "sub range is null but shouldn't");
            } else if (!data->expect_success && (sub_rg != nullptr)){
                tst_assert(false, "sub range is not null but should be (length of %d)", sub_rg->length);
            } else if (data->expect_success) {
                for (size_t i = 0 ; i < data->r_expected.length ; i++ ) {
                    tst_assert(range_val(&data->r_expected, i, u32) == range_val(sub_rg, i, u32), "data at index %i mismatch : expected %d, got %d",
                            i, range_val(&data->r_expected, i, u32), range_val(sub_rg, i, u32));
                }

                range_dynamic_destroy(make_system_allocator(), sub_rg);
            }
        }
)

tst_CREATE_TEST_CASE(range_get_subrange_middle, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 3,
        .end = 6,
        .expect_success = true,
        .r_expected = range_static_create(10, u32, 3, 4, 5),
)
tst_CREATE_TEST_CASE(range_get_subrange_whole, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 0,
        .end = 10,
        .expect_success = true,
        .r_expected = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
)
tst_CREATE_TEST_CASE(range_get_one_element_middle, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 4,
        .end = 5,
        .expect_success = true,
        .r_expected = range_static_create(10, u32, 4),
)
tst_CREATE_TEST_CASE(range_get_one_element_beggining, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 0,
        .end = 1,
        .expect_success = true,
        .r_expected = range_static_create(10, u32, 0),
)
tst_CREATE_TEST_CASE(range_get_one_element_end, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 9,
        .end = 10,
        .expect_success = true,
        .r_expected = range_static_create(10, u32, 9),
)
tst_CREATE_TEST_CASE(range_get_bad_subrange_indexes, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 5,
        .end = 3,
        .expect_success = false,
        .r_expected = range_static_create(10, u32),
)
tst_CREATE_TEST_CASE(range_get_bad_subrange_indexes_2, range_get_subrange,
        .r_source = range_static_create(10, u32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .begin = 2,
        .end = 11,
        .expect_success = false,
        .r_expected = range_static_create(10, u32),
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

    tst_run_test_case(range_insert_other_simple);
    tst_run_test_case(range_insert_other_in_empty);
    tst_run_test_case(range_insert_other_too_large);
    tst_run_test_case(range_insert_other_empty);
    tst_run_test_case(range_insert_other_start);
    tst_run_test_case(range_insert_other_middle);

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

    tst_run_test_case(range_concat_full);
    tst_run_test_case(range_concat_notfull);
    tst_run_test_case(range_concat_one_empty);
    tst_run_test_case(range_concat_two_empty);

    tst_run_test_case(range_get_subrange_middle);
    tst_run_test_case(range_get_subrange_whole);
    tst_run_test_case(range_get_one_element_middle);
    tst_run_test_case(range_get_one_element_beggining);
    tst_run_test_case(range_get_one_element_end);

    tst_run_test_case(range_get_bad_subrange_indexes);
    tst_run_test_case(range_get_bad_subrange_indexes_2);
}

#endif