
#ifndef __RANGE_H__
#define __RANGE_H__

#include <ustd/common.h>
#include <ustd/allocation.h>

/**
 * @brief Type of a fixed-size range of arbitrary values.
 */
typedef struct {
    size_t stride;      /** size, in bytes, of each individual element in the range */
    size_t capacity;    /** maximum length, in number of elements, of the range */
    size_t length;      /** current number of elements in the range */
    byte data[];        /** actual data in the range as a byte array */
} range;

/**
 * @brief Creates a static range from values. This range lives in the context it has been created in.
 */
#define range_static_create(__capacity, __type, ...) \
        ((range *) &(struct { size_t stride; size_t capacity; size_t length; __type data[__capacity]; }) \
        { \
                .stride = sizeof(__type), \
                .capacity = __capacity,  \
                .length = count_of(((__type[]) {__VA_ARGS__})), \
                .data = {__VA_ARGS__} \
        })

// adding elements
bool range_insert(range *r, size_t index, void *value);
bool range_push_back(range *r, void *value);
bool range_push_front(range *r, void *value);
// + range insertion / in place concatenation with Generics

// deletion
bool range_remove(range *r, size_t index);
bool range_pop_back(range *r);
bool range_pop_front(range *r);
void range_clear(range *r);

// accession
#define range_at(__r, __i, __t) ( *((__t *) (((byte *) (__r)->data) + ((__i) * (__r)->stride))) )
#define range_front(__r, __t)   range_at(__r, 0, __t)
#define range_back(__r, __t)    range_at(__r, (__r->length - 1), __t)

// creation & destruction
range *range_dynamic_create(allocator alloc, size_t size_element, size_t nb_elements_max);
range *range_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, void *array);
range *range_dynamic_destroy(allocator alloc, range *r);

// ranges & ranges
range *range_concat(allocator alloc, range *r_left, range *r_right);
range *range_copy_of(allocator alloc, range *r);

#endif
