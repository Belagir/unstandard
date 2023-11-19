
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
    /// range of whatever memory
    range *tree_contents;
    /// range of unsigned long integers
    range *tree_children;
} ttree;

// -------------------------------------------------------------------------------------------------
typedef struct ttree_path {
    ttree *target;
    /// range of indexes
    range *tokens_indexes;
} ttree_path;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static ttree_path *ttree_path_create(allocator alloc, ttree *tree, size_t length);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
ttree *ttree_create(allocator alloc, size_t capacity, size_t element_size)
{
    ttree *tree = { };

    if ((capacity == 0u) || (element_size == 0u)) {
        return NULL;
    }

    tree = alloc.malloc(alloc, sizeof(*tree));

    if (tree) {
        tree->tree_contents = range_dynamic_create(alloc, element_size, capacity);
        tree->tree_children = range_dynamic_create(alloc, sizeof(size_t), capacity);
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
    range_dynamic_destroy(alloc, (*tree)->tree_children);

    alloc.free(alloc, *tree);
    *tree = NULL;

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_path *ttree_get_path(allocator alloc, ttree *tree, const range *elements_range, i32 (*comparator_f)(const void *elt_left,const void *elt_right))
{
    ttree_path *path = { };
    size_t pos_elements = { 0u };
    size_t pos   = { 0u };
    size_t limit = { 0u };

    if (!tree || !elements_range || !comparator_f) {
        return NULL;
    }

    path = ttree_path_create(alloc, tree, elements_range->length);
    if (!path) {
        return NULL;
    }

    limit = tree->tree_contents->length;
    while ((pos < limit) && (pos_elements < elements_range->length)) {
        if (comparator_f(range_at(elements_range, pos_elements), range_at(tree->tree_contents, pos)) == 0) {
            range_insert(path->tokens_indexes, path->tokens_indexes->length, (void *) &pos);
            pos += 1u;
            pos_elements += 1u;
        } else {
            pos += range_val(tree->tree_children, pos, size_t) + 1;
        }
    }

    if (pos_elements < elements_range->length) {
        ttree_path_destroy(alloc, &path);
    }

    return path;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_path_destroy(allocator alloc, ttree_path **path)
{
    if (!path || !*path) {
        return TTREE_INVALID_OBJECT;
    }

    range_dynamic_destroy(alloc, (*path)->tokens_indexes);

    alloc.free(alloc, *path);
    *path = NULL;

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static ttree_path *ttree_path_create(allocator alloc, ttree *tree, size_t length)
{
    ttree_path *path = { };

    path = alloc.malloc(alloc, sizeof(*path));

    if (path) {
        *path = (ttree_path) { .target = tree, .tokens_indexes = range_dynamic_create(alloc, sizeof(size_t), length) };
    }

    return path;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
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

static i32 test_comparator_u32(const void *elt_right, const void *elt_left) {
    u32 v_left  = *(u32 *) elt_left;
    u32 v_right = *(u32 *) elt_right;

    return (v_left > v_right) - (v_left < v_right);
}

tst_CREATE_TEST_SCENARIO(search_path,
        {
            range_static(10, u32)    tree_start_state_contents;
            range_static(10, size_t) tree_start_state_children;
            range_static(10, u32) searched_path;

            bool expect_found;
            range_static(10, size_t) expected_indexes;
        },
        {
            ttree tree = {};
            tree.tree_contents = (range *) &data->tree_start_state_contents;
            tree.tree_children = (range *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path(make_system_allocator(), &tree, (range *) &data->searched_path, &test_comparator_u32);

            if (data->expect_found) {
                tst_assert(path != NULL, "path was not found");
            } else {
                tst_assert(path == NULL, "path was found anyway");
            }

            if (path) {
                tst_assert_equal(data->expected_indexes.length, path->tokens_indexes->length, "a length of %d");

                for (size_t i = 0 ; i < data->searched_path.length ; i++) {
                    tst_assert(range_val(&data->expected_indexes, i, size_t) == range_val(path->tokens_indexes, i, size_t),
                            "data at index %d mismatch, expected %d, got %d", i,
                            range_val(&data->expected_indexes, i, size_t), range_val(path->tokens_indexes, i, size_t));
                }

                ttree_path_destroy(make_system_allocator(), &path);
            }
        }
)

tst_CREATE_TEST_CASE(search_present, search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 84, 86),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t, 3, 4, 6),
)
tst_CREATE_TEST_CASE(search_present_node, search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 87, 88),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t, 7, 8),
)
tst_CREATE_TEST_CASE(search_absent, search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 88, 86),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(search_too_far, search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 84, 86, 91),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(search_empty_path, search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(search_empty_tree, search_path,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .searched_path             = range_static_create(10, u32, 1),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(search_empty_and_empty, search_path,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .searched_path             = range_static_create(10, u32),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
void ttree_execute_unittests(void)
{
    tst_run_test_case(tree_lifetime_u32);
    tst_run_test_case(tree_lifetime_1ko);
    tst_run_test_case(tree_lifetime_bad_size);
    tst_run_test_case(tree_lifetime_bad_count);

    tst_run_test_case(search_present);
    tst_run_test_case(search_present_node);
    tst_run_test_case(search_absent);
    tst_run_test_case(search_too_far);
    tst_run_test_case(search_empty_path);
    tst_run_test_case(search_empty_tree);
    tst_run_test_case(search_empty_and_empty);
}

#endif
