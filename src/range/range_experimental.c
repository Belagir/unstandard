
#include <ustd/experimental/range.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

typedef struct range_anonymous {
    size_t length;
    size_t capacity;
    byte data[];
} range_anonymous;

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
    if (target.r->length + 1 > target.r->capacity) {
        return false;
    }

    index = min(index, target.r->length);

    for (size_t i = target.r->length ; i > index ; i--) {
        bytewise_copy(target.r->data + (i * target.stride), target.r->data + (i - 1) * target.stride, target.stride);
    }

    rrange_set(target, index, value);
    target.r->length += 1;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool rrange_insert_range(rrange_any target, size_t index, const rrange_any other)
{
    if (((target.r->length + other.r->length) > target.r->capacity) || (target.stride != other.stride)) {
        return false;
    }

    index = min(index, target.r->length);

    for (size_t i = target.r->length ; i > index ; i--) {
        bytewise_copy(target.r->data + ((i + (other.r->length - 1)) * target.stride), target.r->data + (i - 1) * target.stride, target.stride);
    }

    for (size_t i = 0 ; i < other.r->length ; i++) {
        rrange_set(target, index + i, other.r->data + (i * other.stride));
    }
    target.r->length += other.r->length;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool rrange_remove(rrange_any target, size_t index)
{
    return rrange_remove_interval(target, index, index + 1);
}

// -------------------------------------------------------------------------------------------------
bool rrange_remove_interval(rrange_any target, size_t from, size_t to)
{
    size_t nb_shifted_elements = { 0 };
    size_t nb_removed_elements = { 0 };

    if ((from >= to) || (to > target.r->length)) {
        return false;
    }

    nb_shifted_elements = target.r->length - to;
    nb_removed_elements = to - from;

    for (size_t i = 0 ; i <= nb_shifted_elements ; i++) {
        bytewise_copy(
                target.r->data + (from + i) * target.stride,
                target.r->data + (from + i + nb_removed_elements) * target.stride,
                target.stride);
    }
    target.r->length -= (to - from);

    return true;
}

// -------------------------------------------------------------------------------------------------
void rrange_clear(rrange_any target)
{
    target.r->length = 0u;
}

// -------------------------------------------------------------------------------------------------
size_t rrange_index_of(const rrange_any haystack, rrange_comparator comparator, const void *needle, size_t from)
{
    bool found = { false };
    size_t pos = { min(from, haystack.r->length) };

    while ((pos < haystack.r->length) && !found) {
        found = (comparator(haystack.r->data + (pos * haystack.stride), needle) == 0);
        pos += !found;
    }

    return pos;
}

// -------------------------------------------------------------------------------------------------
void *rrange_create_dynamic(allocator alloc, size_t size_element, size_t nb_elements_max)
{
    range_anonymous *new_range = { };

    if ((size_element == 0u) || (nb_elements_max == 0u)) {
        return NULL;
    }

    new_range = alloc.malloc(alloc, sizeof(*new_range) +  size_element * nb_elements_max);
    if (!new_range) {
        return NULL;
    }

    *new_range = (range_anonymous) { .capacity = nb_elements_max, .length = 0u };

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void rrange_destroy_dynamic(allocator alloc, rrange_any *target)
{
    if (!target || !target->r) {
        return;
    }

    alloc.free(alloc, target->r);
    target->r = NULL;
}

// -------------------------------------------------------------------------------------------------
void *rrange_create_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, const void *array)
{
    range_anonymous *new_range = { };

    if ((array == NULL) || (nb_elements_max < nb_elements)) {
        return NULL;
    }

    new_range = rrange_create_dynamic(alloc, size_element, nb_elements_max);
    if (!new_range) {
        return NULL;
    }

    bytewise_copy(new_range->data, array, nb_elements * size_element);
    new_range->length = nb_elements;

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *rrange_create_dynamic_from_resize_of(allocator alloc, const rrange_any target, size_t new_capacity)
{
    range_anonymous *new_range = { };

    new_range = rrange_create_dynamic(alloc, target.stride, new_capacity);

    if (!new_range) {
        return NULL;
    }

    new_range->length = min(new_capacity, target.r->length);
    bytewise_copy(new_range->data, target.r->data, new_range->length * target.stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *rrange_concat(allocator alloc, const rrange_any r_left, const rrange_any r_right)
{
    range_anonymous *new_range = { };

    if (r_left.stride != r_left.stride) {
        return NULL;
    }

    new_range = rrange_create_dynamic(alloc, r_left.stride, r_left.r->length + r_right.r->length);

    if (!new_range) {
        return NULL;
    }

    new_range->length = r_left.r->length + r_right.r->length;

    bytewise_copy(new_range->data, r_left.r->data, r_left.stride * r_left.r->length);
    bytewise_copy(new_range->data + (r_left.stride * r_left.r->length), r_right.r->data, r_right.stride * r_right.r->length);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *rrange_copy_of(allocator alloc, const rrange_any target)
{
    range_anonymous *new_range = { };
    const size_t sizeof_copy = sizeof(*target.r) + (target.r->capacity * target.stride);

    new_range = rrange_create_dynamic(alloc, target.stride, target.r->capacity);
    if (!new_range) {
        return NULL;
    }

    new_range->length = target.r->length;
    bytewise_copy(new_range->data, target.r->data, target.r->length * target.stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void rrange_set(rrange_any target, size_t index, const void *value)
{
    bytewise_copy(target.r->data + (index * target.stride), value, target.stride);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

static i32 test_compare_u32(const void *el1, const void *el2) {
    u32 val1 = *((u32 *) el1);
    u32 val2 = *((u32 *) el2);

    return (val1 > val2) - (val1 < val2);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_remove_interval,
        {
            rrange(u32, 10) array;
            size_t remove_from;
            size_t remove_to;

            rrange(u32, 10) expected_array;
            bool expect_removal;
        },
        {
            bool success = rrange_remove_interval(rrange_to_any(&data->array), data->remove_from, data->remove_to);

            tst_assert_equal(data->expect_removal, success, "success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(rrange_remove_interval_whole, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 0,
        .remove_to      = 10,
        .expected_array = rrange_create_static(u32, 10),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_beginning, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 0,
        .remove_to      = 3,
        .expected_array = rrange_create_static(u32, 10, 3, 4, 5, 6, 7, 8, 9),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_end, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 4,
        .remove_to      = 10,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 2, 3),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_middle, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 3,
        .remove_to      = 8,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 2, 8, 9),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_too_far, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 5,
        .remove_to      = 11,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_removal = false,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_one_element, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 2,
        .remove_to      = 3,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 3, 4, 5, 6, 7, 8, 9),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_bad_interval, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 4,
        .remove_to      = 3,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_removal = false,
)
tst_CREATE_TEST_CASE(rrange_remove_interval_empty_interval, rrange_remove_interval,
        .array          = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .remove_from    = 4,
        .remove_to      = 4,
        .expected_array = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .expect_removal = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_search,
        {
            rrange(u32, 10) array;
            u32 searched_for;
            size_t search_from;

            size_t expected_position;
        },
        {
            size_t pos = rrange_index_of(rrange_to_any(&data->array), &test_compare_u32, &data->searched_for, data->search_from);
            tst_assert_equal(data->expected_position, pos, "position of %d");
        }
)
tst_CREATE_TEST_CASE(rrange_search_for_head, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .searched_for       = 0,
        .search_from        = 0,
        .expected_position  = 0,
)
tst_CREATE_TEST_CASE(rrange_search_for_end, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .searched_for       = 9,
        .search_from        = 0,
        .expected_position  = 9,
)
tst_CREATE_TEST_CASE(rrange_search_in_middle, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 5, 6, 7, 1, 2, 3, 4, 8, 9),
        .searched_for       = 3,
        .search_from        = 0,
        .expected_position  = 6,
)
tst_CREATE_TEST_CASE(rrange_search_second_occurence, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 5, 6, 7, 1, 2, 6, 4, 8, 9),
        .searched_for       = 6,
        .search_from        = 3,
        .expected_position  = 6,
)
tst_CREATE_TEST_CASE(rrange_search_first_occurence, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 5, 6, 7, 1, 2, 6, 4, 8, 9),
        .searched_for       = 6,
        .search_from        = 2,
        .expected_position  = 2,
)
tst_CREATE_TEST_CASE(rrange_search_not_found, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 5, 6, 7, 1, 2, 6, 4, 8, 9),
        .searched_for       = 99,
        .search_from        = 0,
        .expected_position  = 10,
)
tst_CREATE_TEST_CASE(rrange_search_not_found_after, rrange_search,
        .array              = rrange_create_static(u32, 10, 0, 5, 6, 7, 1, 2, 5, 4, 8, 9),
        .searched_for       = 6,
        .search_from        = 3,
        .expected_position  = 10,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_create_from,
        {
            u32 source[10];
            size_t source_length;

            rrange(u32, 10) expected_range;
        },
        {
            rrange(u32) *created_range = rrange_create_dynamic_from(make_system_allocator(), sizeof(*data->source), 10, data->source_length, data->source);

            tst_assert_equal(data->expected_range.length, created_range->length, "length of %d");
            for (size_t i = 0 ; i < data->expected_range.length ; i++) {
                tst_assert_equal_ext(data->expected_range.data[i], created_range->data[i], "value of %d", "at index %d", i);
            }

            if (created_range) {
                rrange_destroy_dynamic(make_system_allocator(), &rrange_to_any(created_range));
            }
        }
)
tst_CREATE_TEST_CASE(rrange_create_from_full, rrange_create_from,
        .source = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, },
        .source_length = 10,
        .expected_range = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
)
tst_CREATE_TEST_CASE(rrange_create_from_part, rrange_create_from,
        .source = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .source_length = 6,
        .expected_range = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5),
)
tst_CREATE_TEST_CASE(rrange_create_from_empty, rrange_create_from,
        .source = { },
        .source_length = 0,
        .expected_range = rrange_create_static(u32, 10),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_resize,
        {
            rrange(u32, 10) array;
            size_t new_size;

            rrange(u32, 20) expected_array;
            bool expect_success;
        },
        {
            rrange(u32) *new_range = rrange_create_dynamic_from_resize_of(make_system_allocator(), rrange_to_any(&data->array), data->new_size);

            if (data->expect_success) {
                tst_assert(new_range, "range was not created");
                tst_assert_equal(data->new_size, new_range->capacity, "capacity of %d");
                for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                    tst_assert_equal_ext(data->expected_array.data[i], new_range->data[i], "value of %d", "at index %d", i);
                }
            } else {
                tst_assert(!new_range, "range was still created");
            }

            rrange_destroy_dynamic(make_system_allocator(), &rrange_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(rrange_resize_to_smaller, rrange_resize,
        .array          = rrange_create_static(u32, 10, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .new_size       = 6,
        .expected_array = rrange_create_static(u32, 20, 0, 2, 4, 6, 8, 10),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(rrange_resize_to_bigger, rrange_resize,
        .array          = rrange_create_static(u32, 10, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .new_size       = 15,
        .expected_array = rrange_create_static(u32, 20, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(rrange_resize_to_same_size, rrange_resize,
        .array          = rrange_create_static(u32, 10, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .new_size       = 10,
        .expected_array = rrange_create_static(u32, 20, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(rrange_resize_to_nothing, rrange_resize,
        .array          = rrange_create_static(u32, 10, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18),
        .new_size       = 0,
        .expected_array = rrange_create_static(u32, 20),
        .expect_success = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_concat,
        {
            rrange(u32, 10) range_left;
            rrange(u32, 10) range_right;

            rrange(u32, 20) expected_range;
        },
        {
            rrange(u32) *new_range = rrange_concat(make_system_allocator(), rrange_to_any(&data->range_left), rrange_to_any(&data->range_right));

            tst_assert_equal(data->expected_range.length, new_range->length, "length of %d");
            for (size_t i = 0 ; i < data->expected_range.length ; i++) {
                tst_assert_equal_ext(data->expected_range.data[i], new_range->data[i], "value of %d", "at index %d", i);
            }

            rrange_destroy_dynamic(make_system_allocator(), &rrange_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(rrange_concat_two_populated, rrange_concat,
        .range_left     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .range_right    = rrange_create_static(u32, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
        .expected_range = rrange_create_static(u32, 20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
)
tst_CREATE_TEST_CASE(rrange_concat_left_empty, rrange_concat,
        .range_left     = rrange_create_static(u32, 10),
        .range_right    = rrange_create_static(u32, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
        .expected_range = rrange_create_static(u32, 20, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19),
)
tst_CREATE_TEST_CASE(rrange_concat_right_empty, rrange_concat,
        .range_left     = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
        .range_right    = rrange_create_static(u32, 10),
        .expected_range = rrange_create_static(u32, 20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(rrange_copy,
        {
            rrange(u32, 10) array;
        },
        {
            rrange(u32) *new_range = rrange_copy_of(make_system_allocator(), rrange_to_any(&data->array));

            tst_assert_equal(data->array.length, new_range->length, "length of %d");

            for (size_t i = 0 ; i < data->array.length ; i++) {
                tst_assert_equal_ext(data->array.data[i], new_range->data[i], "value of %d", "at index %d", i);
            }

            rrange_destroy_dynamic(make_system_allocator(), &rrange_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(rrange_copy_full, rrange_copy,
        .array = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
)
tst_CREATE_TEST_CASE(rrange_copy_not_full, rrange_copy,
        .array = rrange_create_static(u32, 10, 0, 1, 2, 3, 4, 5)
)
tst_CREATE_TEST_CASE(rrange_copy_empty, rrange_copy,
        .array = rrange_create_static(u32, 10)
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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

    tst_run_test_case(rrange_remove_interval_whole);
    tst_run_test_case(rrange_remove_interval_beginning);
    tst_run_test_case(rrange_remove_interval_end);
    tst_run_test_case(rrange_remove_interval_middle);
    tst_run_test_case(rrange_remove_interval_too_far);
    tst_run_test_case(rrange_remove_interval_one_element);
    tst_run_test_case(rrange_remove_interval_bad_interval);
    tst_run_test_case(rrange_remove_interval_empty_interval);

    tst_run_test_case(rrange_search_for_head);
    tst_run_test_case(rrange_search_for_end);
    tst_run_test_case(rrange_search_in_middle);
    tst_run_test_case(rrange_search_second_occurence);
    tst_run_test_case(rrange_search_first_occurence);
    tst_run_test_case(rrange_search_not_found);
    tst_run_test_case(rrange_search_not_found_after);

    tst_run_test_case(rrange_create_from_full);
    tst_run_test_case(rrange_create_from_part);
    tst_run_test_case(rrange_create_from_empty);

    tst_run_test_case(rrange_resize_to_smaller);
    tst_run_test_case(rrange_resize_to_bigger);
    tst_run_test_case(rrange_resize_to_same_size);
    tst_run_test_case(rrange_resize_to_nothing);

    tst_run_test_case(rrange_concat_two_populated);
    tst_run_test_case(rrange_concat_left_empty);
    tst_run_test_case(rrange_concat_right_empty);

    tst_run_test_case(rrange_copy_full);
    tst_run_test_case(rrange_copy_not_full);
    tst_run_test_case(rrange_copy_empty);
}

#endif
