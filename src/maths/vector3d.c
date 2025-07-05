
#include <ustd/math3d.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
vector3
vector3_add(vector3 v1, vector3 v2)
{
    return (vector3) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// -------------------------------------------------------------------------------------------------
vector3
vector3_scale(f32 scale, vector3 v1)
{
    return (vector3) { v1.x * scale, v1.y * scale, v1.z * scale };
}

// -------------------------------------------------------------------------------------------------
f32
vector3_euclidian_norm(vector3 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// -------------------------------------------------------------------------------------------------
vector3
vector3_normalize(vector3 v)
{
    f32 vector_length = 0.0f;
    f32 inv_vector_length = 0.0f;

    vector_length = vector3_euclidian_norm(v);

    if (float_equal(vector_length, 0.0f, 1u))
    {
        return (vector3) { 0u };
    }

    inv_vector_length = 1.0f / vector_length;

    return (vector3) { .x = v.x * inv_vector_length, .y = v.y * inv_vector_length, .z = v.z * inv_vector_length };
}

// -------------------------------------------------------------------------------------------------
vector3
vector3_rotate_by_quaternion(vector3 v, quaternion q)
{
    vector3 rotated = { 0u };

    rotated.x =   (v.x * ((q.i * q.i) + (q.w * q.w) - (q.j * q.j) - (q.k * q.k)))
                + (v.y * (( 2.0f * q.i * q.j) - ( 2.0f * q.w * q.k)))
                + (v.z * (( 2.0f * q.i * q.k) + ( 2.0f * q.w * q.j)));

    rotated.y =   (v.x * (( 2.0f * q.w * q.k) + ( 2.0f * q.i * q.j)))
                + (v.y * ((q.w * q.w) - (q.i * q.i) + (q.j * q.j) - (q.k * q.k)))
                + (v.z * ((-2.0f * q.w * q.i) + ( 2.0f * q.j * q.k)));

    rotated.z =   (v.x * ((-2.0f * q.w * q.j) + ( 2.0f * q.i * q.k)))
                + (v.y * (( 2.0f * q.w * q.i) + ( 2.0f * q.j * q.k)))
                + (v.z * ((q.w * q.w) - (q.i * q.i) - (q.j * q.j) + (q.k * q.k)));

    return rotated;
}

// -------------------------------------------------------------------------------------------------

vector3
vector3_substract(vector3 v1, vector3 v2)
{
    vector3 result = { 0u };

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
}

// -------------------------------------------------------------------------------------------------

vector3
vector3_cross_product(vector3 v1, vector3 v2)
{
    vector3 result = { 0u };

    result.x = (v1.y * v2.z) - (v1.z * v2.y);
    result.y = (v1.z * v2.x) - (v1.x * v2.z);
    result.z = (v1.x * v2.y) - (v1.y * v2.x);

    return result;
}

// -------------------------------------------------------------------------------------------------
f32
vector3_dot_product(vector3 v1, vector3 v2)
{
    f32 result = 0.0f;

    result = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));

    return result;
}

// -------------------------------------------------------------------------------------------------
u32
vector3_is_almost_equal(vector3 v1, vector3 v2, u32 max_ulps_diff)
{
    return float_equal(v1.x, v2.x, max_ulps_diff) && float_equal(v1.y, v2.y, max_ulps_diff) && float_equal(v1.z, v2.z, max_ulps_diff);
}
