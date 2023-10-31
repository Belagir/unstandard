
#include <ustd/math.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
vector2_cartesian_t
vector2_polar_to_cartesian(vector2_polar_t vec)
{
    vector2_cartesian_t result = { 0u };

    result.v = ((f32) cos(vec.angle)) * vec.magnitude;
    result.w = ((f32) sin(vec.angle)) * vec.magnitude;

    return result;
}

// -------------------------------------------------------------------------------------------------
vector2_polar_t
vector2_cartesian_to_polar(vector2_cartesian_t vec)
{
    vector2_polar_t result = { 0u };

    result.magnitude = (f32) (sqrt(pow(vec.v, 2.0f) + pow(vec.w, 2.0f)));
    result.angle = (f32) (atan2f(vec.v, vec.w));

    return result;
}
