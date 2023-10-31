
#ifndef __ALLOCATION_H__
#define __ALLOCATION_H__

#include <ustd/common.h>

typedef struct {
    void *(*malloc)(size_t);
    void (*free)(void *);
} allocator;

allocator allocator_system(void);

#endif
