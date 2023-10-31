
#include <ustd/math3d.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
vector3_t
vector3_add(vector3_t v1, vector3_t v2)
{
    return (vector3_t) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// -------------------------------------------------------------------------------------------------
vector3_t
vector3_scale(f32 scale, vector3_t v1)
{
    return (vector3_t) { v1.x * scale, v1.y * scale, v1.z * scale };
}

// -------------------------------------------------------------------------------------------------
f32
vector3_euclidian_norm(vector3_t v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// -------------------------------------------------------------------------------------------------
vector3_t
vector3_normalize(vector3_t v)
{
    f32 vector_length = 0.0f;
    f32 inv_vector_length = 0.0f;

    vector_length = vector3_euclidian_norm(v);

    if (float_equal(vector_length, 0.0f, 1u))
    {
        return (vector3_t) { 0u };
    }

    inv_vector_length = 1.0f / vector_length;

    return (vector3_t) { .x = v.x * inv_vector_length, .y = v.y * inv_vector_length, .z = v.z * inv_vector_length };
}

// -------------------------------------------------------------------------------------------------
vector3_t
vector3_rotate_by_quaternion(vector3_t v, quaternion_t q)
{
    vector3_t rotated = { 0u };

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

vector3_t
vector3_substract(vector3_t v1, vector3_t v2)
{
    vector3_t result = { 0u };

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
}

// -------------------------------------------------------------------------------------------------

vector3_t
vector3_cross_product(vector3_t v1, vector3_t v2)
{
    vector3_t result = { 0u };

    result.x = (v1.y * v2.z) - (v1.z * v2.y);
    result.y = (v1.z * v2.x) - (v1.x * v2.z);
    result.z = (v1.x * v2.y) - (v1.y * v2.x);

    return result;
}

// -------------------------------------------------------------------------------------------------
f32
vector3_dot_product(vector3_t v1, vector3_t v2)
{
    f32 result = 0.0f;

    result = ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));

    return result;
}

// -------------------------------------------------------------------------------------------------
u32
vector3_is_almost_equal(vector3_t v1, vector3_t v2, u32 max_ulps_diff)
{
    return float_equal(v1.x, v2.x, max_ulps_diff) && float_equal(v1.y, v2.y, max_ulps_diff) && float_equal(v1.z, v2.z, max_ulps_diff);
}
