
#include <ustd/hashmap.h>

#include <ustd_impl/array_impl.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 */
struct hashmap_impl {
    ARRAY(u32) keys;

    size_t length;
    size_t capacity;
    u32 stride;
    byte data[];
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static struct hashmap_impl *hashmap_impl_of(HASHMAP_ANY map);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @param element_size
 * @param starting_capacity
 * @return HASHMAP_ANY
 */
HASHMAP_ANY hashmap_create(
        struct allocator alloc,
        size_t element_size,
        size_t starting_capacity)
{
    struct hashmap_impl *new_hashmap = nullptr;

    if ((element_size == 0) || (starting_capacity == 0)) {
        return nullptr;
    }

    new_hashmap = alloc.malloc(alloc,
            sizeof(*new_hashmap) + (starting_capacity * element_size));


    *new_hashmap = (struct hashmap_impl) {
            .keys = array_create(alloc,
                    sizeof(*new_hashmap->keys),
                    starting_capacity),

            .length = 0,
            .capacity = starting_capacity,
            .stride = (u32) element_size,
    };

    return &(new_hashmap->data);
}

/**
 * @brief
 *
 * @param alloc
 * @param hashmap
 */
void hashmap_destroy(
        struct allocator alloc,
        HASHMAP_ANY *map)
{
    struct hashmap_impl *target = nullptr;

    if (!map || !*map) {
        return;
    }

    target = hashmap_impl_of(*map);

    array_destroy(alloc, (ARRAY_ANY *) &(target->keys));
    alloc.free(alloc, target);

    *map = nullptr;
}

/**
 * @brief
 *
 * @param alloc
 * @param map
 * @param additional_capacity
 */
void hashmap_ensure_capacity(
        struct allocator alloc,
        HASHMAP_ANY *map,
        size_t additional_capacity)
{
    struct hashmap_impl *target = nullptr;
    size_t needed_size = 0;

    HASHMAP_ANY new_map = nullptr;
    struct hashmap_impl *new_map_impl = nullptr;
    struct array_impl *new_keys_impl = nullptr;

    if (!map || !*map || (additional_capacity == 0)) {
        return;
    }

    target = hashmap_impl_of(*map);

    needed_size = target->length + additional_capacity;
    if (needed_size < target->capacity) {
        return;
    }

    new_map = hashmap_create(alloc, target->stride, needed_size*2);
    new_map_impl = hashmap_impl_of(new_map);
    new_keys_impl = array_impl_of(new_map_impl->keys);

    for (size_t i = 0 ; i < target->length * target->stride ; i++) {
        new_map_impl->data[i] = target->data[i];
    }
    new_map_impl->length = target->length;

    for (size_t i = 0 ; i < target->length ; i++) {
        new_map_impl->keys[i] = target->keys[i];
    }
    new_keys_impl->length = array_length(target->keys);

    hashmap_destroy(alloc, map);
    *map = new_map;
}

/**
 * @brief
 *
 * @param key
 * @return u32
 */
u32 hashmap_hash_of(
        const char *key, u32 seed)
{
    return hash_jenkins_one_at_a_time((const byte *) key,
            c_string_length(key, 128, false), 0);
}

/**
 * @brief
 *
 * @param map
 * @param key
 * @return size_t
 */
size_t hashmap_index_of(
        HASHMAP_ANY map,
        const char *key)
{
    u32 hash = 0;

    if (!map) {
        return 0;
    }

    if (!key) {
        return array_length(map);
    }

    hash = hashmap_hash_of(key, 0);
    return hashmap_index_of_hashed(map, hash);
}

/**
 * @brief
 *
 * @param map
 * @param hash
 * @return size_t
 */
size_t hashmap_index_of_hashed(
        HASHMAP_ANY map,
        u32 hash)
{
    struct hashmap_impl *target = nullptr;
    bool exists = false;
    size_t pos = 0;

    if (!map) {
        return 0;
    }

    target = hashmap_impl_of(map);
    exists = array_sorted_find(target->keys, &hash_compare, &hash, &pos);

    if (exists) {
        return pos;
    } else {
        return array_length(map);
    }
}

/**
 * @brief
 *
 * @param map
 * @param key
 * @param value
 */
size_t hashmap_set(
        HASHMAP_ANY map,
        const char *key,
        void *value)
{
    u32 hash = 0;

    if (!map) {
        return 0;
    }

    if (!key) {
        return array_length(map);
    }

    hash = hashmap_hash_of(key, 0);
    return hashmap_set_hashed(map, hash, value);
}

/**
 * @brief
 *
 * @param map
 * @param hash
 * @param value
 */
size_t hashmap_set_hashed(
        HASHMAP_ANY map,
        u32 hash,
        void *value)
{
    struct hashmap_impl *target = nullptr;
    bool exists = false;
    size_t pos = 0;

    if (!map) {
        return 0;
    }

    if (!value) {
        return array_length(map);
    }

    target = hashmap_impl_of(map);
    exists = array_sorted_find(target->keys, &hash_compare, &hash, &pos);

    if (exists) {
        bytewise_copy(target->data + (target->stride * pos),
                value, target->stride);
    } else {
        array_insert_value(target->keys, pos, &hash);
        array_insert_value(map, pos, value);
    }

    return pos;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief
 *
 * @param map
 * @return struct hashmap_impl*
 */
static struct hashmap_impl *hashmap_impl_of(HASHMAP_ANY map)
{
    return CONTAINER_OF(map, struct hashmap_impl, data);
}
