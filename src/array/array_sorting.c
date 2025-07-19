
#include <ustd_impl/array_impl.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define PARENT(index) (((index) >> (1u))-(1u))
#define LEFT(index)   (((index) << (1u))+(1u))
#define RIGHT(index)  ((((index) << (1u)) | 0x1)+(1u))

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void swap_pointed(u8 pos1[static 1], u8 pos2[static 1], size_t datasize);

static void heapify(void *array, size_t length_heap, size_t index, comparator_f comparator);

static void build_heap(void *array, comparator_f comparator);

// -------------------------------------------------------------------------------------------------

void array_sort(void *array, comparator_f comparator)
{
    struct array_impl *target = array_impl_of(array);

    if (target->length == 0u) {
        return;
    }

    build_heap(array, comparator);

    size_t length_heap = target->length;

    for (size_t i = (target->length - 1u); i >= 1u; i--) {
        swap_pointed(target->data, (void *) ((uintptr_t) target->data + (uintptr_t) (i * target->stride)), target->stride);
        length_heap = length_heap - 1u;
        heapify(array, length_heap, 0u, comparator);
    }
}

// -------------------------------------------------------------------------------------------------

bool array_is_sorted(void *array, comparator_f comparator)
{
    size_t pos = { 0u };
    struct array_impl *target = array_impl_of(array);

    if (target->length == 0u) {
        return true;
    }

    pos = 1u;
    while ((pos < target->length) && (comparator((void *) (target->data + (pos * target->stride)), (void *) (target->data + ((pos - 1) * target->stride))) != -1)) {
        pos += 1u;
    }

    return (pos == target->length);
}

// -------------------------------------------------------------------------------------------------

bool array_sorted_find(void *haystack, comparator_f comparator, void *needle, size_t *out_position)
{
    i32 beginning = 0u;
    i32 end = 0u;
    i32 index = 0u;
    i32 comp_result = 2;

    struct array_impl *target = array_impl_of(haystack);

    beginning = 0u;
    end = (i32) target->length - 1;

    while ((beginning <= end) && (comp_result != 0)) {
        index = (i32) CEIL_DIV(beginning + end, 2);

        comp_result = comparator(needle, (void *) (target->data + ((size_t) index * target->stride)));

        if (comp_result == 1) {
            beginning = index + 1;
        }
        else if (comp_result == -1) {
            end = index - 1;
        }
    }

    while ((index > 0) && (comparator(needle, (void *) (target->data + ((size_t) (index - 1) * target->stride))) == 0)) {
        index -= 1;
        comp_result = comparator(needle, (void *) (target->data + ((size_t) index * target->stride)));
    }

    if (out_position != NULL) {
        if ((comp_result == 0) || (comp_result == -1) || (target->length == 0u)) {
            *out_position = (size_t) index;
        }
        else {
            *out_position = (size_t) (index + comp_result);
        }
    }

    return (comp_result == 0);
}

// -------------------------------------------------------------------------------------------------

size_t array_sorted_remove(void *haystack, comparator_f comparator, void *needle)
{
    u32 found = 0u;
    size_t found_pos = 0u;

    struct array_impl *target = array_impl_of(haystack);

    if (!comparator || !needle) {
        return target->length;
    }

    found = array_sorted_find(haystack, comparator, needle, &found_pos);

    if (!found) {
        return target->length;
    }

    array_remove(haystack, found_pos);

    return found_pos;
}

// -------------------------------------------------------------------------------------------------

size_t array_sorted_insert(void *haystack, comparator_f comparator, void *inserted_needle)
{
    size_t theorical_position = 0u;

    struct array_impl *target = array_impl_of(haystack);

    if (!comparator || !inserted_needle)
    {
        return target->length;
    }

    (void) array_sorted_find(haystack, comparator, inserted_needle, &theorical_position);
    array_insert_value(haystack, theorical_position, inserted_needle);

    return theorical_position;
}


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

static void swap_pointed(u8 pos1[static 1], u8 pos2[static 1], size_t datasize)
{
   u8 tmp = 0;

    for (size_t i = 0 ; i < datasize ; i++) {
        tmp = pos1[i];
        pos1[i] = pos2[i];
        pos2[i] = tmp;
    }
}

// -------------------------------------------------------------------------------------------------

static void heapify(void *array, size_t length_heap, size_t index, comparator_f comparator)
{
    size_t imax;
    size_t left;
    size_t right;

    i32 heaped = 0u;

    struct array_impl *target = array_impl_of(array);

    while (!heaped) {
        left = LEFT(index);
        right = RIGHT(index);

        imax = index;

        if ((right < length_heap) && (comparator((void *) (target->data + (right * target->stride)), (void *) (target->data + (imax * target->stride))) == 1)) {
            imax = right;
        }

        if ((left < length_heap)  && (comparator((void *) (target->data + (left * target->stride)), (void *) (target->data + (imax * target->stride))) == 1)) {
            imax = left;
        }

        heaped = (imax == index);

        if (!heaped) {
            swap_pointed((void *) ((uintptr_t) target->data + (uintptr_t) (imax * target->stride)), (void *) ((uintptr_t) target->data + (uintptr_t) (index * target->stride)), target->stride);
            index = imax;
        }
    }
}

// -------------------------------------------------------------------------------------------------

static void build_heap(void *array, comparator_f comparator)
{
    const size_t start_at = (size_t) ceil((f64) array_length(array) / 2.0F);

    for (size_t i = start_at ; i > 0u ; i--) {
        heapify(array, array_length(array), i, comparator);
    }
    // last iteration, working with unsigned has its toll
    if (array_length(array) > 0u) {
        heapify(array, array_length(array), 0u, comparator);
    }
}


#if 1
// #ifdef UNITTESTING

#include <ustd/testutilities.h>

static i32 test_i32_comparator(const void *v1, const void *v2) {
    i32 val1 = *((i32 *) v1);
    i32 val2 = *((i32 *) v2);

    return (val1 > val2) - (val1 < val2);
}

tst_CREATE_TEST_SCENARIO(array_heap_sort,
        {
            struct { size_t length; size_t capacity; u32 stride; i32 data[10]; } input;
            struct { size_t length; size_t capacity; u32 stride; i32 data[10]; } expected;
        },
        {
            i32 *input = data->input.data;

            array_sort(input, &test_i32_comparator);

            tst_assert_equal(data->expected.length, data->input.length, "length of %d");
            tst_assert_equal(data->expected.capacity, data->input.capacity, "capacity of %d");
            tst_assert_equal(data->expected.stride, data->input.stride, "stride of %d");

            for (size_t i = 0 ; i < data->expected.length ; i++) {
                tst_assert_equal_ext(data->expected.data[i], data->input.data[i], "%d", "at index %d", i);
            }

            tst_assert(array_is_sorted(input, &test_i32_comparator), "array was not sorted.");
        }
)

tst_CREATE_TEST_CASE(array_heap_sort_full, array_heap_sort,
        .input    = { 10, 10, 4, { 3, 6, 4, 0, 9, 1, 8, 2, 5, 7 } },
        .expected = { 10, 10, 4, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } },
)

tst_CREATE_TEST_CASE(array_heap_sort_partial, array_heap_sort,
        .input    = { 5, 10, 4, { -1, 3, 99, -401, 3, } },
        .expected = { 5, 10, 4, { -401, -1, 3, 3, 99, } },
)

tst_CREATE_TEST_CASE(array_heap_sort_empty, array_heap_sort,
        .input    = { 0, 10, 4, { } },
        .expected = { 0, 10, 4, { } },
)


static i32 test_u32_comparator(const void *v1, const void *v2) {
    u32 val1 = *((u32 *) v1);
    u32 val2 = *((u32 *) v2);

    return (val1 > val2) - (val1 < val2);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(array_sorted_u32_find,
        {
            struct { size_t length; size_t capacity; u32 stride; i32 data[20]; } array;
            u32 needle;

            size_t expected_position;
            size_t expect_success;
        },
        {
            size_t theorical_pos = 0u;
            size_t found = array_sorted_find(&data->array.data, &test_u32_comparator, (void *) &data->needle, &theorical_pos);

            tst_assert_equal(data->expected_position, theorical_pos, "position %d");
            tst_assert((data->expect_success && (found)) || (!data->expect_success), "element was %sfound",
                        (data->expect_success)? "not " : "");
        }
)

tst_CREATE_TEST_CASE(array_sorted_u32_find_nominal, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_nominal_2, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle            = 14u,
        .expected_position = 14u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_at_start, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle            = 0u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_at_end, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle            = 19u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 301u,
        .expected_position = 12u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_start, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 8u,
        .expected_position = 0u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_end, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 5000u,
        .expected_position = 19u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_not_found, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 390u,
        .expected_position = 16u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_not_found_2, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 388u,
        .expected_position = 15u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_not_found_before, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 2u,
        .expected_position = 0u,
        .expect_success    = 0u)


tst_CREATE_TEST_CASE(array_sorted_u32_find_other_array_not_found_after, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 5000u } },
        .needle            = 5001u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(array_sorted_u32_not_found, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle            = 89u,
        .expected_position = 20u,
        .expect_success    = 0u)

tst_CREATE_TEST_CASE(array_sorted_find_first_occ_adjacent, array_sorted_u32_find,
        .array             =  { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 13u, 13u, 13u, 17u, 18u, 19u } },
        .needle            = 13u,
        .expected_position = 13u,
        .expect_success    = 1u)

tst_CREATE_TEST_CASE(array_sorted_find_in_empty, array_sorted_u32_find,
        .array             =  { 0, 20, 4, { } },
        .needle            = 42u,
        .expected_position = 0u,
        .expect_success    = 0u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(array_sorted_remove_element,
        {
            struct { size_t length; size_t capacity; u32 stride; i32 data[20]; } array;
            u32 needle;

            struct { size_t length; size_t capacity; u32 stride; i32 data[20]; } expected_array;
            u32 expect_deletion;
        },
        {
            size_t deletion_pos = array_sorted_remove(data->array.data, &test_u32_comparator, (void *) &data->needle);

            tst_assert(((data->expect_deletion && (deletion_pos != 20u)) || (!data->expect_deletion)), "element was %sdeleted",
                        (data->expect_deletion)? "not " : "");

            for (size_t i = 0u ; i < (20u - data->expect_deletion) ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "value of %d", "at index %d", i);
            }
        }
)

tst_CREATE_TEST_CASE(array_sorted_remove_element_nominal, array_sorted_remove_element,
        .array           = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle          = 15u,
        .expected_array  = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 16u, 17u, 18u, 19u, 0u } },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(array_sorted_remove_element_nominal_2, array_sorted_remove_element,
        .array           = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle          = 14u,
        .expected_array  = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 15u, 16u, 17u, 18u, 19u, 0u } },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(array_sorted_remove_element_at_start, array_sorted_remove_element,
        .array           = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle          = 0u,
        .expected_array  = { 20, 20, 4, { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 0u } },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(array_sorted_remove_element_at_end, array_sorted_remove_element,
        .array           = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle          = 19u,
        .expected_array  = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 0u } },
        .expect_deletion = 1u)

tst_CREATE_TEST_CASE(array_sorted_remove_element_not_found, array_sorted_remove_element,
        .array           = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .needle          = 20u,
        .expected_array  = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u } },
        .expect_deletion = 0u)

tst_CREATE_TEST_CASE(array_sorted_remove_element_in_empty, array_sorted_remove_element,
        .array           = { 0, 20, 4, { } },
        .needle          = 20u,
        .expected_array  = { 0, 20, 4, { } },
        .expect_deletion = 0u)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(array_sorted_insert,
        {
            struct { size_t length; size_t capacity; u32 stride; i32 data[20]; } array;
            size_t array_length;
            u32 inserted_element;

            u32 expected_position;
            struct { size_t length; size_t capacity; u32 stride; i32 data[20]; } expected_array;
        },
        {
            data->array.length = data->array_length;
            size_t insertion_pos = array_sorted_insert(&data->array.data, &test_u32_comparator, (void *) &data->inserted_element);

            tst_assert_equal(data->expected_position, insertion_pos, "position %d");

            for (size_t i = 0u ; i < (data->array.length) ; i++) {
                tst_assert_equal_ext(data->expected_array.data[i], data->array.data[i], "value of %d", "at index %d", i);
            }
        }
)

tst_CREATE_TEST_CASE(array_sorted_insert_nominal, array_sorted_insert,
        .array             = { 15, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u } },
        .array_length      = 15u,
        .inserted_element  = 11u,
        .expected_position = 11u,
        .expected_array    = { 16, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u } },
)
tst_CREATE_TEST_CASE(array_sorted_insert_at_beginning, array_sorted_insert,
        .array             = { 15, 20, 4, { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u, 0u } },
        .array_length      = 15u,
        .inserted_element  = 0u,
        .expected_position = 0u,
        .expected_array    = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u } },
)
tst_CREATE_TEST_CASE(array_sorted_insert_second_position, array_sorted_insert,
        .array             = { 16, 20, 4, { 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
        .array_length      = 1u,
        .inserted_element  = 2u,
        .expected_position = 1u,
        .expected_array    = { 20, 20, 4, { 1u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
)
tst_CREATE_TEST_CASE(array_sorted_insert_penultimate, array_sorted_insert,
        .array             = { 4, 20, 4, { 1u, 2u, 3u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
        .array_length      = 4u,
        .inserted_element  = 4u,
        .expected_position = 3u,
        .expected_array    = { 20, 20, 4, { 1u, 2u, 3u, 4u, 5u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
)
tst_CREATE_TEST_CASE(array_sorted_insert_at_end, array_sorted_insert,
        .array             = { 15, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 0u, 0u, 0u, 0u, 0u } },
        .array_length      = 15u,
        .inserted_element  = 15u,
        .expected_position = 15u,
        .expected_array    = { 20, 20, 4, { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 0u, 0u, 0u } },
)
tst_CREATE_TEST_CASE(array_sorted_u32_insert_other, array_sorted_insert,
        .array             = { 19, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 395u, 400u, 4025u, 0u } },
        .array_length      = 19u,
        .inserted_element  = 390u,
        .expected_position = 16u,
        .expected_array    = { 20, 20, 4, { 8u, 12u, 18u, 64u, 65u, 65u, 65u, 132u, 256u, 280u, 290u, 300u, 301u, 302u, 303u, 389u, 390u, 395u, 400u, 4025u } },
)
tst_CREATE_TEST_CASE(array_sorted_u32_insert_in_empty, array_sorted_insert,
        .array             = { 0, 20, 4, { } },
        .array_length      = 0u,
        .inserted_element  = 42u,
        .expected_position = 0u,
        .expected_array    = { 20, 20, 4, { 42u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } },
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


void array_sort_execute_unittests(void)
{
    tst_run_test_case(array_heap_sort_full);
    tst_run_test_case(array_heap_sort_partial);
    tst_run_test_case(array_heap_sort_empty);

    tst_run_test_case(array_sorted_u32_find_nominal);
    tst_run_test_case(array_sorted_u32_find_nominal_2);
    tst_run_test_case(array_sorted_u32_find_at_start);
    tst_run_test_case(array_sorted_u32_find_at_end);
    tst_run_test_case(array_sorted_u32_find_other_array);
    tst_run_test_case(array_sorted_u32_find_other_array_start);
    tst_run_test_case(array_sorted_u32_find_other_array_end);
    tst_run_test_case(array_sorted_u32_find_other_array_not_found);
    tst_run_test_case(array_sorted_u32_find_other_array_not_found_2);
    tst_run_test_case(array_sorted_u32_find_other_array_not_found_before);
    tst_run_test_case(array_sorted_u32_find_other_array_not_found_after);
    tst_run_test_case(array_sorted_u32_not_found);
    tst_run_test_case(array_sorted_find_first_occ_adjacent);
    tst_run_test_case(array_sorted_find_in_empty);

    tst_run_test_case(array_sorted_remove_element_nominal);
    tst_run_test_case(array_sorted_remove_element_nominal_2);
    tst_run_test_case(array_sorted_remove_element_at_start);
    tst_run_test_case(array_sorted_remove_element_at_end);
    tst_run_test_case(array_sorted_remove_element_not_found);
    tst_run_test_case(array_sorted_remove_element_in_empty);

    tst_run_test_case(array_sorted_insert_nominal);
    tst_run_test_case(array_sorted_insert_at_beginning);
    tst_run_test_case(array_sorted_insert_second_position);
    tst_run_test_case(array_sorted_insert_penultimate);
    tst_run_test_case(array_sorted_insert_at_end);
    tst_run_test_case(array_sorted_u32_insert_other);
    tst_run_test_case(array_sorted_u32_insert_in_empty);
}

#endif
