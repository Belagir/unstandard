
#include <ustd/common.h>

// -------------------------------------------------------------------------------------------------
u32 hash_jenkins_one_at_a_time(const byte *key, size_t length, u32 seed)
{
    size_t i = 0u;
    u32 hash = seed;

    while (i < length) {
        hash += key[i++];
        hash += hash << 10u;
        hash ^= hash >> 6u;
    }

    hash += hash << 3u;
    hash ^= hash >> 11u;
    hash += hash << 15u;

    return hash;
}

// -------------------------------------------------------------------------------------------------
i32 hash_compare(const void *lhs, const void *rhs)
{
    u32 val_lhs = *(u32 *) lhs;
    u32 val_rhs = *(u32 *) rhs;

    return (val_lhs > val_rhs) - (val_lhs < val_rhs);
}

// -------------------------------------------------------------------------------------------------
i32 hash_compare_doubleref(const void *lhs, const void *rhs)
{
    return hash_compare(*(u32 **) lhs, *(u32 **) rhs);
}
