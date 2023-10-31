
#include <ustd/allocation.h>
#include <ustd/logging.h>
#include <ustd/math.h>
#include <ustd/math3d.h>
#include <ustd/range.h>
#include <ustd/res.h>
#include <ustd/sorting.h>
#include <ustd/testutilities.h>

int main(int argc, char const *argv[])
{
#ifdef UNITTESTING
    quaternion_execute_unittests();
    sorted_array_execute_unittests();
#endif
    range *r = range_static_create(10, i32);

    (void) range_insert(r, 0,  &(i32) { 42 });
    (void) range_insert(r, 3,  &(i32) { 43 });
    (void) range_insert(r, 0,  &(i32) { 41 });
    (void) range_push_front(r, &(i32) { 40 });
    (void) range_push_back(r,  &(i32) { 44 });

    (void) range_remove(r, 3);

    for (size_t i = 0 ; i < r->length ; i++) {
        printf("%d ", range_at(r, i, i32));
    }
    printf("\n");

    range_front(r, i32) = 87;
    printf("%d, %d\n", range_front(r, i32), range_back(r, i32));

    return 0;
}
