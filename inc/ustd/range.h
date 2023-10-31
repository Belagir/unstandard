
#ifndef __RANGE_H__
#define __RANGE_H__

#include <ustd/common.h>
#include <ustd/allocation.h>

typedef struct {
    size_t stride;
    size_t length;
    size_t capacity;

    allocator alloc;

    byte *data;
} range;

#define range_create(__type, __capacity, __alloc) range_create_raw(sizeof(__type), __capacity, __alloc)
range range_create_raw(size_t stride, size_t capacity, allocator alloc);

range range_set_elt(range r, size_t pos, byte element[static 1]);
void range_get_elt(const range r, size_t pos, byte element[static 1]);

range range_destroy(range r);

#endif
