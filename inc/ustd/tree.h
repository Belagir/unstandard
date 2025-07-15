/**
 * @file tree.h
 * @author gabriel
 * @brief Manipulate n-ary pre-allocated trees where each element is accessed through a path of other elements.
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef UNSTANDARD_TEEMING_TREE_H__
#define UNSTANDARD_TEEMING_TREE_H__

#include "allocation.h"

#include "range.h"

/**
 * @brief
 *
 */
typedef enum ttree_mishap {
	TTREE_NO_MISHAP,

    TTREE_INVALID_OBJECT,
    TTREE_BAD_PATH,
    TTREE_OUT_OF_MEM,

    TTREE_OTHER_MISHAP,
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
ttree_path *ttree_get_path_absolute(allocator alloc, ttree *tree, range_any elements_range, comparator_f comp);

/**
 * @brief
 *
 * @param alloc
 * @param path
 * @param elements_range
 * @param comp
 * @return ttree_path*
 */
ttree_path *ttree_get_path_relative(allocator alloc, ttree_path *path, range_any elements_range, comparator_f comp);

#define ttree_get_path(__alloc, X, elements_range, comp) ( _Generic(X, ttree * : ttree_get_path_absolute, ttree_path * : ttree_get_path_relative) )(__alloc, X, elements_range, comp)

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
 * @param tree
 * @param path
 * @return ttree_mishap
 */
ttree_mishap ttree_remove(ttree *tree, const ttree_path *path);

/**
 * @brief
 *
 * @param alloc
 * @param path
 * @return ttree_mishap
 */
ttree_mishap ttree_path_destroy(allocator alloc, ttree_path **path);

/**
 * @brief
 *
 * @param path
 * @param apply_f
 * @param additional_args
 */
ttree_mishap ttree_foreach_element(const ttree_path *path, void (apply_f)(void *element, void *additional_args), void *additional_args);

/**
 * @brief
 *
 * @param path
 * @param apply_f
 * @param additional_args
 */
ttree_mishap ttree_foreach_path(allocator alloc, const ttree_path *path, void (apply_f)(ttree_path *some_path, void *additional_args), void *additional_args);

#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif


#endif