
#include <ustd/sorting.h>

#include <math.h>

#define PARENT(index) (((index) >> (1u))-(1u))
#define LEFT(index)   (((index) << (1u))+(1u))
#define RIGHT(index)  ((((index) << (1u)) | 0x1)+(1u))

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void swap_pointed(u8 pos1[static 1], u8 pos2[static 1], size_t datasize);

static void heapify(void *data, size_t size, size_t length, size_t index, i32 (*comparator)(const void*, const void*));

static void build_heap(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*));

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
void heapsort_sort(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*))
{
    build_heap(data, size, length, comparator);

    size_t length_heap = length;

    for (size_t i = (length - 1); i >= 1u; i--) {
        swap_pointed(data, (void *) ((uintptr_t) data + (uintptr_t) (i * size)), size);
        length_heap = length_heap - 1;
        heapify(data, size, length_heap, 0u, comparator);
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
static void heapify(void *data, size_t size, size_t length, size_t index, i32 (*comparator)(const void*, const void*))
{
    size_t imax;
    size_t left;
    size_t right;

    i32 heaped = 0u;

    while (!heaped) {
        left = LEFT(index);
        right = RIGHT(index);

        imax = index;

        if ((right < length) && (comparator((void *) ((uintptr_t) data + (uintptr_t) (right*size)), (void *) ((uintptr_t) data + (uintptr_t) (imax*size))) == 1)) {
            imax = right;
        }

        if ((left < length) && (comparator((void *) ((uintptr_t) data + (uintptr_t) (left*size)), (void *) ((uintptr_t) data + (uintptr_t) (imax*size))) == 1)) {
            imax = left;
        }

        heaped = (imax == index);

        if (!heaped) {
            swap_pointed((void *) ((uintptr_t) data + (uintptr_t) (imax*size)), (void *) ((uintptr_t) data + (uintptr_t) (index*size)), size);
            index = imax;
        }
    }
}

// -------------------------------------------------------------------------------------------------
static void build_heap(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*))
{
    const size_t start_at = (size_t) ceil((f64) length / 2.0F);

    for (size_t i = start_at ; i > 0u ; i--) {
        heapify(data, size, length, i, comparator);
    }
    // last iteration, working with unsigned has its toll
    if (length > 0u) {
        heapify(data, size, length, 0u, comparator);
    }
}

// -------------------------------------------------------------------------------------------------
i32 is_sorted(void *data, size_t size, size_t length, i32 (*comparator)(const void*, const void*))
{
    size_t pos = 0u;

    pos = 1u;
    while ((pos < length) && (comparator((void *) ((uintptr_t) data + (pos * size)), (void *) ((uintptr_t) data + ((pos-1) * size))) != -1)) {
        pos += 1u;
    }

    return (pos == length);
}
