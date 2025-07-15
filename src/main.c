
#include <ustd/allocation.h>
#include <ustd/array.h>
#include <ustd/common.h>
#include <ustd/logging.h>
#include <ustd/math.h>
#include <ustd/math3d.h>
#include <ustd/range.h>
#include <ustd/res.h>
#include <ustd/sorting.h>
#include <ustd/testutilities.h>
#include <ustd/tree.h>

int main(void)
{
#ifdef UNITTESTING
    quaternion_execute_unittests();
    heapsort_execute_unittests();
    sorted_range_execute_unittests();
    ttree_execute_unittests();
    range_experimental_execute_unittests();
    array_execute_unittests();

#endif
    return 0;
}
