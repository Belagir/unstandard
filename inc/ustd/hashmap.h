
#ifndef HASHMAP_H__
#define HASHMAP_H__

#include <ustd/array.h>

#define HASHMAP(type_) type_ *
#define HASHMAP_ANY void *

#define hashmap_get(hashmap_, index_, out_value_) \
        array_get(hashmap_, index_, out_value_)

#define hashmap_length(hashmap_) \
        arra_length(hashmap_)

#define hashmap_capacity(hashmap_) \
        array_capacity(hashmap_)

HASHMAP_ANY hashmap_create(
        struct allocator alloc,
        size_t element_size,
        size_t starting_capacity);

void hashmap_destroy(
        struct allocator alloc,
        HASHMAP_ANY *map);

void hashmap_ensure_capacity(
        struct allocator alloc,
        HASHMAP_ANY *map,
        size_t additional_capacity);

u32 hashmap_hash_of(
        const char *key, u32 seed);

size_t hashmap_index_of(
        HASHMAP_ANY map,
        const char *key);

size_t hashmap_index_of_hashed(
        HASHMAP_ANY map,
        u32 hash);

size_t hashmap_set(
        HASHMAP_ANY map,
        const char *key,
        void *value);

size_t hashmap_set_hashed(
        HASHMAP_ANY map,
        u32 hash,
        void *value);

void hashmap_remove(
        HASHMAP_ANY map,
        const char *key);

void hashmap_remove_hashed(
        HASHMAP_ANY map,
        u32 hash);

ARRAY(u32) hashmap_keys(
        HASHMAP_ANY map);

#endif
