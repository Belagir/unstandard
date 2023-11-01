
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
    i32 array[10] = { 1, 2, 3, 4, 0 };

    range *r = range_dynamic_from(make_system_allocator(), sizeof(i32), 10, 4, array);
    range *r2;
    range *r3;

    (void) range_insert(r, 0,  &(i32) { 42 });
    (void) range_insert(r, 3,  &(i32) { 43 });
    (void) range_insert(r, 0,  &(i32) { 41 });
    (void) range_push_front(r, &(i32) { 40 });
    (void) range_push_back(r,  &(i32) { 44 });

    (void) range_remove(r, 3);

    r2 = range_copy_of(make_system_allocator(), r);
    r3 = range_concat(make_system_allocator(), r, r2);
    range_dynamic_destroy(make_system_allocator(), r);

    for (size_t i = 0 ; i < r2->length ; i++) {
        printf("%d ", range_at(r2, i, i32));
    }
    printf("\n");

    range_dynamic_destroy(make_system_allocator(), r2);

    for (size_t i = 0 ; i < r3->length ; i++) {
        printf("%d ", range_at(r3, i, i32));
    }
    printf("\n");

    range_dynamic_destroy(make_system_allocator(), r3);

    return 0;
}
