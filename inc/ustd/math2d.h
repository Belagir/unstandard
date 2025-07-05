
#ifndef __UNSTANDARD_MATH_2D_H__
#define __UNSTANDARD_MATH_2D_H__

#include "common.h"
#include "math.h"

#define VECTOR2_ZERO ((vector2) {{ 0.f, 0.f }})
#define VECTOR2_ORIGIN VECTOR2_ZERO

#define VECTOR2_X_POSITIVE ((vector2) {{  1.f, 0.f }})
#define VECTOR2_Y_POSITIVE ((vector2) {{ 0.f,  1.f }})

#define VECTOR2_X_NEGATIVE ((vector2) {{ -1.f, 0.f }})
#define VECTOR2_Y_NEGATIVE ((vector2) {{ 0.f, -1.f }})

/**
 * @brief
 *
 */
typedef union vector2 {
    f32 vec[2u];
    struct { f32 x, y; };
} vector2;

/**
 * @brief
 *
 * @param v
 * @return vector2
 */
vector2 vector2_negate(vector2 v);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2 vector2_add(vector2 v1, vector2 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2 vector2_substract(vector2 v1, vector2 v2);

/**
 * @brief Calculates the area between two vectors. A bit like a cross product, but in 2D space.
 *
 * @param v1
 * @param v2
 * @return vector2
 */
f32 vector2_area_product(vector2 v1, vector2 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
f32 vector2_dot_product(vector2 v1, vector2 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector2
 */
vector2 vector2_triple_product(vector2 v1, vector2 v2, vector2 v3);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2 vector2_members_product(vector2 v1, vector2 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
f32 vector2_angle_to(vector2 v1, vector2 v2);

/**
 * @brief Returns the anti-clockwise normal vector orthogonal to some other vector, normalized.
 *
 * @param v
 * @return
 */
vector2 vector2_normal_of(vector2 v);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return
 */
vector2 vector2_direction_to(vector2 v1, vector2 v2);

/**
 * @brief
 *
 * @param v
 * @return
 */
vector2 vector2_normalize(vector2 v);

/**
 * @brief
 *
 * @param v
 * @return f32
 */
f32 vector2_euclidian_norm(vector2 v);

/**
 * @brief
 *
 * @param scale
 * @param v
 * @return
 */
vector2 vector2_scale(f32 scale, vector2 v);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @param max_ulps_diff
 * @return true
 * @return false
 */
bool vector2_is_almost_equal(vector2 v1, vector2 v2, u32 max_ulps_diff);

#endif
