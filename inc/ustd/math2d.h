
#ifndef __UNSTANDARD_MATH_2D_H__
#define __UNSTANDARD_MATH_2D_H__

#include <ustd/common.h>

#define VECTOR2_ZERO ((vector2_t) { 0.f, 0.f })

#define VECTOR2_X_POSITIVE ((vector2_t) {  1.f, 0.f })
#define VECTOR2_Y_POSITIVE ((vector2_t) { 0.f,  1.f })

#define VECTOR2_X_NEGATIVE ((vector2_t) { -1.f, 0.f })
#define VECTOR2_Y_NEGATIVE ((vector2_t) { 0.f, -1.f })

/**
 * @brief
 *
 */
typedef union vector2_t {
    f32 vec[2u];
    struct { f32 x, y; };
} vector2_t;

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2_t vector2_add(vector2_t v1, vector2_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2_t vector2_subsctract(vector2_t v1, vector2_t v2);

/**
 * @brief Calculates the area between two vectors. A bit like a cross product, but in 2D space.
 *
 * @param v1
 * @param v2
 * @return vector2_t
 */
f32 vector2_area_product(vector2_t v1, vector2_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
f32 vector2_dot_product(vector2_t v1, vector2_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2_t vector2_members_product(vector2_t v1, vector2_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
f32 vector2_angle_to(vector2_t v1, vector2_t v2);

/**
 * @brief
 *
 * @param v
 * @return
 */
vector2_t vector2_normalize(vector2_t v);

/**
 * @brief
 *
 * @param v
 * @return f32
 */
f32 vector2_euclidian_norm(vector2_t v);

/**
 * @brief
 *
 * @param scale
 * @param v
 * @return
 */
vector2_t vector2_scale(f32 scale, vector2_t v);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @param max_ulps_diff
 * @return true
 * @return false
 */
bool vector2_is_almost_equal(vector2_t v1, vector2_t v2, u32 max_ulps_diff);

#endif
