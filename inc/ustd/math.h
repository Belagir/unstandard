/**
 * @file math.h
 * @author gabriel
 * @brief This module aggregates math-related declarations that are useful when used with the math.h module from the stdlib.
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __UNSTANDARD_MATH_H__
#define __UNSTANDARD_MATH_H__

#include <ustd/common.h>

#define SQRT_OF_3 (1.73205f)      ///< approximation of the square root of 3
#define THREE_HALVES (1.5f)       ///< not an *approximation* of 3 / 2
#define PI (3.14159265359f)
#define PI_T_2 ((2.0f) * (PI))    ///< 2 times pi

#define RAD_TO_DEG (57.2957795131f)
#define DEG_TO_RAD (0.0174532925199f)

/**
 * @brief Vector as two cartesian coordinates.
 * Colinear with the raylib Vector2 type.
 */
typedef struct vector2_cartesian {
    f32 v;
    f32 w;
} vector2_cartesian;

/**
 * @brief Vector as two polar coordinates.
 */
typedef struct vector2_polar {
    f32 angle;
    f32 magnitude;
} vector2_polar;

/**
 * @brief Converts a 2d vector from a polar coordinate pair to a cartesian one.
 *
 * @param[in] vec vector to convert
 * @return vector2_cartesian polar coordinates equivalent to the supplied vector
 */
vector2_cartesian vector2_polar_to_cartesian(vector2_polar vec);

/**
 * @brief Converts a 2d vector from a cartesian coordinate pair to a polar one.
 *
 * @param[in] vec vector to convert
 * @return vector2_polar cartesian coordinates equivalent to the supplied vector
 */
vector2_polar vector2_cartesian_to_polar(vector2_cartesian vec);

/**
 * @brief Checks for equality between two floats.
 *
 * @param[in] val1 first float
 * @param[in] val2 second float
 * @param[in] max_ulps_diff maximum difference in bit representation
 * @return u32 0 if unequal, 1 if equal (up to the MAX difference)
 */
u32 float_equal(f32 val1, f32 val2, u32 max_ulps_diff);

/**
 * @brief Calculates a normal distribution of a certain mean and variance (not squared variance !).
 *
 * @param[in] x input value
 * @param[in] mean mean if the distribution
 * @param[in] variance variance of the distribution
 * @return f32 chance of x along the distribution
 */
f32 normal_distribution(f32 x, f32 mean, f32 variance);

#endif
