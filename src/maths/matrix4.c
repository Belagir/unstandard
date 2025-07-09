
#include <ustd/math3d.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
matrix4
matrix4_identity(void)
{
    return (matrix4) {
             1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1,
    };
}

// -------------------------------------------------------------------------------------------------
matrix4
matrix_translate(matrix4 m, vector3 offset)
{
    m.m12 += offset.x;
    m.m13 += offset.y;
    m.m14 += offset.z;

    return m;
}

// -------------------------------------------------------------------------------------------------
vector3
matrix_origin(matrix4 m)
{
    return (vector3) { m.m12, m.m13, m.m14 };
}

// -------------------------------------------------------------------------------------------------
matrix4
matrix4_get_view_matrix(vector3 eye, vector3 target, vector3 up)
{
    matrix4 matrix_out = { 0u };
    vector3 vz = { 0u };
    vector3 vx = { 0u };
    vector3 vy = { 0u };

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
matrix4
matrix4_get_projection_matrix(f32 near_distance, f32 far_distance, f32 fov, f32 aspect)
{
    matrix4 matrix_out = { 0u };
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
matrix4
matrix4_get_model_matrix(f32 x, f32 y, f32 z, f32 scale)
{
    matrix4 matrix_out = { 0u };

    matrix_out = matrix4_identity();

    matrix_out.m0 = scale;
    matrix_out.m5 = scale;
    matrix_out.m10 = scale;

    matrix_out.m12 = x;
    matrix_out.m13 = y;
    matrix_out.m14 = z;

    return matrix_out;
}