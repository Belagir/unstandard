
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
            limit = pos + range_val(tree->tree_children, pos, size_t) + 1;
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
byte *ttree_path_content(const ttree_path *path)
{
    if (!path || (path->tokens_indexes->length == 0)) {
        return NULL;
    }

    return range_at(path->target->tree_contents, range_val_back(path->tokens_indexes, size_t));
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_add(ttree *tree, const ttree_path *path, const byte *value)
{
    size_t insertion_pos = { };
    bool insertion_success = { };

    if (!tree || !value) {
        return TTREE_INVALID_OBJECT;
    } else if (!path || (path->target != tree)) {
        return TTREE_BAD_PATH;
    } else if (tree->tree_children->length == tree->tree_children->capacity) {
        return TTREE_OUT_OF_MEM;
    }

    if (path->tokens_indexes->length > 0) {
        insertion_pos = range_val_back(path->tokens_indexes, size_t) + 1;
    } else {
        insertion_pos = 0;
    }

    // insert a new entry in both ranges for the new element
    insertion_success = range_insert_value(tree->tree_contents, insertion_pos, value)
                        && range_insert_value(tree->tree_children, insertion_pos, &(size_t) { 0 });

    // increment all of the parents' children count
    for (size_t i = 0 ; i < path->tokens_indexes->length ; i++) {
        range_val(tree->tree_children, range_val(path->tokens_indexes, i, size_t), size_t) += 1;
    }

    return (insertion_success) ? TTREE_NO_MISHAP : TTREE_OTHER_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_remove(ttree *tree, const ttree_path *path)
{
    size_t removal_pos = { };
    size_t removal_length = { };
    bool removal_success = { };

    if (!tree) {
        return TTREE_INVALID_OBJECT;
    } else if (!path || (path->target != tree)) {
        return TTREE_BAD_PATH;
    }

    if (path->tokens_indexes->length == 0) {
        range_clear(tree->tree_contents);
        range_clear(tree->tree_children);
        return TTREE_NO_MISHAP;
    }

    removal_pos = range_val_back(path->tokens_indexes, size_t);
    removal_length = range_val(tree->tree_children, removal_pos, size_t) + 1;

    for (size_t i = 0 ; i < path->tokens_indexes->length ; i++) {
        range_val(tree->tree_children, range_val(path->tokens_indexes, i, size_t), size_t) -= 1;
    }

    removal_success = range_remove_interval(tree->tree_contents, removal_pos, removal_pos + removal_length)
                      && range_remove_interval(tree->tree_children, removal_pos, removal_pos + removal_length);

    return (removal_success) ? TTREE_NO_MISHAP : TTREE_OTHER_MISHAP;
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

tst_CREATE_TEST_SCENARIO(tree_search_path,
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

tst_CREATE_TEST_CASE(tree_search_present, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 84, 86),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t, 3, 4, 6),
)
tst_CREATE_TEST_CASE(tree_search_present_node, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 87, 88),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t, 7, 8),
)
tst_CREATE_TEST_CASE(tree_search_absent, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 88, 86),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_search_too_far, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32, 83, 84, 86, 91),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_search_empty_path, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   80, 81, 82, 83, 84, 85, 86, 87, 88, 89),
        .tree_start_state_children = range_static_create(10, size_t, 2,  0,  0,  3,  2,  0,  0,  2,  1,  0),
        .searched_path             = range_static_create(10, u32),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_search_empty_tree, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .searched_path             = range_static_create(10, u32, 1),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_search_empty_and_empty, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .searched_path             = range_static_create(10, u32),
        .expect_found              = true,
        .expected_indexes          = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_search_adjacent, tree_search_path,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .searched_path             = range_static_create(10, u32, 41, 42, 44),
        .expect_found              = false,
        .expected_indexes          = range_static_create(10, size_t),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_add_element,
        {
            range_static(10, u32)    tree_start_state_contents;
            range_static(10, size_t) tree_start_state_children;
            range_static(10, u32) addition_path;
            u32 added_value;

            bool expect_addition;
            range_static(10, u32)    tree_end_state_contents;
            range_static(10, size_t) tree_end_state_children;
        },
        {
            ttree tree = {};
            ttree_mishap result = {};

            tree.tree_contents = (range *) &data->tree_start_state_contents;
            tree.tree_children = (range *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path(make_system_allocator(), &tree, (range *) &data->addition_path, &test_comparator_u32);

            result = ttree_add(&tree, path, (const byte *) &data->added_value);

            if (data->expect_addition) {
                tst_assert(result == TTREE_NO_MISHAP, "addition failed !");
            } else {
                tst_assert(result != TTREE_NO_MISHAP, "addition passed anyway !");
            }

            for (size_t i = 0 ; i < tree.tree_contents->length ; i++) {
                tst_assert_equal(range_val(tree.tree_contents, i, u32),    range_val(&data->tree_end_state_contents, i, u32),    "tree content of %d");
                tst_assert_equal(range_val(tree.tree_children, i, size_t), range_val(&data->tree_end_state_children, i, size_t), "children count of %d");
            }

            if (path) {
                ttree_path_destroy(make_system_allocator(), &path);
            }
        }
)

tst_CREATE_TEST_CASE(tree_add_element_in_empty, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .addition_path             = range_static_create(10, u32),
        .added_value               = 42,
        .expect_addition           = true,
        .tree_end_state_contents   = range_static_create(10, u32,   42),
        .tree_end_state_children   = range_static_create(10, size_t, 0),
)
tst_CREATE_TEST_CASE(tree_add_element_at_root, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32,   41, 42, 43),
        .tree_start_state_children = range_static_create(10, size_t, 2,  1,  0),
        .addition_path             = range_static_create(10, u32),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_static_create(10, u32,   99, 41, 42, 43),
        .tree_end_state_children   = range_static_create(10, size_t, 0,  2,  1,  0),
)
tst_CREATE_TEST_CASE(tree_add_element_at_node, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32,   41, 42, 43),
        .tree_start_state_children = range_static_create(10, size_t, 2,  1,  0),
        .addition_path             = range_static_create(10, u32, 41),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_static_create(10, u32,   41, 99, 42, 43),
        .tree_end_state_children   = range_static_create(10, size_t, 3,  0,  1,  0),
)
tst_CREATE_TEST_CASE(tree_add_element_at_node_far, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32,   41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 2,  1,  0,  3,  0,  0,  0,  1,  0),
        .addition_path             = range_static_create(10, u32, 44, 45),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_static_create(10, u32,   41, 42, 43, 44, 45, 99, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 2,  1,  0,  4,  1,  0,  0,  0,  1,  0),
)
tst_CREATE_TEST_CASE(tree_add_element_bad_path, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32,   41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 2,  1,  0,  3,  0,  0,  0,  1,  0),
        .addition_path             = range_static_create(10, u32, 41, 45),
        .added_value               = 99,
        .expect_addition           = false,
        .tree_end_state_contents   = range_static_create(10, u32,   41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 2,  1,  0,  3,  0,  0,  0,  1,  0),

)
tst_CREATE_TEST_CASE(tree_add_element_full, tree_add_element,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .addition_path             = range_static_create(10, u32),
        .added_value               = 99,
        .expect_addition           = false,
        .tree_end_state_contents   = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_remove_element,
        {
            range_static(10, u32)    tree_start_state_contents;
            range_static(10, size_t) tree_start_state_children;
            range_static(10, u32) removal_path;

            bool expect_removal;
            range_static(10, u32)    tree_end_state_contents;
            range_static(10, size_t) tree_end_state_children;

        },
        {
            ttree tree = {};
            ttree_mishap result = {};

            tree.tree_contents = (range *) &data->tree_start_state_contents;
            tree.tree_children = (range *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path(make_system_allocator(), &tree, (range *) &data->removal_path, &test_comparator_u32);

            result = ttree_remove(&tree, path);

            if (data->expect_removal) {
                tst_assert(result == TTREE_NO_MISHAP, "removal failed !");
            } else {
                tst_assert(result != TTREE_NO_MISHAP, "removal passed anyway !");
            }

            for (size_t i = 0 ; i < tree.tree_contents->length ; i++) {
                tst_assert_equal_ext(range_val(&data->tree_end_state_contents, i, u32),    range_val(tree.tree_contents, i, u32),    "tree content of %d",
                        "\t(index %d)", i);
                tst_assert_equal_ext(range_val(&data->tree_end_state_children, i, size_t), range_val(tree.tree_children, i, size_t), "children count of %d",
                        "\t(index %d)", i);
            }

            if (path) {
                ttree_path_destroy(make_system_allocator(), &path);
            }

        }
)
tst_CREATE_TEST_CASE(tree_remove_element_whole_empty, tree_remove_element,
        .tree_start_state_contents = range_static_create(10, u32),
        .tree_start_state_children = range_static_create(10, size_t),
        .removal_path              = range_static_create(10, u32),
        .expect_removal            = true,
        .tree_end_state_contents   = range_static_create(10, u32),
        .tree_end_state_children   = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_remove_element_whole_populated, tree_remove_element,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .removal_path              = range_static_create(10, u32),
        .expect_removal            = true,
        .tree_end_state_contents   = range_static_create(10, u32),
        .tree_end_state_children   = range_static_create(10, size_t),
)
tst_CREATE_TEST_CASE(tree_remove_leaf, tree_remove_element,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .removal_path              = range_static_create(10, u32, 41, 42, 43),
        .expect_removal            = true,
        .tree_end_state_contents   = range_static_create(10, u32,   40, 41, 42, 44, 45, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 0,  1,  0,  3,  0,  0,  0,  1,  0),
)
tst_CREATE_TEST_CASE(tree_remove_node, tree_remove_element,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .removal_path              = range_static_create(10, u32, 41),
        .expect_removal            = true,
        .tree_end_state_contents   = range_static_create(10, u32,   40, 44, 45, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 0,  3,  0,  0,  0,  1,  0),
)
tst_CREATE_TEST_CASE(tree_remove_bad_path, tree_remove_element,
        .tree_start_state_contents = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_start_state_children = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
        .removal_path              = range_static_create(10, u32, 41, 42, 44),
        .expect_removal            = false,
        .tree_end_state_contents   = range_static_create(10, u32,   40, 41, 42, 43, 44, 45, 46, 47, 48, 49),
        .tree_end_state_children   = range_static_create(10, size_t, 0,  2,  1,  0,  3,  0,  0,  0,  1,  0),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
void ttree_execute_unittests(void)
{
    tst_run_test_case(tree_lifetime_u32);
    tst_run_test_case(tree_lifetime_1ko);
    tst_run_test_case(tree_lifetime_bad_size);
    tst_run_test_case(tree_lifetime_bad_count);

    tst_run_test_case(tree_search_present);
    tst_run_test_case(tree_search_present_node);
    tst_run_test_case(tree_search_absent);
    tst_run_test_case(tree_search_too_far);
    tst_run_test_case(tree_search_empty_path);
    tst_run_test_case(tree_search_empty_tree);
    tst_run_test_case(tree_search_empty_and_empty);
    tst_run_test_case(tree_search_adjacent);

    tst_run_test_case(tree_add_element_in_empty);
    tst_run_test_case(tree_add_element_at_root);
    tst_run_test_case(tree_add_element_at_node);
    tst_run_test_case(tree_add_element_at_node_far);
    tst_run_test_case(tree_add_element_bad_path);
    tst_run_test_case(tree_add_element_full);

    tst_run_test_case(tree_remove_element_whole_empty);
    tst_run_test_case(tree_remove_element_whole_populated);
    tst_run_test_case(tree_remove_leaf);
    tst_run_test_case(tree_remove_node);
    tst_run_test_case(tree_remove_bad_path);
}

#endif
