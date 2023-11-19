
#ifndef __TEEMING_TREE_H__
#define __TEEMING_TREE_H__

/**
 * @brief
 *
 */
typedef enum ttree_mishap {
	TTREE_NO_MISHAP,
} ttree_mishap;

/**
 * @brief Opaque type to pass around a tree stored contiguously in an array.
 *
 */
typedef struct ttree ttree;



#ifdef UNITTESTING
void ttree_execute_unittests(void);
#endif

#endif
