
#include <ustd/math3d.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
matrix4_t
matrix4_identity(void)
{
    return (matrix4_t) {
             1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1,
    };
}

// -------------------------------------------------------------------------------------------------
void
matrix4_to_array(matrix4_t m, f32 (*array)[16u])
{
    (*array)[0u] = m.m0;
    (*array)[1u] = m.m1;
    (*array)[2u] = m.m2;
    (*array)[3u] = m.m3;

    (*array)[4u] = m.m4;
    (*array)[5u] = m.m5;
    (*array)[6u] = m.m6;
    (*array)[7u] = m.m7;

    (*array)[8u] = m.m8;
    (*array)[9u] = m.m9;
    (*array)[10u] = m.m10;
    (*array)[11u] = m.m11;

    (*array)[12u] = m.m12;
    (*array)[13u] = m.m13;
    (*array)[14u] = m.m14;
    (*array)[15u] = m.m15;
}

// -------------------------------------------------------------------------------------------------
matrix4_t
matrix4_get_view_matrix(vector3_t eye, vector3_t target, vector3_t up)
{
    matrix4_t matrix_out = { 0u };
    vector3_t vz = { 0u };
    vector3_t vx = { 0u };
    vector3_t vy = { 0u };

    matrix_out = matrix4_identity();

    vz = vector3_normalize(vector3_substract(eye, target));
    vx = vector3_normalize(vector3_cross_product(up, vz));
    vy = vector3_cross_product(vz, vx);

    matrix_out.m0 = vx.x;
    matrix_out.m1 = vy.x;
    matrix_out.m2 = vz.x;
    matrix_out.m3 = 0.0f;

    matrix_out.m4 = vx.y;
    matrix_out.m5 = vy.y;
    matrix_out.m6 = vz.y;
    matrix_out.m7 = 0.0f;

    matrix_out.m8 =  vx.z;
    matrix_out.m9 =  vy.z;
    matrix_out.m10 = vz.z;
    matrix_out.m11 = 0.0f;

    matrix_out.m12 = -vector3_dot_product(vx, eye);
    matrix_out.m13 = -vector3_dot_product(vy, eye);
    matrix_out.m14 = -vector3_dot_product(vz, eye);
    matrix_out.m15 = 1.0f;

    return matrix_out;
}

// -------------------------------------------------------------------------------------------------
matrix4_t
matrix4_get_projection_matrix(f32 near_distance, f32 far_distance, f32 fov, f32 aspect)
{
    matrix4_t matrix_out = { 0u };
    f64 top = 0.0f;
    f64 bottom = 0.0f;
    f64 left = 0.0f;
    f64 right = 0.0f;
    f32 rl = 0.0f;
    f32 tb = 0.0f;
    f32 fn = 0.0f;

    matrix_out = matrix4_identity();

    top = near_distance * tanf(fov * DEG_TO_RAD * 0.5f);
    bottom = -top;
    right = top * aspect;
    left = -right;

    rl = (f32) (right - left);
    tb = (f32) (top - bottom);
    fn = (f32) (far_distance - near_distance);

    matrix_out.m0 = (near_distance * 2.0f) / rl;
    matrix_out.m5 = (near_distance * 2.0f) / tb;
    matrix_out.m8 = (f32) (right + left) / rl;
    matrix_out.m9 = (f32) (top + bottom) / tb;
    matrix_out.m10 = -(far_distance + near_distance) / fn;
    matrix_out.m11 = -1.0f;
    matrix_out.m14 = -(far_distance * near_distance * 2.0f) / fn;
    matrix_out.m15 = 1.0f;

    return matrix_out;
}

// -------------------------------------------------------------------------------------------------
matrix4_t
matrix4_get_model_matrix(f32 x, f32 y, f32 z, f32 scale)
{
    matrix4_t matrix_out = { 0u };

    matrix_out = matrix4_identity();

    matrix_out.m0 = scale;
    matrix_out.m5 = scale;
    matrix_out.m10 = scale;

    matrix_out.m12 = x;
    matrix_out.m13 = y;
    matrix_out.m14 = z;

    return matrix_out;
}