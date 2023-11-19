
#ifndef __TEEMING_TREE_H__
#define __TEEMING_TREE_H__

#include <ustd/allocation.h>

/**
 * @brief
 *
 */
typedef enum ttree_mishap {
	TTREE_NO_MISHAP,

    TTREE_INVALID_OBJECT,
} ttree_mishap;

/**
 * @brief Opaque type to pass around a tree stored contiguously in an array.
 *
 */
typedef struct ttree ttree;

typedef struct ttree_path {
    ttree *target;
    range *tokens_indexes;
} ttree_path;

/**
 * @brief Creates a tree of a with an allocator.
 *
 * @param[inout] alloc
 * @param[in] capacity
 * @param[in] element_size
 * @return ttree*
 */
ttree *ttree_create(allocator alloc, size_t capacity, size_t element_size);

/**
 * @brief Destroys a tree.
 *
 * @param[inout] alloc
 * @param[inout] tree
 */
ttree_mishap ttree_destroy(allocator alloc, ttree **tree);

#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif

#endif
