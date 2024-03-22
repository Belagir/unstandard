
#include <ustd/math3d.h>

#include <math.h>

#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_identity(void)
{
    return (quaternion_t) { 0.0f, 0.0f, 0.0f, 1.0f };
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_add(quaternion_t q1, quaternion_t q2)
{
    return (quaternion_t) { q1.i + q2.i, q1.j + q2.j, q1.k + q2.k, q1.w + q2.w };
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_multiply(quaternion_t q1, quaternion_t q2)
{
    quaternion_t result = { 0u };

    result.w = (q1.w * q2.w) - (q1.i * q2.i) - (q1.j * q2.j) - (q1.k * q2.k);
    result.i = (q1.j * q2.k) - (q1.k * q2.j) + (q1.w * q2.i) + (q1.i * q2.w);
    result.j = (q1.k * q2.i) - (q1.i * q2.k) + (q1.w * q2.j) + (q1.j * q2.w);
    result.k = (q1.i * q2.j) - (q1.j * q2.i) + (q1.w * q2.k) + (q1.k * q2.w);

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_from_axis_and_angle(vector3_t axis, f32 angle)
{
    quaternion_t result = { 0u };
    vector3_t normalized_axis = { 0u };
    f32 half_angle = 0.0f;
    f32 sin_of_half_angle = 0.0f;

    half_angle = angle / 2.0f;

    // normalize vector3
    normalized_axis = vector3_normalize(axis);

    // quaternion generation
    sin_of_half_angle = sinf(half_angle);
    result.w = cosf(half_angle);
    result.i = normalized_axis.x * sin_of_half_angle;
    result.j = normalized_axis.y * sin_of_half_angle;
    result.k = normalized_axis.z * sin_of_half_angle;

    // normalize quaternion
    result = quaternion_normalize(result);

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_from_vector3_to_vector3(vector3_t v_from, vector3_t v_to)
{
    quaternion_t result = { 0u };
    f32 dot_product = 0.0f;
    vector3_t cross_product = { 0u };

    dot_product = vector3_dot_product(v_from, v_to);
    cross_product = vector3_cross_product(v_from, v_to);

    result.i = cross_product.x;
    result.j = cross_product.y;
    result.k = cross_product.z;
    result.w = 1.0f + dot_product;

    result = quaternion_normalize(result);

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_conjugate(quaternion_t q)
{
    return (quaternion_t) { .i = -q.i, .j = -q.j, .k = -q.k, .w = q.w };
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_invert(quaternion_t q)
{
    return quaternion_scale(1.0f / quaternion_euclidian_norm_squared(q), quaternion_conjugate(q));
}

// -------------------------------------------------------------------------------------------------
f32
quaternion_euclidian_norm(quaternion_t q)
{
    return sqrtf(quaternion_euclidian_norm_squared(q));
}

// -------------------------------------------------------------------------------------------------
f32
quaternion_euclidian_norm_squared(quaternion_t q)
{
    return (q.i * q.i) + (q.j * q.j) + (q.k * q.k) + (q.w * q.w);
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_scale(f32 scale, quaternion_t q)
{
    return (quaternion_t) { .i = (scale * q.i), .j = (scale * q.j), .k = (scale * q.k), .w = (scale * q.w) };
}

f32
quaternion_dot_product(quaternion_t q1, quaternion_t q2)
{
    f32 result = 0.0f;

    result = MAX(-1.0f + __FLT_EPSILON__, MIN(1.0f - __FLT_EPSILON__, (q1.i * q2.i) + (q1.j * q2.j) + (q1.k * q2.k) + (q1.w * q2.w)));

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_normalize(quaternion_t q)
{
    f32 length = 0.0f;
    f32 inv_length = 0.0f;

    length = quaternion_euclidian_norm(q);

    if (float_equal(length, 0.0f, 1u))
    {
        length = 1.0f;
    }

    inv_length = 1.0f / length;

    return (quaternion_t) { .i = q.i * inv_length, .j = q.j * inv_length, .k = q.k * inv_length, .w = q.w * inv_length };
}

// -------------------------------------------------------------------------------------------------
matrix4_t
quaternion_to_matrix(quaternion_t q)
{
    matrix4_t result = matrix4_identity();

    f32 a2 = q.i * q.i;
    f32 b2 = q.j * q.j;
    f32 c2 = q.k * q.k;
    f32 ac = q.i * q.k;
    f32 ab = q.i * q.j;
    f32 bc = q.j * q.k;
    f32 ad = q.w * q.i;
    f32 bd = q.w * q.j;
    f32 cd = q.w * q.k;

    result.m0  = 1.0f - 2.0f * (b2 + c2);
    result.m1  = 2.0f * (ab + cd);
    result.m2  = 2.0f * (ac - bd);

    result.m4  = 2.0f * (ab - cd);
    result.m5  = 1.0f - 2.0f * (a2 + c2);
    result.m6  = 2.0f * (bc + ad);

    result.m8  = 2.0f * (ac + bd);
    result.m9  = 2.0f * (bc - ad);
    result.m10 = 1.0f - 2.0f * (a2 + b2);

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_linear_interpolation(quaternion_t q1, quaternion_t q2, f32 amount)
{
    quaternion_t result = { 0 };

    result.i = q1.i + amount * (q2.i - q1.i);
    result.j = q1.j + amount * (q2.j - q1.j);
    result.k = q1.k + amount * (q2.k - q1.k);
    result.w = q1.w + amount * (q2.w - q1.w);

    return result;
}

// -------------------------------------------------------------------------------------------------
quaternion_t
quaternion_spherical_linear_interpolation(quaternion_t q1, quaternion_t q2, f32 amount)
{
    const f32 angle = acosf(quaternion_dot_product(q1, q2));
    const f32 denom = sinf(angle);

    if (float_equal(amount, 0.0f, 20u) || float_equal(angle, 0.0f, 1u))
    {
        return q1;
    }
    else if (float_equal(amount, 1.0f, 20u))
    {
        return q2;
    }

    return  quaternion_scale(1.0f / denom,
            quaternion_add(quaternion_scale(sinf((1.0f - amount) * angle), q1),
                           quaternion_scale(sinf((amount)        * angle), q2)));
}

// -------------------------------------------------------------------------------------------------
f32
quaternion_geodesic_distance(quaternion_t q1, quaternion_t q2)
{
    f32 dot_product = 0.0f;

    dot_product  = (q1.i * q2.i) + (q1.j * q2.j) + (q1.k * q2.k) + (q1.w * q2.w);

    // with rotation (so normalized) quaternions, having the dot product > 1.0f should not happen but it does
    if (dot_product > 1.0f)
    {
        dot_product = 1.0f - __FLT_EPSILON__;
    }

    return acosf((2.0f * (dot_product * dot_product)) - 1.0f);
}
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_identity,
        {
            quaternion_t original_quaternion;
        },
        {
            quaternion_t id_quaternion = data->original_quaternion;

            id_quaternion = quaternion_identity();

            tst_assert(float_equal(1.0f, id_quaternion.w, 1u), "expected a real part of %f, got %f", 1.0f, id_quaternion.w);
            tst_assert(float_equal(0.0f, id_quaternion.i, 1u), "expected a i part of %f, got %f", 0.0f, id_quaternion.i);
            tst_assert(float_equal(0.0f, id_quaternion.j, 1u), "expected a j part of %f, got %f", 0.0f, id_quaternion.j);
            tst_assert(float_equal(0.0f, id_quaternion.k, 1u), "expected a k part of %f, got %f", 0.0f, id_quaternion.k);
        }
)

tst_CREATE_TEST_CASE(quaternion_identity_nominal, quaternion_identity,
        .original_quaternion = { .i =  0.0f, .j =  0.0f, .k =  0.0f, .w =  0.0f })
tst_CREATE_TEST_CASE(quaternion_identity_negative, quaternion_identity,
        .original_quaternion = { .i = -3.0f, .j = -3.0f, .k = -3.0f, .w = -3.0f })
tst_CREATE_TEST_CASE(quaternion_identity_positive, quaternion_identity,
        .original_quaternion = { .i =  3.0f, .j =  3.0f, .k =  3.0f, .w =  3.0f })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_multiplication,
        {
            quaternion_t first_operand;
            quaternion_t second_operand;

            quaternion_t expected_result;
        },
        {
            quaternion_t result = { 0u };

            result = quaternion_multiply(data->first_operand, data->second_operand);

            tst_assert(float_equal(data->expected_result.w, result.w, 1u), "expected a real part of %f, got %f", data->expected_result.w, result.w);
            tst_assert(float_equal(data->expected_result.i, result.i, 1u), "expected a i part of %f, got %f", data->expected_result.i, result.i);
            tst_assert(float_equal(data->expected_result.j, result.j, 1u), "expected a j part of %f, got %f", data->expected_result.j, result.j);
            tst_assert(float_equal(data->expected_result.k, result.k, 1u), "expected a k part of %f, got %f", data->expected_result.k, result.k);
        }
)

tst_CREATE_TEST_CASE(quaternion_multiplication_nominal, quaternion_multiplication,
        .first_operand   = { .i =  3.0f, .j =  0.0f, .k = -1.0f, .w =  0.0f },
        .second_operand  = { .i =  0.0f, .j =  1.0f, .k =  1.0f, .w =  2.0f },
        .expected_result = { .i =  7.0f, .j = -3.0f, .k =  1.0f, .w =  1.0f })

tst_CREATE_TEST_CASE(quaternion_multiplication_nominal_inverted, quaternion_multiplication,
        .first_operand   = { .i =  0.0f, .j =  1.0f, .k =  1.0f, .w =  2.0f },
        .second_operand  = { .i =  3.0f, .j =  0.0f, .k = -1.0f, .w =  0.0f },
        .expected_result = { .i =  5.0f, .j =  3.0f, .k = -5.0f, .w =  1.0f })


tst_CREATE_TEST_CASE(quaternion_multiplication_with_id, quaternion_multiplication,
        .first_operand   = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .second_operand  = quaternion_identity(),
        .expected_result = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f })

tst_CREATE_TEST_CASE(quaternion_multiplication_with_id_inverted, quaternion_multiplication,
        .first_operand   = quaternion_identity(),
        .second_operand  = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .expected_result = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f })

tst_CREATE_TEST_CASE(quaternion_multiplication_with_zero, quaternion_multiplication,
        .first_operand   = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .second_operand  = { 0u },
        .expected_result = { 0u })

tst_CREATE_TEST_CASE(quaternion_multiplication_with_zero_inverted, quaternion_multiplication,
        .first_operand   = { 0u },
        .second_operand  = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .expected_result = { 0u })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_conjugate,
        {
            quaternion_t input;
            quaternion_t expected_result;
        },
        {
            quaternion_t result = { 0u };

            result = quaternion_conjugate(data->input);

            tst_assert(float_equal(data->expected_result.w, result.w, 1u), "expected a real part of %f, got %f", data->expected_result.w, result.w);
            tst_assert(float_equal(data->expected_result.i, result.i, 1u), "expected a i part of %f, got %f", data->expected_result.i, result.i);
            tst_assert(float_equal(data->expected_result.j, result.j, 1u), "expected a j part of %f, got %f", data->expected_result.j, result.j);
            tst_assert(float_equal(data->expected_result.k, result.k, 1u), "expected a k part of %f, got %f", data->expected_result.k, result.k);
        }
)

tst_CREATE_TEST_CASE(quaternion_conjugate_nominal, quaternion_conjugate,
        .input           = { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .expected_result = { .i = -3.0f, .j = -4.0f, .k =  6.0f, .w =  3.0f })

tst_CREATE_TEST_CASE(quaternion_conjugate_with_id, quaternion_conjugate,
        .input           = quaternion_identity(),
        .expected_result = quaternion_identity())

tst_CREATE_TEST_CASE(quaternion_conjugate_with_zero, quaternion_conjugate,
        .input           = { 0u },
        .expected_result = { 0u })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_conjugate_multiplication,
        {
            quaternion_t first_operand;
            quaternion_t second_operand;
        },
        {
            quaternion_t multiplied_conj = { 0u };
            quaternion_t conjs_multiplied = { 0u };

            multiplied_conj = quaternion_conjugate(quaternion_multiply(data->first_operand, data->second_operand));
            conjs_multiplied = quaternion_multiply(quaternion_conjugate(data->second_operand), quaternion_conjugate(data->first_operand));

            tst_assert(float_equal(multiplied_conj.w, conjs_multiplied.w, 1u), "expected a real part of %f, got %f", multiplied_conj.w, conjs_multiplied.w);
            tst_assert(float_equal(multiplied_conj.i, conjs_multiplied.i, 1u), "expected a i part of %f, got %f",    multiplied_conj.i, conjs_multiplied.i);
            tst_assert(float_equal(multiplied_conj.j, conjs_multiplied.j, 1u), "expected a j part of %f, got %f",    multiplied_conj.j, conjs_multiplied.j);
            tst_assert(float_equal(multiplied_conj.k, conjs_multiplied.k, 1u), "expected a k part of %f, got %f",    multiplied_conj.k, conjs_multiplied.k);
        }
)

tst_CREATE_TEST_CASE(quaternion_conjugate_multiplication_nominal, quaternion_conjugate_multiplication,
        .first_operand =  { .i =  3.0f, .j =  4.0f, .k = -6.0f, .w =  3.0f },
        .second_operand = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  0.0f })

tst_CREATE_TEST_CASE(quaternion_conjugate_multiplication_zero, quaternion_conjugate_multiplication,
        .first_operand =  { 0u },
        .second_operand = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  0.0f })

tst_CREATE_TEST_CASE(quaternion_conjugate_multiplication_identity, quaternion_conjugate_multiplication,
        .first_operand =  quaternion_identity(),
        .second_operand = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  0.0f })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_scale,
        {
            quaternion_t quaternion;
            f32 scale;

            quaternion_t expected_result;
        },
        {
            quaternion_t result = { 0u };

            result = quaternion_scale(data->scale, data->quaternion);

            tst_assert(float_equal(data->expected_result.w, result.w, 1u), "expected a real part of %f, got %f", data->expected_result.w, result.w);
            tst_assert(float_equal(data->expected_result.i, result.i, 1u), "expected a i part of %f, got %f", data->expected_result.i, result.i);
            tst_assert(float_equal(data->expected_result.j, result.j, 1u), "expected a j part of %f, got %f", data->expected_result.j, result.j);
            tst_assert(float_equal(data->expected_result.k, result.k, 1u), "expected a k part of %f, got %f", data->expected_result.k, result.k);

        }
)

tst_CREATE_TEST_CASE(quaternion_scale_nominal, quaternion_scale,
        .quaternion      = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  4.0f },
        .scale           = 16.0f,
        .expected_result = { .i = 88.0f, .j = -16.0f, .k = 192.0f, .w = 64.0f })

tst_CREATE_TEST_CASE(quaternion_scale_by_zero, quaternion_scale,
        .quaternion      = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  4.0f },
        .scale           = 0.0f,
        .expected_result = { .i =  0.0f, .j =  0.0f, .k =  0.0f, .w =  0.0f })

tst_CREATE_TEST_CASE(quaternion_scale_by_one, quaternion_scale,
        .quaternion      = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  4.0f },
        .scale           = 1.0f,
        .expected_result = { .i =  5.5f, .j = -1.0f, .k = 12.0f, .w =  4.0f })
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
tst_CREATE_TEST_SCENARIO(quaternion_norm,
        {
            quaternion_t quaternion;
            f32 expected_norm;
        },
        {
            f32 norm = 0.0f;
            f32 norm_squared = 0.0f;

            norm = quaternion_euclidian_norm(data->quaternion);
            norm_squared = quaternion_euclidian_norm_squared(data->quaternion);

            tst_assert(float_equal(norm * norm, norm_squared, 1u), "norm square mismatch : %f and %f", norm * norm, norm_squared);
            tst_assert(float_equal(data->expected_norm, norm, 1u), "norm mismatch : expected %f, got %f", data->expected_norm, norm);
        }
)

tst_CREATE_TEST_CASE(quaternion_norm_nominal, quaternion_norm,
        .quaternion    = { .i = 5.5f, .j = -1.0f, .k = 12.0f, .w = 4.0f },
        .expected_norm = 13.829317f)

tst_CREATE_TEST_CASE(quaternion_norm_zero, quaternion_norm,
        .quaternion    = { 0u },
        .expected_norm = 0.0f)

tst_CREATE_TEST_CASE(quaternion_norm_id, quaternion_norm,
        .quaternion    = quaternion_identity(),
        .expected_norm = 1.0f)
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void
quaternion_execute_unittests(void)
{
    tst_run_test_case(quaternion_identity_nominal);
    tst_run_test_case(quaternion_identity_negative);
    tst_run_test_case(quaternion_identity_positive);

    tst_run_test_case(quaternion_multiplication_nominal);
    tst_run_test_case(quaternion_multiplication_nominal_inverted);
    tst_run_test_case(quaternion_multiplication_with_id);
    tst_run_test_case(quaternion_multiplication_with_id_inverted);

    tst_run_test_case(quaternion_multiplication_with_zero);
    tst_run_test_case(quaternion_multiplication_with_zero_inverted);

    tst_run_test_case(quaternion_conjugate_nominal);
    tst_run_test_case(quaternion_conjugate_with_id);
    tst_run_test_case(quaternion_conjugate_with_zero);

    tst_run_test_case(quaternion_conjugate_multiplication_nominal);
    tst_run_test_case(quaternion_conjugate_multiplication_zero);
    tst_run_test_case(quaternion_conjugate_multiplication_identity);

    tst_run_test_case(quaternion_scale_nominal);
    tst_run_test_case(quaternion_scale_by_zero);
    tst_run_test_case(quaternion_scale_by_one);

    tst_run_test_case(quaternion_norm_nominal);
    tst_run_test_case(quaternion_norm_zero);
    tst_run_test_case(quaternion_norm_id);
}

#endif
