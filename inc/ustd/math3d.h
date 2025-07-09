/**
 * @file math3d.h
 * @author gabriel
 * @brief Contains declaration about math operations traditionaly used in three dimensional space.
 * @version 0.1
 * @date 2023-12-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __UNSTANDARD_MATH_3D_H__
#define __UNSTANDARD_MATH_3D_H__

#include "math.h"

#define VECTOR3_X_POSITIVE ((vector3) {  1.0f,  0.0f,  0.0f })        ///< unit 3D vector pointing to +x (cockatrice's forward)
#define VECTOR3_Y_POSITIVE ((vector3) {  0.0f,  1.0f,  0.0f })        ///< unit 3D vector pointing to +y (cockatrice's up)
#define VECTOR3_Z_POSITIVE ((vector3) {  0.0f,  0.0f,  1.0f })        ///< unit 3D vector pointing to +z (cockatrice's right)

#define VECTOR3_X_NEGATIVE ((vector3) { -1.0f,  0.0f,  0.0f })        ///< unit 3D vector pointing to -x (cockatrice's backward)
#define VECTOR3_Y_NEGATIVE ((vector3) {  0.0f, -1.0f,  0.0f })        ///< unit 3D vector pointing to -y (cockatrice's up)
#define VECTOR3_Z_NEGATIVE ((vector3) {  0.0f,  0.0f, -1.0f })        ///< unit 3D vector pointing to -z (cockatrice's left)

#define VECTOR3_ORIGIN ((vector3) { 0.0f,  0.0f, 0.0f })             ///< zero 3D vector

/**
 * @brief 3D vector.
 */
typedef struct vector3
{
    f32 x;
    f32 y;
    f32 z;
} vector3;

/**
 * @brief Quaternion.
 */
typedef struct quaternion
{
    // complex part
    f32 i;
    f32 j;
    f32 k;
    // real part
    f32 w;
} quaternion;

/**
 * @brief 4x4 matrix.
 */
typedef struct matrix4
{
    f32 m0,  m1,  m2,  m3;
    f32 m4,  m5,  m6,  m7;
    f32 m8,  m9,  m10, m11;
    f32 m12, m13, m14, m15;
} matrix4;

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3
 */
vector3
vector3_add(vector3 v1, vector3 v2);

/**
 * @brief
 *
 * @param scale
 * @param v1
 * @return vector3
 */
vector3
vector3_scale(f32 scale, vector3 v1);

/**
 * @brief
 *
 */
f32
vector3_euclidian_norm(vector3 v);

/**
 * @brief
 *
 * @param v
 * @return vector3
 */
vector3
vector3_normalize(vector3 v);

/**
 * @brief
 *
 * @param vector
 * @param rotation
 * @return vector3
 */
vector3
vector3_rotate_by_quaternion(vector3 vector, quaternion rotation);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3
 */
vector3
vector3_substract(vector3 v1, vector3 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3
 */
vector3
vector3_cross_product(vector3 v1, vector3 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3
 */
f32
vector3_dot_product(vector3 v1, vector3 v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @param max_ulps_diff
 * @return u32
 */
u32
vector3_is_almost_equal(vector3 v1, vector3 v2, u32 max_ulps_diff);

/**
 * @brief
 *
 * @return quaternion
 */
quaternion
quaternion_identity(void);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return quaternion
 */
quaternion
quaternion_multiply(quaternion q1, quaternion q2);

/**
 * @brief
 *
 * @param axis
 * @param angle
 * @return quaternion
 */
quaternion
quaternion_from_axis_and_angle(vector3 axis, f32 angle);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return quaternion
 */
quaternion
quaternion_from_vector3_to_vector3(vector3 v1, vector3 v2);

/**
 * @brief
 *
 * @param q
 * @return quaternion
 */
quaternion
quaternion_conjugate(quaternion q);

/**
 * @brief
 *
 * @param q
 * @return quaternion
 */
quaternion
quaternion_invert(quaternion q);

/**
 * @brief
 *
 * @param q
 * @return f32
 */
f32
quaternion_euclidian_norm(quaternion q);

/**
 * @brief
 *
 * @param q
 * @return f32
 */
f32
quaternion_euclidian_norm_squared(quaternion q);

/**
 * @brief
 *
 * @param scale
 * @param q
 * @return quaternion
 */
quaternion
quaternion_scale(f32 scale, quaternion q);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return f32
 */
f32
quaternion_dot_product(quaternion q1, quaternion q2);

/**
 * @brief
 *
 * @param q
 * @return quaternion
 */
quaternion
quaternion_normalize(quaternion q);

/**
 * @brief
 *
 * @param q
 * @return matrix4
 */
matrix4
quaternion_to_matrix(quaternion q);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return quaternion
 */
quaternion
quaternion_linear_interpolation(quaternion q1, quaternion q2, f32 amount);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @param amount
 * @return quaternion
 */
quaternion
quaternion_spherical_linear_interpolation(quaternion q1, quaternion q2, f32 amount);

/**
 * @brief
 * @param q1
 * @param q2
 * @return f32
 */
f32
quaternion_geodesic_distance(quaternion q1, quaternion q2);

/**
 * @brief
 *
 */
quaternion
quaternion_spherical_linear_interpolation(quaternion q1, quaternion q2, f32 amount);

#ifdef UNITTESTING
void
quaternion_execute_unittests(void);
#endif

/**
 * @brief
 *
 * @return matrix4
 */
matrix4
matrix4_identity(void);

/**
 * @brief
 *
 * @param m
 * @param offset
 * @return matrix4
 */
matrix4
matrix_translate(matrix4 m, vector3 offset);

/**
 * @brief
 *
 * @param m
 * @return vector3
 */
vector3
matrix_origin(matrix4 m);

/**
 * @brief
 *
 * @param eye
 * @param target
 * @param up
 * @return matrix4
 */
matrix4
matrix4_get_view_matrix(vector3 eye, vector3 target, vector3 up);

/**
 * @brief
 *
 * @param near_distance
 * @param far_distance
 * @param fov
 * @param aspect
 * @return matrix4
 */
matrix4
matrix4_get_projection_matrix(f32 near_distance, f32 far_distance, f32 fov, f32 aspect);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param z
 * @param scale
 * @return matrix4
 */
matrix4
matrix4_get_model_matrix(f32 x, f32 y, f32 z, f32 scale);

#endif
