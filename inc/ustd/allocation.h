/**
 * @file allocation.h
 * @author gabriel
 * @brief Provides declarations for the custom allocators used widely in the library.
 * Custom allocators permit finer control over what is happening with dynamic memory, allowing different methods of management to be used in a program. If the user has the discipline to always inject the allocators through arguments up to the context that will use the allocator's methods, it is trivial to recognize which parts of the code mess with dynamic memory at a glance. This has the downside of having the developper needing to remember what allocator created what object.
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 */

#ifndef UNSTANDARD_ALLOCATION_H__
#define UNSTANDARD_ALLOCATION_H__

#include "common.h"

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
 * @brief Builds an allocator based on the system's malloc() and free() from stdlib.
 *
 * @return allocator
 */
allocator make_system_allocator(void);

/**
 * @brief Builds an allocator from some memory. The allocator will reside in the supplied memory, and so its lifetime is linked to the memory's lifetime.
 * This allocator is very fast, but is vulnerable to out of bound writes and fragmentation (who thought !). Note that the allocator will use some of the passed memory for itself, so not all bytes that are given to the allocator will be available to be allocated.
 *
 *
 * @param[inout] mem memory the allocator can use
 * @param[in] length length in bytes of the memory region
 * @return allocator
 */
allocator make_static_allocator(byte *mem, size_t length);

#endif
