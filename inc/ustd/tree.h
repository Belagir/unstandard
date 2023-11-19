
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
    TTREE_BAD_PATH,
    TTREE_OUT_OF_MEM,
} ttree_mishap;

/**
 * @brief Opaque type to pass around a tree stored contiguously in an array.
 *
 */
typedef struct ttree ttree;

/**
 * @brief
 *
 */
typedef struct ttree_path ttree_path;

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

/**
 * @brief
 *
 * @param alloc
 * @param tree
 * @param elements_range
 * @return ttree_path
 */
ttree_path *ttree_get_path(allocator alloc, ttree *tree, const range *elements_range, i32 (*comparator_f)(const void *elt_left,const void *elt_right));

/**
 * @brief
 *
 * @param path
 * @return void*
 */
byte *ttree_path_content(const ttree_path *path);

/**
 * @brief
 *
 * @param tree
 * @param path
 * @param value
 * @return ttree_mishap
 */
ttree_mishap ttree_add(ttree *tree, const ttree_path *path, const byte *value);

/**
 * @brief
 *
 * @param alloc
 * @param path
 * @return ttree_mishap
 */
ttree_mishap ttree_path_destroy(allocator alloc, ttree_path **path);

#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif

#endif
