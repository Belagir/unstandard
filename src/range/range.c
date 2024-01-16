
#include <ustd/range.h>
#include <ustd_impl/range_impl.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool range_insert_value(range_any target, size_t index, const void *value)
{
    if (target.r->length + 1 > target.r->capacity) {
        return false;
    }

    index = min(index, target.r->length);

    for (size_t i = target.r->length ; i > index ; i--) {
        bytewise_copy(target.r->data + (i * target.stride), target.r->data + (i - 1) * target.stride, target.stride);
    }

    range_set(target, index, value);
    target.r->length += 1;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool range_insert_range(range_any target, size_t index, const range_any other)
{
    if (!target.r || !other.r || ((target.r->length + other.r->length) > target.r->capacity) || (target.stride != other.stride)) {
        return false;
    }

    index = min(index, target.r->length);

    for (size_t i = target.r->length ; i > index ; i--) {
        bytewise_copy(target.r->data + ((i + (other.r->length - 1)) * target.stride), target.r->data + (i - 1) * target.stride, target.stride);
    }

    for (size_t i = 0 ; i < other.r->length ; i++) {
        range_set(target, index + i, other.r->data + (i * other.stride));
    }
    target.r->length += other.r->length;

    return true;
}

// -------------------------------------------------------------------------------------------------
bool range_remove(range_any target, size_t index)
{
    return range_remove_interval(target, index, index + 1);
}

// -------------------------------------------------------------------------------------------------
bool range_remove_interval(range_any target, size_t from, size_t to)
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
void range_clear(range_any target)
{
    target.r->length = 0u;
}

// -------------------------------------------------------------------------------------------------
size_t range_index_of(const range_any haystack, comparator_f comparator, const void *needle, size_t from)
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
size_t range_count(const range_any haystack, comparator_f comparator, const void *needle, size_t from)
{
    size_t nb_occurences = { 0u };

    for (size_t i = from ; i < haystack.r->length ; i++) {
        nb_occurences += (comparator(range_at(haystack, i), needle) == 0);
    }

    return nb_occurences;
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic(allocator alloc, size_t size_element, size_t nb_elements_max)
{
    range_anonymous *new_range = { };
    range_anonymous model_range = { .capacity = nb_elements_max, .length = 0u };

    if (size_element == 0u) {
        return NULL;
    }

    new_range = alloc.malloc(alloc, sizeof(*new_range) +  size_element * nb_elements_max);
    if (!new_range) {
        return NULL;
    }

    bytewise_copy(new_range, &model_range, sizeof(model_range));

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void range_destroy_dynamic(allocator alloc, range_any *target)
{
    if (!target || !target->r) {
        return;
    }

    alloc.free(alloc, target->r);
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, const void *array)
{
    range_anonymous *new_range = { };

    if ((array == NULL) || (nb_elements_max < nb_elements)) {
        return NULL;
    }

    new_range = range_create_dynamic(alloc, size_element, nb_elements_max);
    if (!new_range) {
        return NULL;
    }

    bytewise_copy(new_range->data, array, nb_elements * size_element);
    new_range->length = nb_elements;

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic_from_resize_of(allocator alloc, const range_any target, size_t new_capacity)
{
    range_anonymous *new_range = { };

    new_range = range_create_dynamic(alloc, target.stride, new_capacity);

    if (!new_range) {
        return NULL;
    }

    new_range->length = min(new_capacity, target.r->length);
    bytewise_copy(new_range->data, target.r->data, new_range->length * target.stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic_from_concat(allocator alloc, const range_any r_left, const range_any r_right)
{
    range_anonymous *new_range = { };

    if (r_left.stride != r_left.stride) {
        return NULL;
    }

    new_range = range_create_dynamic(alloc, r_left.stride, r_left.r->length + r_right.r->length);

    if (!new_range) {
        return NULL;
    }

    new_range->length = r_left.r->length + r_right.r->length;

    bytewise_copy(new_range->data, r_left.r->data, r_left.stride * r_left.r->length);
    bytewise_copy(new_range->data + (r_left.stride * r_left.r->length), r_right.r->data, r_right.stride * r_right.r->length);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic_from_copy_of(allocator alloc, const range_any target)
{
    range_anonymous *new_range = { };
    const size_t sizeof_copy = sizeof(*target.r) + (target.r->capacity * target.stride);

    new_range = range_create_dynamic(alloc, target.stride, target.r->capacity);
    if (!new_range) {
        return NULL;
    }

    new_range->length = target.r->length;
    bytewise_copy(new_range->data, target.r->data, target.r->length * target.stride);

    return new_range;
}

// -------------------------------------------------------------------------------------------------
void *range_create_dynamic_from_subrange_of(allocator alloc, const range_any target, size_t start_index, size_t end_index)
{
    range_anonymous *new_range = { };
    size_t nb_copied_elements = { };

    if ((start_index >= end_index) || (end_index > target.r->length)) {
        return NULL;
    }

    nb_copied_elements = end_index - start_index;
    new_range = range_create_dynamic_from(alloc, target.stride, nb_copied_elements, nb_copied_elements, target.r->data + (start_index * target.stride));

    return new_range;
}


// -------------------------------------------------------------------------------------------------
i32 range_compare(const range_any *range_lhs, const range_any *range_rhs, comparator_f comp_f)
{
    size_t pos = { 0u };
    i32 element_comp = { 0 };

    // finding the differing point in the arrays
    while ((pos < range_lhs->r->length) && (pos < range_rhs->r->length) && (element_comp == 0)) {
        element_comp = comp_f(range_at(*range_lhs, pos), range_at(*range_rhs, pos));
        pos += (element_comp == 0);
    }

    // if there is a differing point compare character at this location
    if (element_comp != 0) {
        return element_comp;
    }

    // no differeing point but maybe one of the ranges is longer than the other
    return (range_lhs->r->length > range_rhs->r->length) - (range_lhs->r->length < range_rhs->r->length);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
void range_set(range_any target, size_t index, const void *value)
{
    bytewise_copy(target.r->data + (index * target.stride), value, target.stride);
}

// -------------------------------------------------------------------------------------------------
range_any range_create_dynamic_as_any(allocator alloc, size_t element_size, size_t capacity)
{
    return (range_any) { .r = range_create_dynamic(alloc, element_size, capacity), .stride = element_size };
}

// -------------------------------------------------------------------------------------------------
void *range_at(range_any target, size_t index)
{
    if (!target.r || (index >= target.r->length)) {
        return NULL;
    }

    return target.r->data + (index * target.stride);
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
tst_CREATE_TEST_SCENARIO(range_insert_value,
        {
            range(u32, 10u) array;
            u32 inserted_value;
            u32 insertion_pos;

            range(u32, 10u) expected_array;
            bool expect_insertion;
        },
        {
            bool success = range_insert_value(range_to_any(&data->array), data->insertion_pos, &data->inserted_value);
            tst_assert_equal(success, data->expect_insertion, "insertion success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(range_insert_in_empty, range_insert_value,
        .array              = range_create_static(u32, 10, { }),
        .inserted_value     = 99,
        .insertion_pos      = 5,
        .expected_array     = range_create_static(u32, 10, { 99 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_value     = 99,
        .insertion_pos      = 6,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 99 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_in_populated_end_far, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_value     = 99,
        .insertion_pos      = 9,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 99 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_in_populated_start, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_value     = 99,
        .insertion_pos      = 0,
        .expected_array     = range_create_static(u32, 10, { 99, 0, 1, 2, 3, 4, 5 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_in_populated_middle, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_value     = 99,
        .insertion_pos      = 3,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 99, 3, 4, 5 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_in_full, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .inserted_value     = 99,
        .insertion_pos      = 8,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(range_insert_in_full_far, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .inserted_value     = 99,
        .insertion_pos      = 15,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(range_insert_in_full_start, range_insert_value,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .inserted_value     = 99,
        .insertion_pos      = 0,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_insertion   = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_insert_other_range,
        {
            range(u32, 10u) array;
            range(u32, 10u) inserted_range;
            u32 insertion_pos;

            range(u32, 10u) expected_array;
            bool expect_insertion;
        },
        {
            bool success = range_insert_range(range_to_any(&data->array), data->insertion_pos, range_to_any(&data->inserted_range));
            tst_assert_equal(data->expect_insertion, success, "insertion success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(range_insert_other_simple, range_insert_other_range,
        .array              = range_create_static(u32, 10, { 0, 1, 2 }),
        .inserted_range     = range_create_static(u32, 10, { 3, 4, 5 }),
        .insertion_pos      = 3,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_other_in_empty, range_insert_other_range,
        .array              = range_create_static(u32, 10, { }),
        .inserted_range     = range_create_static(u32, 10, { 3, 4, 5 }),
        .insertion_pos      = 3,
        .expected_array     = range_create_static(u32, 10, { 3, 4, 5 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_other_too_large, range_insert_other_range,
        .array              = range_create_static(u32, 10, { 1 }),
        .inserted_range     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .insertion_pos      = 0,
        .expected_array     = range_create_static(u32, 10, { 1 }),
        .expect_insertion   = false,
)
tst_CREATE_TEST_CASE(range_insert_other_empty, range_insert_other_range,
        .array              = range_create_static(u32, 10, { 1, 2, 3 }),
        .inserted_range     = range_create_static(u32, 10, { }),
        .insertion_pos      = 0,
        .expected_array     = range_create_static(u32, 10, { 1, 2, 3 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_other_at_start, range_insert_other_range,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_range     = range_create_static(u32, 10, { 6, 7, 8, 9 }),
        .insertion_pos      = 0,
        .expected_array     = range_create_static(u32, 10, { 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 }),
        .expect_insertion   = true,
)
tst_CREATE_TEST_CASE(range_insert_other_at_middle, range_insert_other_range,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .inserted_range     = range_create_static(u32, 10, { 6, 7, 8, 9 }),
        .insertion_pos      = 3,
        .expected_array     = range_create_static(u32, 10, { 0, 1, 2, 6, 7, 8, 9, 3, 4, 5 }),
        .expect_insertion   = true,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_remove_interval,
        {
            range(u32, 10) array;
            size_t remove_from;
            size_t remove_to;

            range(u32, 10) expected_array;
            bool expect_removal;
        },
        {
            bool success = range_remove_interval(range_to_any(&data->array), data->remove_from, data->remove_to);

            tst_assert_equal(data->expect_removal, success, "success of %d");

            tst_assert_equal(data->expected_array.length, data->array.length, "length of %d");
            for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "%d", "at index %d", i);
            }
        }
)
tst_CREATE_TEST_CASE(range_remove_interval_whole, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 0,
        .remove_to      = 10,
        .expected_array = range_create_static(u32, 10, { }),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(range_remove_interval_beginning, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 0,
        .remove_to      = 3,
        .expected_array = range_create_static(u32, 10, { 3, 4, 5, 6, 7, 8, 9 }),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(range_remove_interval_end, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 4,
        .remove_to      = 10,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 3 }),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(range_remove_interval_middle, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 3,
        .remove_to      = 8,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 8, 9 }),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(range_remove_interval_too_far, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 5,
        .remove_to      = 11,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_removal = false,
)
tst_CREATE_TEST_CASE(range_remove_interval_one_element, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 2,
        .remove_to      = 3,
        .expected_array = range_create_static(u32, 10, { 0, 1, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_removal = true,
)
tst_CREATE_TEST_CASE(range_remove_interval_bad_interval, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 4,
        .remove_to      = 3,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_removal = false,
)
tst_CREATE_TEST_CASE(range_remove_interval_empty_interval, range_remove_interval,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .remove_from    = 4,
        .remove_to      = 4,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_removal = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_search,
        {
            range(u32, 10) array;
            u32 searched_for;
            size_t search_from;

            size_t expected_position;
        },
        {
            size_t pos = range_index_of(range_to_any(&data->array), &test_compare_u32, &data->searched_for, data->search_from);
            tst_assert_equal(data->expected_position, pos, "position of %d");
        }
)
tst_CREATE_TEST_CASE(range_search_for_head, range_search,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .searched_for       = 0,
        .search_from        = 0,
        .expected_position  = 0,
)
tst_CREATE_TEST_CASE(range_search_for_end, range_search,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .searched_for       = 9,
        .search_from        = 0,
        .expected_position  = 9,
)
tst_CREATE_TEST_CASE(range_search_in_middle, range_search,
        .array              = range_create_static(u32, 10, { 0, 5, 6, 7, 1, 2, 3, 4, 8, 9 }),
        .searched_for       = 3,
        .search_from        = 0,
        .expected_position  = 6,
)
tst_CREATE_TEST_CASE(range_search_second_occurence, range_search,
        .array              = range_create_static(u32, 10, { 0, 5, 6, 7, 1, 2, 6, 4, 8, 9 }),
        .searched_for       = 6,
        .search_from        = 3,
        .expected_position  = 6,
)
tst_CREATE_TEST_CASE(range_search_first_occurence, range_search,
        .array              = range_create_static(u32, 10, { 0, 5, 6, 7, 1, 2, 6, 4, 8, 9 }),
        .searched_for       = 6,
        .search_from        = 2,
        .expected_position  = 2,
)
tst_CREATE_TEST_CASE(range_search_not_found, range_search,
        .array              = range_create_static(u32, 10, { 0, 5, 6, 7, 1, 2, 6, 4, 8, 9 }),
        .searched_for       = 99,
        .search_from        = 0,
        .expected_position  = 10,
)
tst_CREATE_TEST_CASE(range_search_not_found_after, range_search,
        .array              = range_create_static(u32, 10, { 0, 5, 6, 7, 1, 2, 5, 4, 8, 9 }),
        .searched_for       = 6,
        .search_from        = 3,
        .expected_position  = 10,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_count,
        {
            range(u32, 10) array;
            u32 searched_for;
            size_t search_from;

            size_t expected_count;
        },
        {
            size_t count = range_count(range_to_any(&data->array), &test_compare_u32, &data->searched_for, data->search_from);
            tst_assert_equal(data->expected_count, count, "count of %d");
        }
)
tst_CREATE_TEST_CASE(range_count_once, range_count,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .searched_for       = 3,
        .search_from        = 0,
        .expected_count     = 1,
)
tst_CREATE_TEST_CASE(range_count_nothing, range_count,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .searched_for       = 99,
        .search_from        = 0,
        .expected_count     = 0,
)
tst_CREATE_TEST_CASE(range_count_multiple, range_count,
        .array              = range_create_static(u32, 10, { 3, 1, 2, 3, 4, 5, 6, 3, 8, 3 }),
        .searched_for       = 3,
        .search_from        = 0,
        .expected_count     = 4,
)
tst_CREATE_TEST_CASE(range_count_contiguous, range_count,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 3, 3, 6, 7, 8, 9 }),
        .searched_for       = 3,
        .search_from        = 0,
        .expected_count     = 3,
)
tst_CREATE_TEST_CASE(range_count_after_occurence, range_count,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .searched_for       = 3,
        .search_from        = 4,
        .expected_count     = 0,
)
tst_CREATE_TEST_CASE(range_count_after_occurence_multiple, range_count,
        .array              = range_create_static(u32, 10, { 0, 1, 2, 3, 3, 3, 3, 7, 8, 9 }),
        .searched_for       = 3,
        .search_from        = 4,
        .expected_count     = 3,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_create_from,
        {
            u32 source[10];
            size_t source_length;

            range(u32, 10) expected_range;
        },
        {
            range(u32) *created_range = range_create_dynamic_from(make_system_allocator(), sizeof(*data->source), 10, data->source_length, data->source);

            tst_assert_equal(data->expected_range.length, created_range->length, "length of %d");
            for (size_t i = 0 ; i < data->expected_range.length ; i++) {
                tst_assert_equal_ext(data->expected_range.data[i], created_range->data[i], "value of %d", "at index %d", i);
            }

            if (created_range) {
                range_destroy_dynamic(make_system_allocator(), &range_to_any(created_range));
            }
        }
)
tst_CREATE_TEST_CASE(range_create_from_full, range_create_from,
        .source = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, },
        .source_length = 10,
        .expected_range = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
)
tst_CREATE_TEST_CASE(range_create_from_part, range_create_from,
        .source = { 0, 1, 2, 3, 4, 5, 6, 7 },
        .source_length = 6,
        .expected_range = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
)
tst_CREATE_TEST_CASE(range_create_from_empty, range_create_from,
        .source = { },
        .source_length = 0,
        .expected_range = range_create_static(u32, 10, { }),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_resize,
        {
            range(u32, 10) array;
            size_t new_size;

            range(u32, 20) expected_array;
            bool expect_success;
        },
        {
            range(u32) *new_range = range_create_dynamic_from_resize_of(make_system_allocator(), range_to_any(&data->array), data->new_size);

            if (data->expect_success) {
                tst_assert(new_range, "range was not created");
                tst_assert_equal(data->new_size, new_range->capacity, "capacity of %d");
                for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                    tst_assert_equal_ext(data->expected_array.data[i], new_range->data[i], "value of %d", "at index %d", i);
                }
            } else {
                tst_assert(!new_range, "range was still created");
            }

            range_destroy_dynamic(make_system_allocator(), &range_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(range_resize_to_smaller, range_resize,
        .array          = range_create_static(u32, 10, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .new_size       = 6,
        .expected_array = range_create_static(u32, 20, { 0, 2, 4, 6, 8, 10 }),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(range_resize_to_bigger, range_resize,
        .array          = range_create_static(u32, 10, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .new_size       = 15,
        .expected_array = range_create_static(u32, 20, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(range_resize_to_same_size, range_resize,
        .array          = range_create_static(u32, 10, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .new_size       = 10,
        .expected_array = range_create_static(u32, 20, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(range_resize_to_nothing, range_resize,
        .array          = range_create_static(u32, 10, { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 }),
        .new_size       = 0,
        .expected_array = range_create_static(u32, 20 ,{ }),
        .expect_success = true,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_create_dynamic_from_concat,
        {
            range(u32, 10) range_left;
            range(u32, 10) range_right;

            range(u32, 20) expected_range;
        },
        {
            range(u32) *new_range = range_create_dynamic_from_concat(make_system_allocator(), range_to_any(&data->range_left), range_to_any(&data->range_right));

            tst_assert_equal(data->expected_range.length, new_range->length, "length of %d");
            for (size_t i = 0 ; i < data->expected_range.length ; i++) {
                tst_assert_equal_ext(data->expected_range.data[i], new_range->data[i], "value of %d", "at index %d", i);
            }

            range_destroy_dynamic(make_system_allocator(), &range_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(range_concat_two_populated, range_create_dynamic_from_concat,
        .range_left     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .range_right    = range_create_static(u32, 10, { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }),
        .expected_range = range_create_static(u32, 20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }),
)
tst_CREATE_TEST_CASE(range_concat_left_empty, range_create_dynamic_from_concat,
        .range_left     = range_create_static(u32, 10, { }),
        .range_right    = range_create_static(u32, 10, { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }),
        .expected_range = range_create_static(u32, 20, { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }),
)
tst_CREATE_TEST_CASE(range_concat_right_empty, range_create_dynamic_from_concat,
        .range_left     = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .range_right    = range_create_static(u32, 10, { }),
        .expected_range = range_create_static(u32, 20, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_copy,
        {
            range(u32, 10) array;
        },
        {
            range(u32) *new_range = range_create_dynamic_from_copy_of(make_system_allocator(), range_to_any(&data->array));

            tst_assert_equal(data->array.length, new_range->length, "length of %d");

            for (size_t i = 0 ; i < data->array.length ; i++) {
                tst_assert_equal_ext(data->array.data[i], new_range->data[i], "value of %d", "at index %d", i);
            }

            range_destroy_dynamic(make_system_allocator(), &range_to_any(new_range));
        }
)
tst_CREATE_TEST_CASE(range_copy_full, range_copy,
        .array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 })
)
tst_CREATE_TEST_CASE(range_copy_not_full, range_copy,
        .array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 })
)
tst_CREATE_TEST_CASE(range_copy_empty, range_copy,
        .array = range_create_static(u32, 10, { })
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_get_subrange,
        {
            range(u32, 10) array;
            size_t from;
            size_t to;

            range(u32, 10) expected_array;
            bool expect_success;
        },
        {
            range(u32) *subrange = range_create_dynamic_from_subrange_of(make_system_allocator(), range_to_any(&data->array), data->from, data->to);

            if (data->expect_success) {
                tst_assert(subrange != NULL, "subrange was not created");

                tst_assert_equal(data->expected_array.length, subrange->length, "length of %d");
                for (size_t i = 0 ; i < data->expected_array.length ; i++) {
                    tst_assert_equal_ext(data->expected_array.data[i], subrange->data[i], "value of %d", "at index %d", i);
                }
            } else {
                tst_assert(subrange == NULL, "subrange was still created");
            }

            range_destroy_dynamic(make_system_allocator(), &range_to_any(subrange));
        }
)
tst_CREATE_TEST_CASE(range_get_subrange_whole, range_get_subrange,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .from           = 0,
        .to             = 10,
        .expected_array = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(range_get_subrange_part, range_get_subrange,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .from           = 3,
        .to             = 6,
        .expected_array = range_create_static(u32, 10, { 3, 4, 5 }),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(range_get_subrange_empty, range_get_subrange,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .from           = 5,
        .to             = 4,
        .expected_array = range_create_static(u32, 10, { }),
        .expect_success = false,
)
tst_CREATE_TEST_CASE(range_get_subrange_beyond, range_get_subrange,
        .array          = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .from           = 2,
        .to             = 12,
        .expected_array = range_create_static(u32, 10, { }),
        .expect_success = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(range_compare,
        {
            range(u32, 10) array_left;
            range(u32, 10) array_right;
            i32 expected;
        },
        {
            tst_assert_equal(data->expected, range_compare(&range_to_any(&data->array_left), &range_to_any(&data->array_right), &test_compare_u32),
                    "comparison result of %d");
        }
)
tst_CREATE_TEST_CASE(range_compare_equal, range_compare,
        .array_left  = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .array_right = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expected    = 0,
)
tst_CREATE_TEST_CASE(range_compare_more_than, range_compare,
        .array_left  = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 10 }),
        .array_right = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expected    = 1,
)
tst_CREATE_TEST_CASE(range_compare_more_than_with_length, range_compare,
        .array_left  = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 10 }),
        .array_right = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5 }),
        .expected    = 1,
)
tst_CREATE_TEST_CASE(range_compare_more_than_before_length, range_compare,
        .array_left  = range_create_static(u32, 10, { 0, 1, 2, 99, 4, 5, 6, 7, 8, 10 }),
        .array_right = range_create_static(u32, 10, { 0, 1, 2, 3, 4 }),
        .expected    = 1,
)
tst_CREATE_TEST_CASE(range_compare_less_than, range_compare,
        .array_left  = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 8 }),
        .array_right = range_create_static(u32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }),
        .expected    = -1,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void range_experimental_execute_unittests(void)
{
    tst_run_test_case(range_insert_in_empty);
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
    tst_run_test_case(range_insert_other_at_start);
    tst_run_test_case(range_insert_other_at_middle);

    tst_run_test_case(range_remove_interval_whole);
    tst_run_test_case(range_remove_interval_beginning);
    tst_run_test_case(range_remove_interval_end);
    tst_run_test_case(range_remove_interval_middle);
    tst_run_test_case(range_remove_interval_too_far);
    tst_run_test_case(range_remove_interval_one_element);
    tst_run_test_case(range_remove_interval_bad_interval);
    tst_run_test_case(range_remove_interval_empty_interval);

    tst_run_test_case(range_search_for_head);
    tst_run_test_case(range_search_for_end);
    tst_run_test_case(range_search_in_middle);
    tst_run_test_case(range_search_second_occurence);
    tst_run_test_case(range_search_first_occurence);
    tst_run_test_case(range_search_not_found);
    tst_run_test_case(range_search_not_found_after);

    tst_run_test_case(range_count_once);
    tst_run_test_case(range_count_nothing);
    tst_run_test_case(range_count_multiple);
    tst_run_test_case(range_count_contiguous);
    tst_run_test_case(range_count_after_occurence);
    tst_run_test_case(range_count_after_occurence_multiple);

    tst_run_test_case(range_create_from_full);
    tst_run_test_case(range_create_from_part);
    tst_run_test_case(range_create_from_empty);

    tst_run_test_case(range_resize_to_smaller);
    tst_run_test_case(range_resize_to_bigger);
    tst_run_test_case(range_resize_to_same_size);
    tst_run_test_case(range_resize_to_nothing);

    tst_run_test_case(range_concat_two_populated);
    tst_run_test_case(range_concat_left_empty);
    tst_run_test_case(range_concat_right_empty);

    tst_run_test_case(range_copy_full);
    tst_run_test_case(range_copy_not_full);
    tst_run_test_case(range_copy_empty);

    tst_run_test_case(range_get_subrange_whole);
    tst_run_test_case(range_get_subrange_part);
    tst_run_test_case(range_get_subrange_empty);
    tst_run_test_case(range_get_subrange_beyond);

    tst_run_test_case(range_compare_equal);
    tst_run_test_case(range_compare_more_than);
    tst_run_test_case(range_compare_more_than_with_length);
    tst_run_test_case(range_compare_more_than_before_length);
    tst_run_test_case(range_compare_less_than);
}

#endif
