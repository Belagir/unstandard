
#include <ustd/math.h>

#include <math.h>

// -------------------------------------------------------------------------------------------------
vector2_cartesian vector2_polar_to_cartesian(vector2_polar vec)
{
    return (vector2_cartesian) { .v = ((f32) cos(vec.angle)) * vec.magnitude, .w = ((f32) sin(vec.angle)) * vec.magnitude };
}

// -------------------------------------------------------------------------------------------------
vector2_polar vector2_cartesian_to_polar(vector2_cartesian vec)
{
    return (vector2_polar) { .angle = (f32) (atan2f(vec.v, vec.w)), .magnitude = (f32) (sqrt(pow(vec.v, 2.0f) + pow(vec.w, 2.0f))) };
}
