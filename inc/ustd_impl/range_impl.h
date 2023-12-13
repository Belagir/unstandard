/**
 * @file range_impl.h
 * @author gabriel
 * @brief This header provides some implementation details for the range module. This file gives informations that are not strictly private to the module, that does not need to be known to use it, while being important to build new functionalities. Import this to extend the range module.
 * @version 0.1
 * @date 2023-12-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __RANGE_IMPL_H__
#define __RANGE_IMPL_H__

#include <ustd/range.h>

/**
 * @brief
 *
 */
typedef struct range_anonymous {
    ///
    size_t length;
    ///
    const size_t capacity;
    ///
    byte data[];
} range_anonymous;

/**
 * @brief
 *
 * @param target
 * @param index
 * @param value
 */
void rrange_set(rrange_any target, size_t index, const void *value);

#endif
