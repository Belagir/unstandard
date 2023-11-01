
#ifndef __ALLOCATION_H__
#define __ALLOCATION_H__

#include <ustd/common.h>

/**
 * @brief Allocator structure to pass around custom allocators to method needing dynamic memory mangement.
 */
typedef struct allocator allocator;
typedef struct allocator {
    void *(*malloc)(allocator, size_t);     /** pointer to alloc() memory */
    void (*free)(allocator, void *);        /** pointer to free() memory */
    void *allocator_data;                   /** pointer to the allocator's optinal data */
} allocator;

/**
 * @brief Builds an allocator based on the system's malloc() and free().
 *
 * @return allocator
 */
allocator make_system_allocator(void);

/**
 * @brief Builds an allocator from some memory. The allocator will reside in the supplied memory, and so its lifetime is linked to the memory's lifetime.
 *
 * @param[inout] mem memory the allocator can use
 * @param[in] length length in bytes of the memory region
 * @return allocator
 */
allocator make_static_allocator(byte *mem, size_t length);

#endif
