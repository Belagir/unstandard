
#include <math.h>

#include <ustd/math2d.h>
#include <ustd/math3d.h>

// -------------------------------------------------------------------------------------------------
vector2_t vector2_negate(vector2_t v)
{
    return (vector2_t) { .x = -v.x, .y = -v.y };
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_add(vector2_t v1, vector2_t v2)
{
    return (vector2_t) { .x = v1.x + v2.x, .y = v1.y + v2.y, };
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_substract(vector2_t v1, vector2_t v2)
{
    return (vector2_t) { .x = v1.x - v2.x, .y = v1.y - v2.y, };
}

// -------------------------------------------------------------------------------------------------
f32 vector2_area_product(vector2_t v1, vector2_t v2)
{
    return (v1.x * v2.y) - (v1.y * v2.x);
}

// -------------------------------------------------------------------------------------------------
f32 vector2_dot_product(vector2_t v1, vector2_t v2)
{
    return (v1.x * v2.x) + (v2.y * v2.y);
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_triple_product(vector2_t v1, vector2_t v2, vector2_t v3)
{
    vector3_t vec_generalized = vector3_cross_product(
            vector3_cross_product(
                    (vector3_t) { .x = v1.x, .y = v1.y, .z = 1.f },
                    (vector3_t) { .x = v2.x, .y = v2.y, .z = 1.f }),
            (vector3_t) { .x = v3.x, .y = v3.y, .z = 1.f } );

    return (vector2_t) { vec_generalized.x, vec_generalized.y };
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_members_product(vector2_t v1, vector2_t v2)
{
    return (vector2_t) { .x = (v1.x * v2.x), .y = (v1.y * v2.y) };
}

// -------------------------------------------------------------------------------------------------
f32 vector2_angle_to(vector2_t v1, vector2_t v2)
{
    if (vector2_is_almost_equal(v1, VECTOR2_ZERO, 1u) || vector2_is_almost_equal(v1, VECTOR2_ZERO, 1u)) {
        return INFINITY;
    }

    return acosf(((v1.x * v1.y) + (v2.x * v2.y)) / (vector2_euclidian_norm(v1) * vector2_euclidian_norm(v2)));
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_direction_to(vector2_t v1, vector2_t v2)
{
    return vector2_normalize(vector2_substract(v2, v1));
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_normal_of(vector2_t v)
{
    return vector2_normalize((vector2_t) { -v.y, v.x });
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_normalize(vector2_t v)
{
    f32 v_length = 0.f;
    f32 v_inv_length = 0.f;

    v_length = vector2_euclidian_norm(v);

    if (float_equal(v_length, 0.f, 1u)) {
        return (vector2_t) { 0u };
    }

    v_inv_length = 1.f / v_length;

    return (vector2_t) { .x = v.x * v_inv_length, .y = v.y * v_inv_length, };
}

// -------------------------------------------------------------------------------------------------
f32 vector2_euclidian_norm(vector2_t v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

// -------------------------------------------------------------------------------------------------
vector2_t vector2_scale(f32 scale, vector2_t v)
{
    return (vector2_t) { .x = v.x * scale, .y = v.y * scale, };
}

// -------------------------------------------------------------------------------------------------
bool vector2_is_almost_equal(vector2_t v1, vector2_t v2, u32 max_ulps_diff)
{
    return float_equal(v1.x, v2.x, max_ulps_diff) && float_equal(v1.y, v2.y, max_ulps_diff);
}
