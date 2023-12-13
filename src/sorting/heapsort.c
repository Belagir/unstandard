
#include <ustd/sorting.h>

#include <math.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

#define PARENT(index) (((index) >> (1u))-(1u))
#define LEFT(index)   (((index) << (1u))+(1u))
#define RIGHT(index)  ((((index) << (1u)) | 0x1)+(1u))

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void swap_pointed(u8 pos1[static 1], u8 pos2[static 1], size_t datasize);

static void heapify(rrange_any array, size_t length_heap, size_t index, rrange_comparator comparator);

static void build_heap(rrange_any array, rrange_comparator comparator);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
void heapsort_sort(rrange_any array, rrange_comparator comparator)
{
    if (array.r->length == 0u) {
        return;
    }

    build_heap(array, comparator);

    size_t length_heap = array.r->length;

    for (size_t i = (array.r->length - 1u); i >= 1u; i--) {
        swap_pointed(array.r->data, (void *) ((uintptr_t) array.r->data + (uintptr_t) (i * array.stride)), array.stride);
        length_heap = length_heap - 1u;
        heapify(array, length_heap, 0u, comparator);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void swap_pointed(u8 pos1[static 1], u8 pos2[static 1], size_t datasize)
{
   u8 tmp;

   if (pos1 && pos2) {
      for (size_t i = 0 ; i < datasize ; i++) {
         tmp = pos1[i];
         pos1[i] = pos2[i];
         pos2[i] = tmp;
      }
   }
}

// -------------------------------------------------------------------------------------------------
static void heapify(rrange_any array, size_t length_heap, size_t index, rrange_comparator comparator)
{
    size_t imax;
    size_t left;
    size_t right;

    i32 heaped = 0u;

    while (!heaped) {
        left = LEFT(index);
        right = RIGHT(index);

        imax = index;

        if ((right < length_heap) && (comparator((void *) (array.r->data + (right * array.stride)), (void *) (array.r->data + (imax * array.stride))) == 1)) {
            imax = right;
        }

        if ((left < length_heap)  && (comparator((void *) (array.r->data + (left * array.stride)), (void *) (array.r->data + (imax * array.stride))) == 1)) {
            imax = left;
        }

        heaped = (imax == index);

        if (!heaped) {
            swap_pointed((void *) ((uintptr_t) array.r->data + (uintptr_t) (imax * array.stride)), (void *) ((uintptr_t) array.r->data + (uintptr_t) (index * array.stride)), array.stride);
            index = imax;
        }
    }
}

// -------------------------------------------------------------------------------------------------
static void build_heap(rrange_any array, rrange_comparator comparator)
{
    const size_t start_at = (size_t) ceil((f64) array.r->length / 2.0F);

    for (size_t i = start_at ; i > 0u ; i--) {
        heapify(array, array.r->length, i, comparator);
    }
    // last iteration, working with unsigned has its toll
    if (array.r->length > 0u) {
        heapify(array, array.r->length, 0u, comparator);
    }
}

#if 0
// -------------------------------------------------------------------------------------------------
bool is_sorted(range *array, range_comparator comparator)
{
    size_t pos = { 0u };

    if (array->length == 0) {
        return true;
    }

    pos = 1u;
    while ((pos < array->length) && (comparator((void *) range_at(array, pos), (void *) range_at(array, pos-1)) != -1)) {
        pos += 1u;
    }

    return (pos == array->length);
}
#endif

#ifdef UNITTESTING

i32 test_i32_comparator(const void *v1, const void *v2) {
    i32 val1 = *((i32 *) v1);
    i32 val2 = *((i32 *) v2);

    return (val1 > val2) - (val1 < val2);
}

tst_CREATE_TEST_SCENARIO(heap_sort,
        {
            rrange(i32, 10) to_sort;
            rrange(i32, 10) expected;
        },
        {
            heapsort_sort(rrange_to_any(&data->to_sort), &test_i32_comparator);

            for (size_t i = 0 ; i < 10 ; i++) {
                tst_assert_equal_ext(data->expected.data[i], data->to_sort.data[i], "value of %d", "at index %d", i);
            }

            // tst_assert(is_sorted((range *) &data->to_sort, &test_i32_comparator), "range is not sorted !");
        }
)

tst_CREATE_TEST_CASE(heap_sort_nominal, heap_sort,
        .to_sort =  rrange_create_static(i32, 10, { 2, 6, 3, 9, 8, 4, 1, 7, 5, 0 }),
        .expected = rrange_create_static(i32, 10, { 0, 1, 2, 3, 4, 5, 6, 7, 8 ,9 })
)
tst_CREATE_TEST_CASE(heap_sort_nothing, heap_sort,
        .to_sort =  rrange_create_static(i32, 10, { }),
        .expected = rrange_create_static(i32, 10, { })
)
tst_CREATE_TEST_CASE(heap_sort__one_element, heap_sort,
        .to_sort =  rrange_create_static(i32, 10, { 1 }),
        .expected = rrange_create_static(i32, 10, { 1 })
)

void heapsort_execute_unittests(void)
{
    tst_run_test_case(heap_sort_nominal);
    tst_run_test_case(heap_sort_nothing);
    tst_run_test_case(heap_sort__one_element);
}

#endif
