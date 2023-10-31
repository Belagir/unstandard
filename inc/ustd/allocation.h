
#ifndef __ALLOCATION_H__
#define __ALLOCATION_H__

#include <ustd/common.h>

typedef struct allocator allocator;
typedef struct allocator {
    void *(*malloc)(allocator, size_t);
    void (*free)(allocator, void *);
    void *allocator_data;
} allocator;

allocator make_system_allocator(void);

allocator make_static_allocator(byte *mem, size_t length);

#endif
