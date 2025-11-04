
#include <ustd/path.h>
#include <ustd_impl/array_impl.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct path_impl {
    size_t last_delimiter;
    char delimiter;

    size_t length;
    size_t capacity;
    u32 stride;
    byte data[];
};

// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @param size
 * @return PATH
 */
static PATH path_create(struct allocator alloc, char delimiter, size_t size);

/**
 * @brief
 *
 * @param target
 */
static void path_update_last_delim(struct path_impl *target);

/**
 * @brief
 *
 * @param path
 * @return struct path_impl*
 */
static struct path_impl *path_impl_of(PATH path);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

PATH path_from_cstring(struct allocator alloc, const char *path_cstring, char delimiter, size_t limit)
{
    PATH new_path = nullptr;
    size_t path_length = 0;

    if (!path_cstring) {
        return nullptr;
    }

    path_length = c_string_length(path_cstring, limit, true);

    if (path_length == 0) {
        return nullptr;
    }

    new_path = path_create(alloc, delimiter, path_length);
    path_append(new_path, path_cstring);

    return new_path;
}

// -----------------------------------------------------------------------------

void path_destroy(struct allocator alloc, PATH *path)
{
    struct path_impl *target = nullptr;

    if (!path || !*path) {
        return;
    }

    target = path_impl_of(*path);

    alloc.free(alloc, target);
    *path = nullptr;
}

// -----------------------------------------------------------------------------

void path_clear(PATH path)
{
    struct path_impl *target = nullptr;

    if (!path) {
        return;
    }

    target = path_impl_of(path);

    target->length = 0;
    path_update_last_delim(target);
}

// -----------------------------------------------------------------------------

void path_ensure_capacity(struct allocator alloc, PATH *path, size_t additional_capacity)
{
    size_t size_needed = 0;
    struct path_impl *target = nullptr;

    PATH new_path = nullptr;

    if (!path || !*path) {
        return;
    }

    size_needed = path_length(*path) + additional_capacity;

    if (size_needed < path_capacity(*path)) {
        return;
    }

    target = path_impl_of(*path);
    new_path = path_create(alloc, target->delimiter, size_needed);

    path_append(new_path, *path);

    path_destroy(alloc, path);
    *path = new_path;
}

// -----------------------------------------------------------------------------

void path_up(PATH path)
{
    struct path_impl *target = nullptr;

    if (!path) {
        return;
    }

    target = path_impl_of(path);

    target->length = target->last_delimiter + 1;
    target->data[target->last_delimiter] = '\0';

    path_update_last_delim(target);
}

// -----------------------------------------------------------------------------

void path_append(PATH path, const char *appended)
{
    struct path_impl *target = nullptr;
    size_t appended_len = 0;

    if (!path || !appended) {
        return;
    }

    target = path_impl_of(path);
    appended_len = c_string_length(appended, target->capacity, false);

    if ((appended_len == 0) || ((target->length + appended_len) > target->capacity)) {
        return;
    }

    if (target->length > 1) {
        // replace '\0' with a delimiter
        target->data[target->length-1] = (byte) target->delimiter;
    } else {
        // remove the trailing '\0'
        target->length = 0;
    }

    if (array_append_mem(path, appended, appended_len)) {
        array_push(path, &(char) { '\0' });
    }

    path_update_last_delim(target);
}

// -----------------------------------------------------------------------------

void path_prepend(PATH path, const char *prefix)
{
    struct path_impl *target = nullptr;
    size_t prepended_len = 0;

    if (!path || !prefix) {
        return;
    }

    target = path_impl_of(path);
    prepended_len = c_string_length(prefix, target->capacity, false);

    if ((prepended_len == 0) || ((prepended_len + target->length) > target->capacity)) {
        return;
    }

    if (target->length > 1) {
        // push a delimiter at the start
        array_insert_value(path, 0, &target->delimiter);
    }

    // TODO: less naive approach
    for (size_t i = 0 ; i < prepended_len ; i++) {
        array_insert_value(path, i, prefix + i);
    }

    path_update_last_delim(target);
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

static PATH path_create(struct allocator alloc, char delimiter, size_t size)
{
    struct path_impl *new_path = nullptr;

    if (size == 0) {
        return nullptr;
    }

    new_path = alloc.malloc(alloc, sizeof(*new_path) + (sizeof(char) * size));
    *new_path = (struct path_impl) { 0 };

    new_path->capacity = size;
    new_path->stride = sizeof(char);

    new_path->last_delimiter = 0;
    new_path->delimiter = delimiter;

    array_push(new_path->data, &(char) { '\0' });

    return (PATH) &(new_path->data);
}

// -----------------------------------------------------------------------------

static void path_update_last_delim(struct path_impl *target)
{
    bool found = false;
    found = array_find_back(&target->data, &character_compare, &target->delimiter, &target->last_delimiter);

    if (!found) {
        target->last_delimiter = 0;
    }
}

// -----------------------------------------------------------------------------

static struct path_impl *path_impl_of(PATH path)
{
    return CONTAINER_OF(path, struct path_impl, data);
}

#ifdef UNITTESTING

tst_CREATE_TEST_SCENARIO(path_create,
        {
            const char *path;
            char delim;

            bool must_fail;
            size_t expected_length;
            size_t expected_last_delim;
        },
        {
            struct path_impl *path_target = nullptr;
            PATH path = path_from_cstring(make_system_allocator(), data->path, data->delim, 2048);

            if (data->must_fail) {
                tst_assert(!path, "path was created");
            } else {
                path_target = path_impl_of(path);
                tst_assert_equal(data->expected_length, path_target->length, "length of %ld");
                tst_assert_equal(data->expected_last_delim, path_target->last_delimiter, "last delim at %ld");
                for (size_t i = 0 ; i < data->expected_length ; i++) {
                    tst_assert_equal_ext(data->path[i], path[i], "'%c'", "at index %ld", i);
                }
            }

            path_destroy(make_system_allocator(), &path);
        }
)

tst_CREATE_TEST_CASE(path_create_nominal, path_create,
        .path = "some/path/that/is/normal",
        .delim = '/',
        .expected_length = 25,
        .expected_last_delim = 17,
)
tst_CREATE_TEST_CASE(path_create_null, path_create,
        .path = nullptr,
        .delim = '/',
        .must_fail = true,
        .expected_length = 0,
        .expected_last_delim = 0,
)
tst_CREATE_TEST_CASE(path_create_empty, path_create,
        .path = "",
        .delim = '/',
        .expected_length = 1,
        .expected_last_delim = 0,
)
tst_CREATE_TEST_CASE(path_create_no_folder, path_create,
        .path = "imalone",
        .delim = '/',
        .expected_length = 8,
        .expected_last_delim = 0,
)

tst_CREATE_TEST_SCENARIO(path_up,
        {
            const char *path;
            char delim;

            size_t repeat_nb;

            const char *expected_path;
            size_t expected_length;
            size_t expected_last_delim;
        },
        {
            struct path_impl *path_target = nullptr;
            PATH path = path_from_cstring(make_system_allocator(), data->path, data->delim, 2048);

            for (size_t i = 0 ; i < data->repeat_nb ; i++) {
                path_up(path);
            }

            path_target = path_impl_of(path);
            tst_assert_equal(data->expected_length, path_target->length, "length of %ld");
            tst_assert_equal(data->expected_last_delim, path_target->last_delimiter, "last delim at %ld");
            for (size_t i = 0 ; i < data->expected_length ; i++) {
                tst_assert_equal_ext(data->expected_path[i], path[i], "'%c'", "at index %ld", i);
            }

            path_destroy(make_system_allocator(), &path);
        }
)
tst_CREATE_TEST_CASE(path_up_once, path_up,
        .path = "some/path/here",
        .delim = '/',
        .repeat_nb = 1,
        .expected_path = "some/path",
        .expected_length = 10,
        .expected_last_delim = 4,
)
tst_CREATE_TEST_CASE(path_up_all_the_way, path_up,
        .path = "some/path/here",
        .delim = '/',
        .repeat_nb = 3,
        .expected_path = "",
        .expected_length = 1,
        .expected_last_delim = 0,
)
tst_CREATE_TEST_CASE(path_up_beyond, path_up,
        .path = "some/path/here",
        .delim = '/',
        .repeat_nb = 100,
        .expected_path = "",
        .expected_length = 1,
        .expected_last_delim = 0,
)
tst_CREATE_TEST_CASE(path_up_absolute_beyond, path_up,
        .path = "/some/path/here",
        .delim = '/',
        .repeat_nb = 100,
        .expected_path = "",
        .expected_length = 1,
        .expected_last_delim = 0,
)

tst_CREATE_TEST_SCENARIO(path_append,
        {
            const char *path;
            char delim;
            const char *appended;

            const char *expected_path;
            size_t expected_length;
            size_t expected_last_delim;

        },
        {
            struct path_impl *path_target = nullptr;
            PATH path = path_from_cstring(make_system_allocator(), data->path, data->delim, 2048);

            path_ensure_capacity(make_system_allocator(), &path, data->expected_length);
            path_append(path, data->appended);

            path_target = path_impl_of(path);
            tst_assert_equal(data->expected_length, path_target->length, "length of %ld");
            tst_assert_equal(data->expected_last_delim, path_target->last_delimiter, "last delim at %ld");
            for (size_t i = 0 ; i < data->expected_length ; i++) {
                tst_assert_equal_ext(data->expected_path[i], path[i], "'%c'", "at index %ld", i);
            }

            path_destroy(make_system_allocator(), &path);
        }
)

tst_CREATE_TEST_CASE(path_append_nominal, path_append,
        .path = "some/path",
        .delim = '/',
        .appended = "with_something_after",
        .expected_path = "some/path/with_something_after",
        .expected_length = 31,
        .expected_last_delim = 9,
)
tst_CREATE_TEST_CASE(path_append_on_empty, path_append,
        .path = "",
        .delim = '/',
        .appended = "with_something_after",
        .expected_path = "with_something_after",
        .expected_length = 21,
        .expected_last_delim = 0,
)
tst_CREATE_TEST_CASE(path_append_on_single, path_append,
        .path = "alone",
        .delim = '/',
        .appended = "with_something_after",
        .expected_path = "alone/with_something_after",
        .expected_length = 27,
        .expected_last_delim = 5,
)
tst_CREATE_TEST_CASE(path_append_empty, path_append,
        .path = "some/path",
        .delim = '/',
        .appended = "",
        .expected_path = "some/path",
        .expected_length = 10,
        .expected_last_delim = 4,
)
tst_CREATE_TEST_CASE(path_append_other_path, path_append,
        .path = "some/path",
        .delim = '/',
        .appended = "and/another/path",
        .expected_path = "some/path/and/another/path",
        .expected_length = 27,
        .expected_last_delim = 21,
)

void path_execute_unittests(void)
{
    tst_run_test_case(path_create_nominal);
    tst_run_test_case(path_create_null);
    tst_run_test_case(path_create_empty);
    tst_run_test_case(path_create_no_folder);

    tst_run_test_case(path_up_once);
    tst_run_test_case(path_up_all_the_way);
    tst_run_test_case(path_up_beyond);
    tst_run_test_case(path_up_absolute_beyond);

    tst_run_test_case(path_append_nominal);
    tst_run_test_case(path_append_on_empty);
    tst_run_test_case(path_append_on_single);
    tst_run_test_case(path_append_empty);
    tst_run_test_case(path_append_other_path);
}

#endif
