
#ifndef __UNSTANDARD_OUROBOROS_H__
#define __UNSTANDARD_OUROBOROS_H__

#include <ustd/common.h>

/**
 * @brief Ouroboros allocator data.
 *
 */
typedef struct ouroboros_t ouroboros_t;

/**
 * @brief Returns a pointer to a byte of the array only if the position lies in bounds of the allocated array.
 * It is assumed that the array has been directly allocated.
 *
 * @param target_array The target array which must have been directly allocated.
 * @param position index of the element. This is not the byte index, but the index as the nth element in the array.
 * @return a pointer to the element if it lies in bounds of the allocation, NULL otherwise.
 */
#define orbr_access_bounded(_target_array, _position) (typeof(_target_array)) orbr_access_bounded_raw((void*) _target_array, _position * sizeof(*_target_array))
void *orbr_access_bounded_raw(void *target_array, size_t byte_position);

/**
 * @brief Allocates bytes in the reserved buffer and returns the starting address of the allocated space.
 *
 * If no space could be found in the buffer, then returns NULL. Note that each time the service allocates
 * memory, a little bit of the reserved space is taken by the block's header : a 2048B array will not
 * fit in this allocator's 2048B buffer. Some space is also taken by the allocator data.
 *
 * The speed of the allocation is determined by the state of the buffer. As a rule of thumb, the more free()
 * there was before an alloc() call, the slower it is.
 *
 * @param alloc target allocator
 * @param wanted_size size, in number of bytes, requested to be allocated.
 * @return void* as the address of the allocated space.
 */
void *orbr_alloc(ouroboros_t *alloc, const size_t wanted_size);

/**
 * @brief Frees all of the allocator's blocks.
 * After this, all memory previously allocated is invalid and can be re-allocated.
 *
 * @param alloc target allocator
 */
void orbr_clear(ouroboros_t *alloc);

/**
 * @brief Initialize the allocator by giving it a fixed-size buffer to store objects.
 * It is the caller's responsability to  manage the lifetime of this memory.
 *
 * @param alloc target allocator
 * @param mem non-null memory address
 * @param size size of the available memory.
 */
ouroboros_t *orbr_create(void *mem, const size_t size);

/**
 * @brief Frees a specific address. The block right before the address is set to an invalid state, and the memory
 * itself is left untouched but will be overwritten during later allocations. This is a very fast operation.
 * Additional free() calls to the same address will be ignored, because the allocator has no mean to detect an
 * invalid free().
 *
 * @param alloc target allocator
 * @param object object to deallocate
 */
void orbr_free(ouroboros_t *alloc, void *object);

/**
 * @brief
 *
 * @param alloc
 * @param object
 * @param new_size
 * @return void*
 */
void *orbr_realloc(ouroboros_t *alloc, void *object, size_t new_size);

/**
 * @brief Returns the total space used by the allocator. This counts the block headers' sizes and the
 * allocator's struct data.
 *
 * @param alloc target allocator
 * @return size_t the total size used by the allocator.
 */
size_t orbr_space_used(ouroboros_t *alloc);

#endif