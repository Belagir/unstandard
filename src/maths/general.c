
#include <ustd/math.h>
#include <math.h>
#include <stdlib.h>

// -------------------------------------------------------------------------------------------------
f32
normal_distribution(f32 x, f32 mean, f32 variance)
{
    return (f32) (1.0f / (variance * sqrt(2.0f * (PI)))) * (f32) exp(-0.5f * powf((x - mean) / variance, 2.0f));
}

// -------------------------------------------------------------------------------------------------
u32
float_equal(f32 val1, f32 val2, u32 max_ulps_diff)
{
    i32 repr_val1 = *((i32*) &val1);
    i32 repr_val2 = *((i32*) &val2);

    if ((repr_val1 < 0) != (repr_val2 < 0))
    {
        // direct comparison for +0.0f and -0.0f
        return (val1 == val2);
    }

    return (abs(repr_val1 - repr_val2) <= max_ulps_diff);
}
