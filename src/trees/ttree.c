
// #include <stdlib.h>

#include <ustd/common.h>
#include <ustd/tree.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
typedef struct ttree {
    ///
    size_t nb_nodes;
    ///
    void *data;
} ttree;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param tree
 * @param node
 * @param node_comparator
 * @return size_t
 */
static size_t find_direct_subtree_index(ttree *tree, const void *node, i32 (*node_comparator)(const void *node_1, const void *node_2));

/**
 * @brief
 *
 * @param tree
 * @param node_path
 * @param node_path_length
 * @param node_comparator
 * @return size_t
 */
static subttree find_subtree(ttree *tree, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2));

/**
 * @brief
 *
 * @param subtree
 * @param apply_f
 * @param additional_args
 */
static void foreach_parent_of_subtree_up_down(subttree subtree, node_mutation_function apply_f, void *additional_args);

/**
 * @brief
 *
 * @param subtree
 * @param apply_f
 * @param additional_args
 */
static void foreach_child_of_subtree_up_down(subttree subtree, node_mutation_function apply_f, void *additional_args);

/**
 * @brief
 *
 * @param subtree
 * @param apply_f
 * @param additional_args
 */
static void foreach_child_of_subtree_down_up(subttree subtree, node_mutation_function apply_f, void *additional_args);

/**
 * @brief
 *
 * @param subtree
 * @param apply_f
 * @param additional_args
 */
static void foreach_parent_of_subtree_down_up(subttree subtree, node_mutation_function apply_f, void *additional_args);

/**
 * @brief
 *
 * @param tree
 * @param additional_args
 */
static void increment_tree_nb_nodes(void **data, void *additional_args);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
ttree * ttree_create(allocator alloc, size_t nb_nodes)
{
    ttree *new_tree = { nullptr };

    // checking vital preconditions
    if (nb_nodes == 0u) {
        return nullptr;
    }

    // allocating memory for the tree struct
    new_tree = alloc.malloc(alloc, sizeof(*new_tree) + (sizeof(*new_tree) * (nb_nodes + 1u)));

    if (!new_tree) {
        return nullptr;
    }

    new_tree->nb_nodes = 0u;

    return new_tree;
}

// -------------------------------------------------------------------------------------------------
subttree ttree_get_subtree(ttree *tree, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2))
{
    subttree subtree  = { .parent_tree = tree, .pos = 0u, .parents = range_static_create(TTREE_MAX_DEPTH, void **) };
    return ttree_get_subtree_subtree(subtree, node_path, node_path_length, node_comparator);
}

// -------------------------------------------------------------------------------------------------
subttree ttree_get_subtree_subtree(subttree target, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2))
{
    subttree subtree = target;
    size_t found_index = { 0u };

    if (node_path_length == 0u) {
        subtree.pos = 0u;
        return subtree;
    }

    return find_subtree(subtree.parent_tree, node_path, node_path_length, node_comparator);
}

// -------------------------------------------------------------------------------------------------
void * ttree_get_subtree_content(subttree target)
{
    return target.parent_tree[target.pos].data;
}

// -------------------------------------------------------------------------------------------------
size_t ttree_get_subtree_node_count(subttree target)
{
    return target.parent_tree[target.pos].nb_nodes;
}

// -------------------------------------------------------------------------------------------------
i32 ttree_insert(subttree target, void *node)
{
    size_t insertion_pos = { 0u };
    i32 nb_node_increment = { 0 };

    if (!target.parent_tree || (target.pos > target.parent_tree->nb_nodes)) {
        return TTREE_INCORRECT_PATH;
    }

    // (step 0 : calculate insertion position to be after the target subtree)
    insertion_pos = (target.pos) + (target.parent_tree[target.pos].nb_nodes + 1u);

    // step 1 : shift all memory after the subtree right one unit to make room for the new node
    for (size_t i = target.parent_tree->nb_nodes ; i >= insertion_pos ; i--) {
        bytewise_copy(target.parent_tree + i + 1u, target.parent_tree + i, sizeof(*target.parent_tree));
    }

    // step 2 : insert the new node in the free space
    target.parent_tree[insertion_pos].data = node;
    target.parent_tree[insertion_pos].nb_nodes = 0u;

    // step 3 : increment the children count of all parents of the target subtree
    nb_node_increment = 1;
    target.parent_tree->nb_nodes += 1u;
    foreach_parent_of_subtree_up_down(target, &increment_tree_nb_nodes, &nb_node_increment);

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
i32 ttree_remove(subttree target)
{
    ttree *copy_to   = { nullptr };
    ttree *copy_from = { nullptr };
    size_t copy_size      = { 0u };
    i32 nb_node_increment = { 0u };

    if (!target.parent_tree || (target.pos > target.parent_tree->nb_nodes)) {
        return TTREE_INCORRECT_PATH;
    }

    if (target.pos == 0u) {
        target.parent_tree->nb_nodes = 0u;
        return TTREE_NO_MISHAP;
    }

    // step 1 : calculate the difference in number of nodes for the parents
    nb_node_increment = -1 * ((i32) (target.parent_tree[target.pos].nb_nodes + 1u));

    // determine the copy parameters to overwrite part of the tree
    copy_to = target.parent_tree + target.pos;
    copy_from = target.parent_tree + target.pos + target.parent_tree[target.pos].nb_nodes + 1u;
    copy_size = (target.parent_tree->nb_nodes + 1u) - target.pos - (target.parent_tree[target.pos].nb_nodes + 1u);

    // actualize the parents' number of children
    target.parent_tree->nb_nodes = (size_t) ((i32) target.parent_tree->nb_nodes + nb_node_increment);
    foreach_parent_of_subtree_up_down(target, &increment_tree_nb_nodes, &nb_node_increment);

    // overwrite the subtree with what follows it
    bytewise_copy(copy_to, copy_from, copy_size * sizeof(*copy_to));

    return TTREE_NO_MISHAP;
}

// -------------------------------------------------------------------------------------------------
void ttree_foreach(subttree target, void (*apply_f)(void **node, void *additional_args), void *additional_args, u32 config_flags)
{
    if (config_flags & TTREE_FOREACH_FLAG_DIRECTION_DOWN_UP) {
        if (config_flags & TTREE_FOREACH_FLAG_INCLUDE_CHILDREN) foreach_child_of_subtree_down_up(target, apply_f, &additional_args);
        if (config_flags & TTREE_FOREACH_FLAG_INCLUDE_PARENTS) foreach_parent_of_subtree_down_up(target, apply_f, &additional_args);
    } else if (config_flags & TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN) {
        if (config_flags & TTREE_FOREACH_FLAG_INCLUDE_PARENTS) foreach_parent_of_subtree_up_down(target, apply_f, &additional_args);
        if (config_flags & TTREE_FOREACH_FLAG_INCLUDE_CHILDREN) foreach_child_of_subtree_up_down(target, apply_f, &additional_args);
    }
}

// -------------------------------------------------------------------------------------------------
void ttree_destroy(allocator alloc, ttree **tree)
{
    alloc.free(alloc, *tree);
    *tree = nullptr;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static size_t find_direct_subtree_index(ttree *tree, const void *node, i32 (*node_comparator)(const void *node_1, const void *node_2))
{
    size_t pos = { 0u };
    bool found = { false };

    pos = 1u;
    while ((pos <= tree->nb_nodes) && !found) {
        found = (node_comparator(tree[pos].data, node) == 0);
        pos += (!found) * (tree[pos].nb_nodes + 1u);
    }

    if (!found) {
        return tree->nb_nodes + 1u;
    }
    return pos;
}

// -------------------------------------------------------------------------------------------------
static subttree find_subtree(ttree *tree, const void *node_path[], size_t node_path_length, i32 (*node_comparator)(const void *node_1, const void *node_2))
{
    subttree out_subtree = { .parent_tree = tree, .pos = 0, .parents = range_static_create(TTREE_MAX_DEPTH, void **) };
    bool path_is_correct = { true };
    size_t pos_path = { 0u };
    size_t pos_tree = { 0u };
    void **parent_data = { };

    while ((pos_path < node_path_length) && path_is_correct) {
        pos_tree = find_direct_subtree_index(tree + out_subtree.pos, node_path[pos_path], node_comparator);
        path_is_correct = (pos_tree < tree[out_subtree.pos].nb_nodes + 1u);

        out_subtree.pos += pos_tree;
        pos_path += path_is_correct;

        if ((pos_path < node_path_length) && path_is_correct) {
            parent_data = &(tree + out_subtree.pos)->data;
            range_insert_value((range *) &out_subtree.parents, out_subtree.parents.length, &parent_data);
        }
    }

    if (!path_is_correct) {
        out_subtree = (subttree) { .parent_tree = NULL, .pos = tree->nb_nodes + 1u, .parents = range_static_create(TTREE_MAX_DEPTH, void **) };
    }
    return out_subtree;
}

// -------------------------------------------------------------------------------------------------
static void foreach_parent_of_subtree_up_down(subttree subtree, node_mutation_function apply_f, void *additional_args)
{
    for (size_t i = 0 ; i < subtree.parents.length ; i++) {
        apply_f(subtree.parents.data[i], additional_args);
    }

    if (subtree.pos != 0u) {
        apply_f(&subtree.parent_tree[subtree.pos].data, additional_args);
    }
}

// -------------------------------------------------------------------------------------------------
static void foreach_parent_of_subtree_down_up(subttree subtree, node_mutation_function apply_f, void *additional_args)
{
    for (i64 i = (i64) subtree.parents.length - 1 ; i >= 0 ; i--) {
        apply_f(&subtree.parent_tree[i].data, additional_args);
    }

    if (subtree.pos != 0u) {
        apply_f(&subtree.parent_tree[subtree.pos].data, additional_args);
    }
}

// -------------------------------------------------------------------------------------------------
static void foreach_child_of_subtree_up_down(subttree subtree, node_mutation_function apply_f, void *additional_args)
{
    const size_t index_end = { subtree.pos + subtree.parent_tree[subtree.pos].nb_nodes };

    for (size_t i = subtree.pos + 1u ; i <= index_end ; i++) {
        apply_f(&subtree.parent_tree[i].data, additional_args);
    }
}

// -------------------------------------------------------------------------------------------------
static void foreach_child_of_subtree_down_up(subttree subtree, node_mutation_function apply_f, void *additional_args)
{
    const i64 index_end = { (i64) subtree.pos };

    for (i64 i = (i64) (subtree.pos + subtree.parent_tree[subtree.pos].nb_nodes) ; i > (i64) index_end ; i--) {
        apply_f(&subtree.parent_tree[i].data, additional_args);
    }
}

// -------------------------------------------------------------------------------------------------
static void increment_tree_nb_nodes(void **data, void *additional_args)
{
    ttree *target_tree = (ttree *) container_of(data, data, ttree);
    target_tree->nb_nodes = (size_t) ((i32) target_tree->nb_nodes + *((i32 *) additional_args));
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

// compare two pointers by value
static i32 ttree_test_comparator(const void *n1, const void *n2) {
    size_t val1 = (size_t) n1;
    size_t val2 = (size_t) n2;

    return (val1 > val2) - (val1 < val2);
}

static inline void ttree_test_improvise_tree(u32 (*tree_lengths)[10u], void *(*tree_contents)[10u], ttree (*out_tree)[11u]) {
    // root
    (*out_tree)[0u].nb_nodes = 10u;
    (*out_tree)[0u].data = NULL;

    // rest of the tree
    for (size_t i = 0u ; i < 10u ; i++) {
        (*out_tree)[i+1].nb_nodes = (*tree_lengths)[i];
        (*out_tree)[i+1].data = (void *) (*tree_contents)[i];
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_creation,
        {
            size_t nb_nodes;

            i32 is_failing;
        },
        {
            ttree *tree = ttree_create(make_system_allocator(), data->nb_nodes);

            tst_assert_equal(data->is_failing, (tree == NULL), "nullness test of %d");
            if (tree) {
                ttree_destroy(make_system_allocator(), &tree);
            }
            tst_assert_equal(NULL, tree, "tree address : %#010x");
        }
)

tst_CREATE_TEST_CASE(tree_creation_success, tree_creation,
        .nb_nodes = 12u,
        .is_failing = 0u
)

tst_CREATE_TEST_CASE(tree_creation_number_zero, tree_creation,
        .nb_nodes = 0u,
        .is_failing = 1u
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_find,
        {
            void * tree_contents[10u];
            u32 tree_lengths[10u];

            const void * path[10u];
            size_t path_length;

            u32 expect_found;
            size_t expected_position;
            range_static(TTREE_MAX_DEPTH, void *) expected_parents;
        },
        {
            ttree tree[11u] = { 0u };
            subttree subtree = { 0u };

            ttree_test_improvise_tree(&data->tree_lengths, &data->tree_contents, &tree);

            subtree = ttree_get_subtree(tree, data->path, data->path_length, &ttree_test_comparator);

            if (data->expect_found) {
                tst_assert(subtree.parent_tree != NULL, "tree not found !");
                tst_assert_equal(data->expected_position, subtree.pos, "subtree index of %d");
                if (data->path_length > 0u) tst_assert_equal(data->path[data->path_length-1u], ttree_get_subtree_content(subtree), "node content of %d");
            } else {
                tst_assert(subtree.parent_tree == NULL, "tree found !");
                tst_assert_equal(data->expected_position, subtree.pos, "subtree index of %d");
            }

            tst_assert_equal(data->expected_parents.length, subtree.parents.length, "parent stack of length %d");
            for (size_t i = 0 ; i < data->expected_parents.length ; i++) {
                tst_assert(range_val(&data->expected_parents, i, void *) == *range_val(&subtree.parents, i, void **),
                        "parent data at index %i mismatch : expected %d, got %d", i, range_val(&data->expected_parents, i, void *), *range_val(&subtree.parents, i, void **));
            }
        }
)

tst_CREATE_TEST_CASE(tree_find_leaf, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 1, (void*) 4, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 2u,
        .expect_found = 1u,
        .expected_position = 2u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *, (void *) 1),
)

tst_CREATE_TEST_CASE(tree_find_node, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 3, (void*) 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 2u,
        .expect_found = 1u,
        .expected_position = 8u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *, (void *) 3),
)

tst_CREATE_TEST_CASE(tree_find_node_at_end, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_found = 1u,
        .expected_position = 10u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *),
)

tst_CREATE_TEST_CASE(tree_find_whole_tree, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 0u,
        .expect_found = 1u,
        .expected_position = 0u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *),
)

tst_CREATE_TEST_CASE(tree_find_bad_path, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 1, (void *) 9, (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 3u,
        .expect_found = 0u,
        .expected_position = 11u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *),
)

tst_CREATE_TEST_CASE(tree_find_bad_path_began_right, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 3, (void *) 8, (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 3u,
        .expect_found = 0u,
        .expected_position = 11u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *),
)

tst_CREATE_TEST_CASE(tree_find_path_too_long, tree_find,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .path = { (void *) 1, (void *) 4, (void *) 11, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 3u,
        .expect_found = 0u,
        .expected_position = 11u,
        .expected_parents = range_static_create(TTREE_MAX_DEPTH, void *),
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_insert,
        {
            void * tree_contents[10u];
            u32 tree_lengths[10u];
            size_t tree_nb_nodes;

            void * expected_tree_contents[10u];
            u32 expected_tree_lengths[10u];
            size_t expected_tree_nb_nodes;

            void *added_value;

            const void * path[10u];
            size_t path_length;

            i32 expect_addition;

        },
        {
            ttree tree[11u] = { 0u };
            ttree expected_tree[11u] = { 0u };
            subttree subtree = { 0u };
            i32 insertion_result = 0;

            ttree_test_improvise_tree(&data->tree_lengths, &data->tree_contents, &tree);
            tree[0u].nb_nodes = data->tree_nb_nodes;
            ttree_test_improvise_tree(&data->expected_tree_lengths, &data->expected_tree_contents, &expected_tree);
            expected_tree[0u].nb_nodes = data->expected_tree_nb_nodes;

            subtree = ttree_get_subtree(tree, data->path, data->path_length, &ttree_test_comparator);

            insertion_result = ttree_insert(subtree, data->added_value);
            tst_assert_equal(data->expect_addition, (insertion_result == 0), "insertion success of %d");

            for (size_t i = 0u ; i < expected_tree[0u].nb_nodes ; i++) {
                tst_assert(tree[i].data == expected_tree[i].data, "data at index %d mismatch : expected %d, got %d", i, expected_tree[i].data, tree[i].data);
                tst_assert(tree[i].nb_nodes == expected_tree[i].nb_nodes, "nb_nodes at index %d mismatch : expected %d, got %d", i, expected_tree[i].nb_nodes, tree[i].nb_nodes);
            }
        }
)

tst_CREATE_TEST_CASE(tree_insert_at_root, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .expected_tree_nb_nodes = 10u,

        .added_value = (void *) 10,
        .path = { NULL },
        .path_length = 0u,
        .expect_addition = 1u
)

tst_CREATE_TEST_CASE(tree_insert_at_leaf, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },
        .expected_tree_nb_nodes = 10u,

        .added_value = (void *) 10,
        .path = { (void *) 3, (void *) 8, (void *) 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 3u,
        .expect_addition = 1u
)

tst_CREATE_TEST_CASE(tree_insert_in_middle, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 10, (void *) 3, (void *) 8, (void *) 9 },
        .expected_tree_lengths  = {          1,          0,          4,          0,          0,          0,           0,          2,          1,          0 },
        .expected_tree_nb_nodes = 10u,

        .added_value = (void *) 10,
        .path = { (void *) 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_addition = 1u
)

tst_CREATE_TEST_CASE(tree_insert_fail, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .expected_tree_nb_nodes = 9u,

        .added_value = (void *) 10,
        .path = { (void *) 2, (void *) 9, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 2u,
        .expect_addition = 0u
)

tst_CREATE_TEST_CASE(tree_insert_in_empty, tree_insert,
        .tree_contents = {       NULL,       NULL,       NULL,       NULL,       NULL,       NULL,       NULL,       NULL,       NULL,      NULL },
        .tree_lengths  = {          0,          0,          0,          0,          0,          0,          0,          0,          0,         0 },
        .tree_nb_nodes = 0u,

        .expected_tree_contents = { (void *) 1,       NULL,       NULL,       NULL,       NULL,        NULL,        NULL,       NULL,       NULL,       NULL },
        .expected_tree_lengths  = {          0,          0,          0,          0,          0,           0,           0,          0,          0,          0 },
        .expected_tree_nb_nodes = 1u,

        .added_value = (void *) 1,
        .path = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 0u,
        .expect_addition = 1u
)

tst_CREATE_TEST_CASE(tree_insert_same_value_root, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 1 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .expected_tree_nb_nodes = 10u,

        .added_value = (void *) 1,
        .path = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 0u,
        .expect_addition = 1u
)

tst_CREATE_TEST_CASE(tree_insert_same_unrelated_part, tree_insert,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          1,          0 },
        .tree_nb_nodes = 9u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 1,(void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9 },
        .expected_tree_lengths  = {          1,          0,          4,          1,          0,         0,          0,          2,          1,          1 },
        .expected_tree_nb_nodes = 10u,

        .added_value = (void *) 1,
        .path = { (void *) 2, (void *) 5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 2u,
        .expect_addition = 1u
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(tree_remove,
        {
            void * tree_contents[10u];
            u32 tree_lengths[10u];
            size_t tree_nb_nodes;

            void * expected_tree_contents[10u];
            u32 expected_tree_lengths[10u];
            size_t expected_tree_nb_nodes;

            const void * path[10u];
            size_t path_length;

            i32 expect_removal;
        },
        {
            ttree tree[11u] = { 0u };
            ttree expected_tree[11u] = { 0u };
            i32 removal_status = 0;
            subttree subtree = { 0u };

            ttree_test_improvise_tree(&data->tree_lengths, &data->tree_contents, &tree);
            tree[0u].nb_nodes = data->tree_nb_nodes;
            ttree_test_improvise_tree(&data->expected_tree_lengths, &data->expected_tree_contents, &expected_tree);
            expected_tree[0u].nb_nodes = data->expected_tree_nb_nodes;

            subtree = ttree_get_subtree(tree, data->path, data->path_length, &ttree_test_comparator);
            removal_status = ttree_remove(subtree);

            tst_assert_equal(data->expect_removal, (removal_status == 0), "removal success of %d");

            for (size_t i = 0u ; i < (data->expected_tree_nb_nodes + 1) ; i++) {
                tst_assert(tree[i].data == expected_tree[i].data, "data at index %d mismatch : expected %d, got %d", i, expected_tree[i].data, tree[i].data);

                tst_assert(tree[i].nb_nodes == expected_tree[i].nb_nodes, "nb_nodes at index %d mismatch : expected %d, got %d", i, expected_tree[i].nb_nodes, tree[i].nb_nodes);
            }
        }
)

tst_CREATE_TEST_CASE(tree_remove_at_root, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 10, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9 },
        .tree_lengths  = {          1,          0,           0,          3,          0,          0,          0,          2,          1,          0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .expected_tree_nb_nodes = 9u,

        .path = { (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_at_start_root, tree_remove,
        .tree_contents = { (void *) 10, (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9 },
        .tree_lengths  = {           0,          1,          0,          3,          0,          0,          0,          2,          1,          0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .expected_tree_nb_nodes = 9u,

        .path = { (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_at_end_root, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9,       NULL },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,          0 },
        .expected_tree_nb_nodes = 9u,

        .path = { (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_a_subtree, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 3, (void *) 8, (void *) 9, (void *) 10, NULL, NULL, NULL, NULL },
        .expected_tree_lengths  = {          1,          0,          2,          1,          0,           0,    0,    0,    0,    0 },
        .expected_tree_nb_nodes = 6u,

        .path = { (void *) 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_whole_tree, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .expected_tree_lengths  = {    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
        .expected_tree_nb_nodes = 0u,

        .path = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 0u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_almost_whole_tree, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          8,          0,          0,          0,          0,          0,          0,          0,          0,           0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 10, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .expected_tree_lengths  = {           0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
        .expected_tree_nb_nodes = 1u,

        .path = { (void *) 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,
        .expect_removal = 1u,
)

tst_CREATE_TEST_CASE(tree_remove_fail, tree_remove,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .tree_nb_nodes = 10u,

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          2,          1,          0,           0 },
        .expected_tree_nb_nodes = 10u,

        .path = { (void *) 1, (void *) 8, (void *) 4, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 3u,
        .expect_removal = 0u,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(tree_foreach,
        {
            void * tree_contents[10u];
            u32 tree_lengths[10u];

            void * expected_tree_contents[10u];
            u32 expected_tree_lengths[10u];

            void (*apply_f)(void **node, void *additional_args);

            const void * path[10u];
            size_t path_length;

            u32 tree_division;
        },
        {
            ttree tree[11u] = { 0u };
            ttree expected_tree[11u] = { 0u };
            subttree subtree = { 0u };
            i32 insertion_result = 0;

            ttree_test_improvise_tree(&data->tree_lengths, &data->tree_contents, &tree);
            ttree_test_improvise_tree(&data->expected_tree_lengths, &data->expected_tree_contents, &expected_tree);

            subtree = ttree_get_subtree(tree, data->path, data->path_length, &ttree_test_comparator);
            ttree_foreach(subtree, data->apply_f, (void *) ((size_t) 5), data->tree_division);

            for (size_t i = 0u ; i < 10u ; i++) {
                tst_assert(tree[i].data == expected_tree[i].data, "data at index %d mismatch : expected %d, got %d", i, expected_tree[i].data, tree[i].data);

                tst_assert(tree[i].nb_nodes == expected_tree[i].nb_nodes, "nb_nodes at index %d mismatch : expected %d, got %d", i, expected_tree[i].nb_nodes, tree[i].nb_nodes);
            }
        }
)

static void tree_test_incr_apply(void **node, void *additional_args) {
    size_t *data = (size_t *) node;
    *data += 1u;
}

tst_CREATE_TEST_CASE(tree_foreach_all_tree_incr, tree_foreach,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .expected_tree_contents = { (void *) 2, (void *) 5, (void *) 3, (void *) 6, (void *) 7, (void *) 8, (void *) 4, (void *) 9, (void *) 10, (void *) 11 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,           1,           0 },

        .apply_f = &tree_test_incr_apply,

        .path = { NULL },
        .path_length = 0u,

        .tree_division = TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN | TTREE_FOREACH_FLAG_INCLUDE_CHILDREN,
)

static void tree_test_incr_with_args_apply(void **node, void *additional_args) {
    size_t *data = (size_t *) node;
    *data += *((size_t *) additional_args);
}

tst_CREATE_TEST_CASE(tree_foreach_all_tree_incr_with_args, tree_foreach,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .expected_tree_contents = { (void *) 6, (void *) 9, (void *) 7, (void *) 10, (void *) 11, (void *) 12, (void *) 8, (void *) 13, (void *) 14, (void *) 15 },
        .expected_tree_lengths  = {          1,          0,          3,           0,           0,           0,          3,           2,           1,           0 },

        .apply_f = &tree_test_incr_with_args_apply,

        .path = { NULL },
        .path_length = 0u,

        .tree_division = TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN | TTREE_FOREACH_FLAG_INCLUDE_CHILDREN,
)

tst_CREATE_TEST_CASE(tree_foreach_subtree_children_incr, tree_foreach,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 6, (void *) 7, (void *) 8, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .apply_f = &tree_test_incr_apply,

        .path = { (void *) 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,

        .tree_division = TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN | TTREE_FOREACH_FLAG_INCLUDE_CHILDREN,
)

tst_CREATE_TEST_CASE(tree_foreach_multi_level_subtree_children_incr, tree_foreach,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 9, (void *) 10, (void *) 11 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,           1,           0 },

        .apply_f = &tree_test_incr_apply,

        .path = { (void *) 3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 1u,

        .tree_division = TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN | TTREE_FOREACH_FLAG_INCLUDE_CHILDREN,
)

tst_CREATE_TEST_CASE(tree_foreach_parent_incr, tree_foreach,
        .tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 3, (void *) 8, (void *) 9, (void *) 10 },
        .tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .expected_tree_contents = { (void *) 1, (void *) 4, (void *) 2, (void *) 5, (void *) 6, (void *) 7, (void *) 4, (void *) 9, (void *) 9, (void *) 10 },
        .expected_tree_lengths  = {          1,          0,          3,          0,          0,          0,          3,          2,          1,           0 },

        .apply_f = &tree_test_incr_apply,

        .path = { (void *) 3, (void *) 8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        .path_length = 2u,

        .tree_division = TTREE_FOREACH_FLAG_DIRECTION_UP_DOWN | TTREE_FOREACH_FLAG_INCLUDE_PARENTS,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
void ttree_execute_unittests(void)
{
    tst_run_test_case(tree_creation_success);
    tst_run_test_case(tree_creation_number_zero);

    tst_run_test_case(tree_find_leaf);
    tst_run_test_case(tree_find_node);
    tst_run_test_case(tree_find_node_at_end);
    tst_run_test_case(tree_find_whole_tree);
    tst_run_test_case(tree_find_bad_path);
    tst_run_test_case(tree_find_bad_path_began_right);
    tst_run_test_case(tree_find_path_too_long);

    tst_run_test_case(tree_insert_at_root);
    tst_run_test_case(tree_insert_at_leaf);
    tst_run_test_case(tree_insert_in_middle);
    tst_run_test_case(tree_insert_fail);
    tst_run_test_case(tree_insert_in_empty);
    tst_run_test_case(tree_insert_same_value_root);
    tst_run_test_case(tree_insert_same_unrelated_part);

    tst_run_test_case(tree_remove_at_root);
    tst_run_test_case(tree_remove_at_start_root);
    tst_run_test_case(tree_remove_at_end_root);
    tst_run_test_case(tree_remove_a_subtree);
    tst_run_test_case(tree_remove_whole_tree);
    tst_run_test_case(tree_remove_almost_whole_tree);
    tst_run_test_case(tree_remove_fail);

    tst_run_test_case(tree_foreach_all_tree_incr);
    tst_run_test_case(tree_foreach_all_tree_incr_with_args);
    tst_run_test_case(tree_foreach_subtree_children_incr);
    tst_run_test_case(tree_foreach_multi_level_subtree_children_incr);
    tst_run_test_case(tree_foreach_parent_incr);
}

#endif
