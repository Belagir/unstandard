
#include <ustd/path.h>
#include <ustd_impl/array_impl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct path_impl {
    size_t last_delimiter;
    char delimiter;

    struct array_impl array;
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
    struct path_impl *new_path_target = nullptr;
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

void path_ensure_capacity(struct allocator alloc, PATH *path, size_t additional_capacity)
{
    size_t size_needed = 0;
    struct path_impl *target = nullptr;

    PATH new_path = nullptr;
    struct path_impl *new_path_target = nullptr;

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

    target->array.length = target->last_delimiter;
    target->array.data[target->last_delimiter] = '\0';

    array_find_back(&target->array.data, &character_compare, &target->delimiter, &target->last_delimiter);
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
    appended_len = c_string_length(appended, target->array.capacity, true);
    array_append_mem(path, appended, appended_len);
    array_find_back(&target->array.data, &character_compare, &target->delimiter, &target->last_delimiter);
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
    new_path->array.capacity = size;
    new_path->array.stride = sizeof(char);

    new_path->last_delimiter = 0;
    new_path->delimiter = delimiter;

    return (PATH) &(new_path->array.data);
}

// -----------------------------------------------------------------------------

static struct path_impl *path_impl_of(PATH path)
{
    return CONTAINER_OF(path, struct path_impl, array.data);
}
