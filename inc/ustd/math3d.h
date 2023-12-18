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
#ifndef __MY3DMATH_H__
#define __MY3DMATH_H__

#include <ustd/math.h>

#define vector3_x_positive ((vector3_t) {  1.0f,  0.0f,  0.0f })        ///< unit 3D vector pointing to +x (cockatrice's forward)
#define vector3_y_positive ((vector3_t) {  0.0f,  1.0f,  0.0f })        ///< unit 3D vector pointing to +y (cockatrice's up)
#define vector3_z_positive ((vector3_t) {  0.0f,  0.0f,  1.0f })        ///< unit 3D vector pointing to +z (cockatrice's right)

#define vector3_x_negative ((vector3_t) { -1.0f,  0.0f,  0.0f })        ///< unit 3D vector pointing to -x (cockatrice's backward)
#define vector3_y_negative ((vector3_t) {  0.0f, -1.0f,  0.0f })        ///< unit 3D vector pointing to -y (cockatrice's up)
#define vector3_z_negative ((vector3_t) {  0.0f,  0.0f, -1.0f })        ///< unit 3D vector pointing to -z (cockatrice's left)

#define vector3_origin ((vector3_t) { 0.0f,  0.0f, 0.0f })             ///< zero 3D vector

/**
 * @brief 3D vector.
 */
typedef struct vector3_t
{
    f32 x;
    f32 y;
    f32 z;
} vector3_t;

/**
 * @brief Quaternion.
 */
typedef struct quaternion_t
{
    // complex part
    f32 i;
    f32 j;
    f32 k;
    // real part
    f32 w;
} quaternion_t;

/**
 * @brief 4x4 matrix.
 */
typedef struct matrix4_t
{
    f32 m0, m4, m8,  m12;
    f32 m1, m5, m9,  m13;
    f32 m2, m6, m10, m14;
    f32 m3, m7, m11, m15;
} matrix4_t;

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3_t
 */
vector3_t
vector3_add(vector3_t v1, vector3_t v2);

/**
 * @brief
 *
 * @param scale
 * @param v1
 * @return vector3_t
 */
vector3_t
vector3_scale(f32 scale, vector3_t v1);

/**
 * @brief
 *
 */
f32
vector3_euclidian_norm(vector3_t v);

/**
 * @brief
 *
 * @param v
 * @return vector3_t
 */
vector3_t
vector3_normalize(vector3_t v);

/**
 * @brief
 *
 * @param vector
 * @param rotation
 * @return vector3_t
 */
vector3_t
vector3_rotate_by_quaternion(vector3_t vector, quaternion_t rotation);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3_t
 */
vector3_t
vector3_substract(vector3_t v1, vector3_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3_t
 */
vector3_t
vector3_cross_product(vector3_t v1, vector3_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return vector3_t
 */
f32
vector3_dot_product(vector3_t v1, vector3_t v2);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @param max_ulps_diff
 * @return u32
 */
u32
vector3_is_almost_equal(vector3_t v1, vector3_t v2, u32 max_ulps_diff);

/**
 * @brief
 *
 * @return quaternion_t
 */
quaternion_t
quaternion_identity(void);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return quaternion_t
 */
quaternion_t
quaternion_multiply(quaternion_t q1, quaternion_t q2);

/**
 * @brief
 *
 * @param axis
 * @param angle
 * @return quaternion_t
 */
quaternion_t
quaternion_from_axis_and_angle(vector3_t axis, f32 angle);

/**
 * @brief
 *
 * @param v1
 * @param v2
 * @return quaternion_t
 */
quaternion_t
quaternion_from_vector3_to_vector3(vector3_t v1, vector3_t v2);

/**
 * @brief
 *
 * @param q
 * @return quaternion_t
 */
quaternion_t
quaternion_conjugate(quaternion_t q);

/**
 * @brief
 *
 * @param q
 * @return quaternion_t
 */
quaternion_t
quaternion_invert(quaternion_t q);

/**
 * @brief
 *
 * @param q
 * @return f32
 */
f32
quaternion_euclidian_norm(quaternion_t q);

/**
 * @brief
 *
 * @param q
 * @return f32
 */
f32
quaternion_euclidian_norm_squared(quaternion_t q);

/**
 * @brief
 *
 * @param scale
 * @param q
 * @return quaternion_t
 */
quaternion_t
quaternion_scale(f32 scale, quaternion_t q);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return f32
 */
f32
quaternion_dot_product(quaternion_t q1, quaternion_t q2);

/**
 * @brief
 *
 * @param q
 * @return quaternion_t
 */
quaternion_t
quaternion_normalize(quaternion_t q);

/**
 * @brief
 *
 * @param q
 * @return matrix4_t
 */
matrix4_t
quaternion_to_matrix(quaternion_t q);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @return quaternion_t
 */
quaternion_t
quaternion_linear_interpolation(quaternion_t q1, quaternion_t q2, f32 amount);

/**
 * @brief
 *
 * @param q1
 * @param q2
 * @param amount
 * @return quaternion_t
 */
quaternion_t
quaternion_spherical_linear_interpolation(quaternion_t q1, quaternion_t q2, f32 amount);

/**
 * @brief
 * @param q1
 * @param q2
 * @return f32
 */
f32
quaternion_geodesic_distance(quaternion_t q1, quaternion_t q2);

/**
 * @brief
 *
 */
quaternion_t
quaternion_spherical_linear_interpolation(quaternion_t q1, quaternion_t q2, f32 amount);

#ifdef UNITTESTING
void
quaternion_execute_unittests(void);
#endif

/**
 * @brief
 *
 * @return matrix4_t
 */
matrix4_t
matrix4_identity(void);

/**
 * @brief
 *
 */
void
matrix4_to_array(matrix4_t m, f32 (*array)[16u]);

/**
 * @brief
 *
 * @param eye
 * @param target
 * @param up
 * @return matrix4_t
 */
matrix4_t
matrix4_get_view_matrix(vector3_t eye, vector3_t target, vector3_t up);

/**
 * @brief
 *
 * @param near_distance
 * @param far_distance
 * @param fov
 * @param aspect
 * @return matrix4_t
 */
matrix4_t
matrix4_get_projection_matrix(f32 near_distance, f32 far_distance, f32 fov, f32 aspect);

/**
 * @brief
 *
 * @param x
 * @param y
 * @param z
 * @param scale
 * @return matrix4_t
 */
matrix4_t
matrix4_get_model_matrix(f32 x, f32 y, f32 z, f32 scale);

#endif
