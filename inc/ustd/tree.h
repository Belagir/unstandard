
#ifndef __TEEMING_TREE_H__
#define __TEEMING_TREE_H__

#include <ustd/allocation.h>

#include <ustd/experimental/range.h>

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
ttree_path *ttree_get_path_absolute(allocator alloc, ttree *tree, rrange_any elements_range, comparator_f comp);

/**
 * @brief
 *
 * @param alloc
 * @param path
 * @param elements_range
 * @param comp
 * @return ttree_path*
 */
ttree_path *ttree_get_path_relative(allocator alloc, ttree_path *path, rrange_any elements_range, comparator_f comp);

#define ttree_get_path(__alloc, X, __elements_range, __comp) ( _Generic(X, ttree * : ttree_get_path_absolute, ttree_path * : ttree_get_path_relative) )(__alloc, X, __elements_range, __comp)

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
void ttree_foreach_element(const ttree_path *path, void (apply_f)(void *element, void *additional_args), void *additional_args);

/**
 * @brief
 *
 * @param path
 * @param apply_f
 * @param additional_args
 */
void ttree_foreach_path(const ttree_path *path, void (apply_f)(ttree_path *some_path, void *additional_args), void *additional_args);

#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif


#endif