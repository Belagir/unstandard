/**
 * @file array_impl.h
 * @author gabriel
 * @brief
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef UNSTANDARD_ARRAY_IMPL_H__
#define UNSTANDARD_ARRAY_IMPL_H__

#include "../ustd/array.h"

/**
 * @brief
 *
 */
struct array_impl {
    size_t length;
    size_t capacity;
    u32 stride;
    byte data[];
};

/**
 * @brief
 *
 * @param some_array
 * @return struct array_impl
 */
struct array_impl array_impl_of(void *some_array);

#endif
