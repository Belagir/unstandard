
#include <ustd/common.h>
#include <ustd/range.h>
#include <ustd/tree.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
typedef struct ttree {
    range *tree_contents;
    range *tree_children_count;
} ttree;

// -------------------------------------------------------------------------------------------------
ttree *ttree_create(allocator alloc, size_t capacity, size_t element_size)
{
    ttree *tree = { };

    if ((capacity == 0u) || (element_size == 0u)) {
        return NULL;
    }

    tree = alloc.malloc(alloc, sizeof(*tree));

    if (tree) {
        tree->tree_contents       = range_dynamic_create(alloc, element_size, capacity);
        tree->tree_children_count = range_dynamic_create(alloc, sizeof(size_t), capacity);
    }

    return tree;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_destroy(allocator alloc, ttree **tree)
{
    if (!tree || !*tree) {
        return TTREE_INVALID_OBJECT;
    }

    range_dynamic_destroy(alloc, (*tree)->tree_contents);
    range_dynamic_destroy(alloc, (*tree)->tree_children_count);

    alloc.free(alloc, *tree);
    *tree = NULL;

    return TTREE_NO_MISHAP;
}


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(tree_lifetime,
        {
            size_t capacity;
            size_t element_size;

            bool expect_success;
        },
        {
            ttree *tree = ttree_create(make_system_allocator(), data->capacity, data->element_size);

            if (data->expect_success) {
                tst_assert((tree != NULL), "tree creation failed !");
            } else {
                tst_assert((tree == NULL), "tree was unexpectedly successful !");
            }

            if (tree) {
                ttree_destroy(make_system_allocator(), &tree);
            }


            tst_assert((tree == NULL), "tree was not set to NULL at the end of test");
        }
)

tst_CREATE_TEST_CASE(tree_lifetime_u32, tree_lifetime,
        .capacity       = 16,
        .element_size   = sizeof(u32),
        .expect_success = true,
)
tst_CREATE_TEST_CASE(tree_lifetime_1ko, tree_lifetime,
        .capacity       = 2048u,
        .element_size   = 1024u,
        .expect_success = true,
)
tst_CREATE_TEST_CASE(tree_lifetime_bad_size, tree_lifetime,
        .capacity       = 16,
        .element_size   = 0,
        .expect_success = false,
)
tst_CREATE_TEST_CASE(tree_lifetime_bad_count, tree_lifetime,
        .capacity       = 0,
        .element_size   = sizeof(long),
        .expect_success = false,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
void ttree_execute_unittests(void)
{
    tst_run_test_case(tree_lifetime_u32);
    tst_run_test_case(tree_lifetime_1ko);
    tst_run_test_case(tree_lifetime_bad_size);
    tst_run_test_case(tree_lifetime_bad_count);
}

#endif
