
#ifndef __TEEMING_TREE_H__
#define __TEEMING_TREE_H__

#include <ustd/common.h>
#include <ustd/allocation.h>

/**
 * @brief
 *
 */
typedef enum {
	TTREE_NO_MISHAP,

	TTREE_INCORRECT_PATH
} ttree_mishap;

#define TTREE_FOREACH_FLAG_INCLUDE_CHILDREN  (0x01u)
#define TTREE_FOREACH_FLAG_INCLUDE_PARENTS   (0x02u)
#define TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN (0x04u)
#define TTREE_FOREACH_FLAG_DIRECTION_DOWN_UP (0x08u)

/**
 * @brief Opaque type to pass around a tree stored contiguously in an array.
 *
 */
typedef struct ttree ttree;

/**
 * @brief
 * TODO : add a hash to identify the original state of the parent tree and invalidate the subtree if it changed
 */
typedef struct {
	ttree *parent_tree;
	size_t pos;
} subttree;

/**
 * @brief Creates an array-packed tree on the heap.
 *
 * @param[in] nb_nodes maximum number of nodes held inside the tree
 * @return ttree*
 */
ttree * ttree_create(allocator alloc, size_t nb_nodes);

/**
 * @brief Returns a subtree from a broader tree by its path. The subtree can then be used with other ttree interface methods to change the tree.
 *
 * @param[in] tree target tree, if it is changed during the lifetime of the subtree, the subtree may not be valid anymore
 * @param[in] node_path path to the subtree, node by node
 * @param[in] node_path_length length of the node path
 * @param[in] node_comparator function able to compare two nodes together
 * @return subttree*
 */
subttree ttree_get_subtree(ttree *tree, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2));

/**
 * @brief
 *
 * @param target
 * @param node_path
 * @param node_path_length
 * @param node_comparator
 * @return subttree
 */
subttree ttree_get_subtree_subtree(subttree target, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2));

/**
 * @brief Returns the contents of the node at the path described by the traget subtree.
 *
 * @param[in] target traget subtree
 * @return void*
 */
void * ttree_get_subtree_content(subttree target);

/**
 * @brief
 *
 * @param target
 * @return size_t
 */
size_t ttree_get_subtree_node_count(subttree target);

/**
 * @brief Inserts a new node (by shallow copy) into the tree, next to the target subtree.
 *
 * @param[inout] target target subtree
 * @param[in] node added node
 * @return i32 : 0 if the node was effectively added, > =1 if something went wrong
 */
i32 ttree_insert(subttree target, void *node);

/**
 * @brief Applies a function to all nodes in a subtree (depth first).
 *
 * @param[inout] target target subtree
 * @param[in] apply_f applied function
 * @param[in] additional_args pointer to additional arguments passed to the applied function
 */
void ttree_foreach(subttree target, void (*apply_f)(void **node, void *additional_args), void *additional_args, u32 config_flags);

/**
 * @brief Removes a subtree from the tree.
 *
 * @param[inout] target target subtree
 * @return i32 : 0 if the node was effectively removed, > =1 if something went wrong
 */
i32 ttree_remove(subttree target);

/**
 * @brief Releases the tree from the heap and nullifies the pointer to it.
 *
 * @param[inout] tree discarded tree
 */
void ttree_destroy(allocator alloc, ttree **tree);

#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif

#endif
