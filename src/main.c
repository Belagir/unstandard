
#include <ustd/allocation.h>
#include <ustd/logging.h>
#include <ustd/math.h>
#include <ustd/math3d.h>
#include <ustd/range.h>
#include <ustd/res.h>
#include <ustd/sorting.h>
#include <ustd/testutilities.h>
#include <ustd/tree.h>


int main(int argc, char const *argv[])
{
#ifdef UNITTESTING
    quaternion_execute_unittests();
    heapsort_execute_unittests();
    sorted_range_execute_unittests();
    range_experimental_execute_unittests();
    ttree_execute_unittests();
#endif
}
