
#include <ustd/common.h>
#include <ustd_impl/range_impl.h>
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
    range_any tree_contents;
    /// range of unsigned long integers
    range(size_t) *tree_children;
} ttree;

// -------------------------------------------------------------------------------------------------
typedef struct ttree_path {
    ttree *target;
    /// range of indexes
    range(size_t) *tokens_indexes;
} ttree_path;


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static ttree_path *ttree_path_create(allocator alloc, ttree *tree, size_t length);

static void ttree_path_truncate_to_first_parent(ttree_path *path, size_t index);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
ttree *ttree_create(allocator alloc, size_t capacity, size_t element_size)
{
    ttree *tree = { };
    range_any tree_impl_contents = range_create_dynamic_as_any(alloc, element_size, capacity);

    if ((capacity == 0u) || (element_size == 0u)) {
        return NULL;
    }

    tree = alloc.malloc(alloc, sizeof(*tree));

    if (tree) {
        bytewise_copy(&tree->tree_contents, &tree_impl_contents, sizeof(tree->tree_contents));
        tree->tree_children = range_create_dynamic(alloc, sizeof(size_t), capacity);
    }

    return tree;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_destroy(allocator alloc, ttree **tree)
{
    if (!tree || !*tree) {
        return TTREE_INVALID_OBJECT;
    }

    range_destroy_dynamic(alloc, &((*tree)->tree_contents));
    range_destroy_dynamic(alloc, &range_to_any((*tree)->tree_children));

    alloc.free(alloc, *tree);
    *tree = NULL;

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_path *ttree_get_path_absolute(allocator alloc, ttree *tree, range_any elements_range, comparator_f comp)
{
    ttree_path *root_path = ttree_path_create(alloc, tree, 0);
    ttree_path *path = ttree_get_path_relative(alloc, root_path, elements_range, comp);
    ttree_path_destroy(alloc, &root_path);

    return path;
}

// -------------------------------------------------------------------------------------------------
ttree_path *ttree_get_path_relative(allocator alloc, ttree_path *base_path, range_any elements_range, comparator_f comp)
{
    ttree_path *new_path = { };
    ttree *tree = { };
    size_t original_path_length = { 0u };
    size_t pos_elements = { 0u };
    size_t pos   = { 0u };
    size_t limit = { 0u };

    // preconditions checks
    if (!elements_range.r || !comp || !base_path) {
        return NULL;
    }

    // for convenience
    tree = base_path->target;

    // creating / allocating the returned path
    if (base_path && base_path->tokens_indexes) {
        original_path_length = base_path->tokens_indexes->length;
    }
    new_path = ttree_path_create(alloc, tree, original_path_length + elements_range.r->length);
    if (!new_path) {
        return NULL;
    }

    // copying the other path's contents into the new one
    range_insert_range(range_to_any(new_path->tokens_indexes), 0u, range_to_any(base_path->tokens_indexes));

    // computing the starting pos & limit of the search for the elements
    if (!base_path->tokens_indexes || (base_path->tokens_indexes->length == 0u)) {
        // from the root of the tree :
        pos = 0u;
        limit = tree->tree_children->length;
    } else {
        // from some point in the tree :
        pos = new_path->tokens_indexes->data[new_path->tokens_indexes->length - 1u] + 1u;
        limit = pos + tree->tree_children->data[pos] + 1u;
    }

    // searching the elements
    while ((pos < limit) && (pos_elements < elements_range.r->length)) {
        if (comp(range_at(elements_range, pos_elements), range_at(tree->tree_contents, pos)) == 0u) {
            range_insert_value(range_to_any(new_path->tokens_indexes), new_path->tokens_indexes->length, (void *) &pos);
            limit = pos + tree->tree_children->data[pos] + 1u;
            pos += 1u;
            pos_elements += 1u;
        } else {
            pos += tree->tree_children->data[pos] + 1u;
        }
    }

    // loop had to exit prematurely : the element range does not represent a node in the tree
    if (pos_elements < elements_range.r->length) {
        ttree_path_destroy(alloc, &new_path);
    }
    // happy path
    return new_path;
}

// -------------------------------------------------------------------------------------------------
byte *ttree_path_content(const ttree_path *path)
{
    if (!path || !path->tokens_indexes || (path->tokens_indexes->length == 0)) {
        return NULL;
    }

    return range_at(path->target->tree_contents, path->tokens_indexes->data[path->tokens_indexes->length - 1u]);
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

    if (path->tokens_indexes->length > 0u) {
        insertion_pos = path->tokens_indexes->data[ path->tokens_indexes->length - 1u] + 1u;
    } else {
        insertion_pos = 0u;
    }

    // insert a new entry in both ranges for the new element
    insertion_success = range_insert_value(tree->tree_contents, insertion_pos, value)
                        && range_insert_value(range_to_any(tree->tree_children), insertion_pos, &(size_t) { 0u });

    // increment all of the parents' children count
    for (size_t i = 0 ; i < path->tokens_indexes->length ; i++) {
        tree->tree_children->data[path->tokens_indexes->data[i]] += 1u;
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

    if (path->tokens_indexes->length == 0u) {
        range_clear(tree->tree_contents);
        range_clear(range_to_any(tree->tree_children));
        return TTREE_NO_MISHAP;
    }

    removal_pos = path->tokens_indexes->data[path->tokens_indexes->length - 1u];
    removal_length = tree->tree_children->data[removal_pos] + 1u;

    for (size_t i = 0 ; i < path->tokens_indexes->length ; i++) {
        tree->tree_children->data[path->tokens_indexes->data[i]] -= 1u;
    }

    removal_success = range_remove_interval(tree->tree_contents, removal_pos, removal_pos + removal_length)
                      && range_remove_interval(range_to_any(tree->tree_children), removal_pos, removal_pos + removal_length);

    return (removal_success) ? TTREE_NO_MISHAP : TTREE_OTHER_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_path_destroy(allocator alloc, ttree_path **path)
{
    if (!path || !*path) {
        return TTREE_INVALID_OBJECT;
    }

    range_destroy_dynamic(alloc, &range_to_any((*path)->tokens_indexes));

    alloc.free(alloc, *path);
    *path = NULL;

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_foreach_element(const ttree_path *path, void (apply_f)(void *element, void *additional_args), void *additional_args)
{
    size_t apply_from = { };
    size_t apply_to = { };

    if (!path) {
        return TTREE_BAD_PATH;
    } else if (!apply_f) {
        return TTREE_INVALID_OBJECT;
    }

    if (path->tokens_indexes->length > 0u) {
        apply_from = path->tokens_indexes->data[path->tokens_indexes->length - 1u];
        apply_to = apply_from + path->target->tree_children->data[apply_from] + 1u;
    } else {
        apply_from = 0u;
        apply_to = path->target->tree_contents.r->length;
    }

    for (size_t i = apply_from ; i < apply_to ; i++) {
        apply_f(range_at(path->target->tree_contents, i), additional_args);
    }

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
ttree_mishap ttree_foreach_path(allocator alloc, const ttree_path *path, void (apply_f)(ttree_path *some_path, void *additional_args), void *additional_args)
{
    size_t apply_from = { };
    size_t apply_to = { };
    ttree_path applied_on_path = { };
    size_t last_parent_index = { };

    if (!path) {
        return TTREE_BAD_PATH;
    } else if (!apply_f) {
        return TTREE_INVALID_OBJECT;
    }

    if (path->tokens_indexes->length > 0u) {
        apply_from = path->tokens_indexes->data[path->tokens_indexes->length - 1u];
        apply_to = apply_from + path->target->tree_children->data[apply_from] + 1u;
    } else {
        apply_from = 0u;
        apply_to = path->target->tree_contents.r->length;
    }

    applied_on_path.target = path->target;
    applied_on_path.tokens_indexes = range_create_dynamic_from_resize_of(alloc, range_to_any(path->tokens_indexes), path->tokens_indexes->length + (apply_to - apply_from));

    for (size_t i = apply_from ; i < apply_to ; i++) {
        ttree_path_truncate_to_first_parent(&applied_on_path, i);
        range_insert_value(range_to_any(applied_on_path.tokens_indexes), applied_on_path.tokens_indexes->length, &i);

        apply_f(&applied_on_path, additional_args);
    }

    range_destroy_dynamic(alloc, &range_to_any(applied_on_path.tokens_indexes));

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
        *path = (ttree_path) { .target = tree, .tokens_indexes = range_create_dynamic(alloc, sizeof(size_t), length) };
    }

    return path;
}

// -------------------------------------------------------------------------------------------------
static void ttree_path_truncate_to_first_parent(ttree_path *path, size_t index)
{
    size_t last_index = { };

    if (!path || !path->target || !path->tokens_indexes || (index >= path->target->tree_children->length)) {
        return;
    }

    last_index = path->tokens_indexes->data[path->tokens_indexes->length - 1u];
    while ((path->tokens_indexes->length > 0u) && (last_index + path->target->tree_children->data[last_index] < index)) {
        range_remove(range_to_any(path->tokens_indexes), path->tokens_indexes->length - 1u);
        last_index = path->tokens_indexes->data[path->tokens_indexes->length - 1u];
    }
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
            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;
            range(u32, 10) searched_path;

            bool expect_found;
            range(size_t, 10) expected_indexes;
        },
        {
            ttree tree = {};
            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->searched_path), &test_comparator_u32);

            if (data->expect_found) {
                tst_assert(path != NULL, "path was not found");
            } else {
                tst_assert(path == NULL, "path was found anyway");
            }

            if (path) {
                tst_assert_equal(data->expected_indexes.length, path->tokens_indexes->length, "a length of %d");

                for (size_t i = 0 ; i < data->searched_path.length ; i++) {
                    tst_assert_equal_ext(data->expected_indexes.data[i], path->tokens_indexes->data[i], "value of %d", "at index %d", i);
                }

                ttree_path_destroy(make_system_allocator(), &path);
            }
        }
)

tst_CREATE_TEST_CASE(tree_search_present, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 83, 84, 86 }),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, { 3, 4, 6 }),
)
tst_CREATE_TEST_CASE(tree_search_present_node, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 87, 88 }),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, { 7, 8 }),
)
tst_CREATE_TEST_CASE(tree_search_absent, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 83, 88, 86 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_too_far, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 83, 84, 86, 91 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_empty_path, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, {  } ),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_empty_tree, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10, {  }),
        .tree_start_state_children = range_create_static(size_t, 10, {  }),
        .searched_path             = range_create_static(u32, 10, { 1 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_empty_and_empty, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10, {  }),
        .tree_start_state_children = range_create_static(size_t, 10, {  }),
        .searched_path             = range_create_static(u32, 10, {  }),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_adjacent, tree_search_path,
        .tree_start_state_contents = range_create_static(u32, 10,    {40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, {0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 41, 42, 44 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, { }),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(search_relative,
    {
            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;

            range(u32, 10) searched_path;
            range(u32, 10) relative_to_path;

            bool expect_found;
            range(size_t, 10) expected_indexes;
    },
    {
            ttree tree = {};
            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *base_path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->relative_to_path), &test_comparator_u32);
            ttree_path *path = ttree_get_path_relative(make_system_allocator(), base_path, range_to_any(&data->searched_path), &test_comparator_u32);

            if (data->expect_found) {
                tst_assert(path != NULL, "path was not found");
            } else {
                tst_assert(path == NULL, "path was found anyway");
            }

            if (path) {
                tst_assert_equal(data->expected_indexes.length, path->tokens_indexes->length, "a length of %d");

                for (size_t i = 0 ; i < data->searched_path.length ; i++) {
                    tst_assert_equal_ext(data->expected_indexes.data[i], path->tokens_indexes->data[i], "value of %d", "at index %d", i);
                }

                ttree_path_destroy(make_system_allocator(), &path);
                ttree_path_destroy(make_system_allocator(), &base_path);
            }

    }
)
tst_CREATE_TEST_CASE(tree_search_relative_valid_and_valid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 81 }),
        .relative_to_path          = range_create_static(u32, 10, { 80 }),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, { 0, 1 }),
)
tst_CREATE_TEST_CASE(tree_search_relative_invalid_and_valid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 81, 85 }),
        .relative_to_path          = range_create_static(u32, 10, { 80 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_empty_and_valid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, {  }),
        .relative_to_path          = range_create_static(u32, 10, { 83, 84 }),
        .expect_found              = true,
        .expected_indexes          = range_create_static(size_t, 10, { 3, 4 }),
)
tst_CREATE_TEST_CASE(tree_search_relative_toolong_and_valid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 85, 86 }),
        .relative_to_path          = range_create_static(u32, 10, { 83, 84 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_valid_and_invalid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 87, 88 }),
        .relative_to_path          = range_create_static(u32, 10, { 88 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_invalid_and_invalid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 89, 88 }),
        .relative_to_path          = range_create_static(u32, 10, { 88 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_empty_and_invalid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, {  }),
        .relative_to_path          = range_create_static(u32, 10, { 88 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_toolong_and_invalid, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, { 83, 84, 85, 86 }),
        .relative_to_path          = range_create_static(u32, 10, { 88 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_search_relative_empty_and_toolong, search_relative,
        .tree_start_state_contents = range_create_static(u32, 10,   { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  0,  0,  3,  2,  0,  0,  2,  1,  0 }),
        .searched_path             = range_create_static(u32, 10, {  }),
        .relative_to_path          = range_create_static(u32, 10, { 83, 84, 85, 86 }),
        .expect_found              = false,
        .expected_indexes          = range_create_static(size_t, 10, {  }),
)


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_add_element,
        {
            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;
            range(u32, 10) addition_path;
            u32 added_value;

            bool expect_addition;
            range(u32, 10)    tree_end_state_contents;
            range(size_t, 10) tree_end_state_children;
        },
        {
            ttree tree = {};
            ttree_mishap result = {};

            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->addition_path), &test_comparator_u32);

            result = ttree_add(&tree, path, (const byte *) &data->added_value);

            if (data->expect_addition) {
                tst_assert(result == TTREE_NO_MISHAP, "addition failed !");
            } else {
                tst_assert(result != TTREE_NO_MISHAP, "addition passed anyway !");
            }

            for (size_t i = 0 ; i < tree.tree_contents.r->length ; i++) {
                tst_assert_equal_ext(data->tree_end_state_contents.data[i], *(u32 *) range_at(tree.tree_contents, i), "value of %d", "at index %d", i);
                tst_assert_equal_ext(data->tree_end_state_children.data[i], tree.tree_children->data[i], "value of %d", "at index %d", i);
            }

            if (path) {
                ttree_path_destroy(make_system_allocator(), &path);
            }
        }
)


tst_CREATE_TEST_CASE(tree_add_element_in_empty, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10, {  }),
        .tree_start_state_children = range_create_static(size_t, 10, {  }),
        .addition_path             = range_create_static(u32, 10, {  }),
        .added_value               = 42,
        .expect_addition           = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 42 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0 }),
)
tst_CREATE_TEST_CASE(tree_add_element_at_root, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 41, 42, 43 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  1,  0 }),
        .addition_path             = range_create_static(u32, 10, {  }),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 99, 41, 42, 43 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0,  2,  1,  0 }),
)
tst_CREATE_TEST_CASE(tree_add_element_at_node, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 41, 42, 43 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  1,  0 }),
        .addition_path             = range_create_static(u32, 10, { 41 }),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 41, 99, 42, 43 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 3,  0,  1,  0 }),
)
tst_CREATE_TEST_CASE(tree_add_element_at_node_far, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .addition_path             = range_create_static(u32, 10, { 44, 45 }),
        .added_value               = 99,
        .expect_addition           = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 41, 42, 43, 44, 45, 99, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 2,  1,  0,  4,  1,  0,  0,  0,  1,  0 }),
)
tst_CREATE_TEST_CASE(tree_add_element_bad_path, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .addition_path             = range_create_static(u32, 10, { 41, 45 }),
        .added_value               = 99,
        .expect_addition           = false,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 2,  1,  0,  3,  0,  0,  0,  1,  0 }),

)
tst_CREATE_TEST_CASE(tree_add_element_full, tree_add_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .addition_path             = range_create_static(u32, 10, {  }),
        .added_value               = 99,
        .expect_addition           = false,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_remove_element,
        {
            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;
            range(u32, 10) removal_path;

            bool expect_removal;
            range(u32, 10)    tree_end_state_contents;
            range(size_t, 10) tree_end_state_children;

        },
        {
            ttree tree = {};
            ttree_mishap result = {};

            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->removal_path), &test_comparator_u32);

            result = ttree_remove(&tree, path);

            if (data->expect_removal) {
                tst_assert(result == TTREE_NO_MISHAP, "removal failed !");
            } else {
                tst_assert(result != TTREE_NO_MISHAP, "removal passed anyway !");
            }

            for (size_t i = 0 ; i < tree.tree_contents.r->length ; i++) {
                tst_assert_equal_ext(data->tree_end_state_contents.data[i], *(u32 *) range_at(tree.tree_contents, i), "value of %d", "at index %d", i);
                tst_assert_equal_ext(data->tree_end_state_children.data[i], tree.tree_children->data[i], "value of %d", "at index %d", i);
            }

            if (path) {
                ttree_path_destroy(make_system_allocator(), &path);
            }

        }
)
tst_CREATE_TEST_CASE(tree_remove_element_whole_empty, tree_remove_element,
        .tree_start_state_contents = range_create_static(u32, 10, {  }),
        .tree_start_state_children = range_create_static(size_t, 10, {  }),
        .removal_path              = range_create_static(u32, 10, {  }),
        .expect_removal            = true,
        .tree_end_state_contents   = range_create_static(u32, 10, {  }),
        .tree_end_state_children   = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_remove_element_whole_populated, tree_remove_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .removal_path              = range_create_static(u32, 10, {  }),
        .expect_removal            = true,
        .tree_end_state_contents   = range_create_static(u32, 10, {  }),
        .tree_end_state_children   = range_create_static(size_t, 10, {  }),
)
tst_CREATE_TEST_CASE(tree_remove_leaf, tree_remove_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .removal_path              = range_create_static(u32, 10, { 41, 42, 43 }),
        .expect_removal            = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 40, 41, 42, 44, 45, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0,  1,  0,  3,  0,  0,  0,  1,  0 }),
)
tst_CREATE_TEST_CASE(tree_remove_node, tree_remove_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .removal_path              = range_create_static(u32, 10, { 41 }),
        .expect_removal            = true,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 40, 44, 45, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0,  3,  0,  0,  0,  1,  0 }),
)
tst_CREATE_TEST_CASE(tree_remove_bad_path, tree_remove_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
        .removal_path              = range_create_static(u32, 10, { 41, 42, 44 }),
        .expect_removal            = false,
        .tree_end_state_contents   = range_create_static(u32, 10,   { 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 }),
        .tree_end_state_children   = range_create_static(size_t, 10, { 0,  2,  1,  0,  3,  0,  0,  0,  1,  0 }),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_foreach_element,
        {

            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;
            range(u32, 10) path_to_iterate;

            void (*apply_f)(void *element, void *args);
            void *additional_args;
            size_t additional_args_size;

            range(u32, 10)    tree_end_state_contents;
            range(size_t, 10) tree_end_state_children;
            void *end_state_additional_args;
        },
        {
            ttree tree = {};

            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->path_to_iterate), &test_comparator_u32);

            ttree_foreach_element(path, data->apply_f, data->additional_args);

            for (size_t i = 0 ; i < tree.tree_contents.r->length ; i++) {
                tst_assert_equal_ext(data->tree_end_state_contents.data[i], *(u32 *) range_at(tree.tree_contents, i), "value of %d", "at index %d", i);
                tst_assert_equal_ext(data->tree_end_state_children.data[i], tree.tree_children->data[i], "value of %d", "at index %d", i);
            }

            tst_assert_memory_equal(data->additional_args, data->end_state_additional_args, data->additional_args_size, "passed arguments are not what was expected");

            ttree_path_destroy(make_system_allocator(), &path);
        }
)

static void test_apply_sum(void *element, void *accumulator) {
    u32 number = *(u32 *) element;
    u32 *sum = (u32 *) accumulator;

    *sum += number;
}

tst_CREATE_TEST_CASE(tree_foreach_element_sum_whole, tree_foreach_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, {}),
        .apply_f = &test_apply_sum,
        .additional_args = (void *) &(u32) { 0u },
        .additional_args_size = sizeof(u32),
        .tree_end_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(u32) { 42u },
)
tst_CREATE_TEST_CASE(tree_foreach_element_sum_part, tree_foreach_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, { 10, 5 }),
        .apply_f = &test_apply_sum,
        .additional_args = (void *) &(u32) { 0u },
        .additional_args_size = sizeof(u32),
        .tree_end_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(u32) { 9u },
)
tst_CREATE_TEST_CASE(tree_foreach_element_sum_nothing, tree_foreach_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, { 10, 5, 99 }),
        .apply_f = &test_apply_sum,
        .additional_args = (void *) &(u32) { 0u },
        .additional_args_size = sizeof(u32),
        .tree_end_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(u32) { 0u },
)

static void test_apply_increment(void *element, void *accumulator) {
    u32 *number = (u32 *) element;

    *number += 1u;
}

tst_CREATE_TEST_CASE(tree_foreach_element_incr_whole, tree_foreach_element,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, {  }),
        .apply_f = &test_apply_increment,
        .additional_args = (void *) &(u32) { 0u },
        .additional_args_size = sizeof(u32),
        .tree_end_state_contents = range_create_static(u32, 10,   { 11, 6, 5, 10, 2, 7, 4, 2, 3, 2 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(u32) { 0u },
)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_foreach_path,
        {

            range(u32, 10)    tree_start_state_contents;
            range(size_t, 10) tree_start_state_children;
            range(u32, 10) path_to_iterate;

            void (*apply_f)(ttree_path *element, void *args);
            void *additional_args;
            size_t additional_args_size;

            range(u32, 10)    tree_end_state_contents;
            range(size_t, 10) tree_end_state_children;
            void *end_state_additional_args;
        },
        {
            ttree tree = {};

            bytewise_copy(&tree.tree_contents, &range_to_any(&data->tree_start_state_contents), sizeof(tree.tree_contents));
            tree.tree_children = (void *) &data->tree_start_state_children;

            ttree_path *path = ttree_get_path_absolute(make_system_allocator(), &tree, range_to_any(&data->path_to_iterate), &test_comparator_u32);

            ttree_foreach_path(make_system_allocator(), path, data->apply_f, data->additional_args);

            for (size_t i = 0 ; i < tree.tree_contents.r->length ; i++) {
                tst_assert_equal_ext(data->tree_end_state_contents.data[i], *(u32 *) range_at(tree.tree_contents, i), "value of %d", "at index %d", i);
                tst_assert_equal_ext(data->tree_end_state_children.data[i], tree.tree_children->data[i], "value of %d", "at index %d", i);
            }

            tst_assert_memory_equal(data->additional_args, data->end_state_additional_args, data->additional_args_size, "passed arguments are not what was expected");

            ttree_path_destroy(make_system_allocator(), &path);
        }
)

static void test_apply_sum_on_path(ttree_path *path, void *additional_args)
{
    u32 *sum = (u32 *) additional_args;

    *sum += *(u32 *) ttree_path_content(path);
}

tst_CREATE_TEST_CASE(tree_foreach_path_sum_whole, tree_foreach_path,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, {}),
        .apply_f = &test_apply_sum_on_path,
        .additional_args = (void *) &(u32) { 0u },
        .additional_args_size = sizeof(u32),
        .tree_end_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(u32) { 42u },
)

typedef range(size_t, 10) test_depth_range;

static void test_apply_record_depth_on_path(ttree_path *path, void *additional_args)
{
    test_depth_range *depths = (test_depth_range *) additional_args;

    range_insert_value(range_to_any(depths), depths->length, &path->tokens_indexes->length);
}

tst_CREATE_TEST_CASE(tree_foreach_path_record_depth, tree_foreach_path,
        .tree_start_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_start_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .path_to_iterate = range_create_static(size_t, 10, {}),
        .apply_f = &test_apply_record_depth_on_path,
        .additional_args = (void *) &(test_depth_range) range_create_static(size_t, 10, {}),
        .additional_args_size = sizeof(test_depth_range),
        .tree_end_state_contents = range_create_static(u32, 10,   { 10, 5, 4, 9, 1, 6, 3, 1, 2, 1 }),
        .tree_end_state_children = range_create_static(size_t, 10, { 2, 1, 0, 4, 0, 0, 0, 0, 1, 0 }),
        .end_state_additional_args = (void *) &(test_depth_range) range_create_static(size_t, 10, { 1, 2, 3, 1, 2, 2, 2, 2, 1, 2 }),
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

    tst_run_test_case(tree_search_relative_valid_and_valid);
    tst_run_test_case(tree_search_relative_invalid_and_valid);
    tst_run_test_case(tree_search_relative_empty_and_valid);
    tst_run_test_case(tree_search_relative_toolong_and_valid);
    tst_run_test_case(tree_search_relative_valid_and_invalid);
    tst_run_test_case(tree_search_relative_invalid_and_invalid);
    tst_run_test_case(tree_search_relative_toolong_and_invalid);
    tst_run_test_case(tree_search_relative_empty_and_invalid);
    tst_run_test_case(tree_search_relative_empty_and_toolong);

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

    tst_run_test_case(tree_foreach_element_sum_whole);
    tst_run_test_case(tree_foreach_element_sum_part);
    tst_run_test_case(tree_foreach_element_sum_nothing);
    tst_run_test_case(tree_foreach_element_incr_whole);

    tst_run_test_case(tree_foreach_path_sum_whole);
    tst_run_test_case(tree_foreach_path_record_depth);
}

#endif
