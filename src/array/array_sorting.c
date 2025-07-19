
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

void array_sort_execute_unittests(void)
{
    tst_run_test_case(array_heap_sort_full);
    tst_run_test_case(array_heap_sort_partial);
    tst_run_test_case(array_heap_sort_empty);
}

#endif
