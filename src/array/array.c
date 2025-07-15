
#include <ustd_impl/array_impl.h>

#define UNITTESTING
#ifdef UNITTESTING
#include <ustd/testutilities.h>
#endif




// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
struct array_impl *array_impl_of(void *some_array)
{
    return CONTAINER_OF(some_array, struct array_impl, data);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------

void *array_create(allocator alloc, u32 size_element, size_t nb_elements_max)
{
    struct array_impl *new_array = nullptr;

    if (nb_elements_max == 0) {
        return nullptr;
    }

    new_array = alloc.malloc(alloc, sizeof(*new_array) + (size_element * nb_elements_max));

    new_array->capacity = nb_elements_max;
    new_array->length = 0;
    new_array->stride = size_element;

    return new_array->data;
}

// -------------------------------------------------------------------------------------------------

void array_destroy(allocator alloc, void **array)
{
    struct array_impl *target = nullptr;

    if (!array || !*array) {
        return;
    }

    target = array_impl_of(*array);

    alloc.free(alloc, target);
    *array = nullptr;
}

// -------------------------------------------------------------------------------------------------

bool array_insert_value(void *array, size_t index, const void *to_value)
{
    struct array_impl *target = nullptr;
    size_t insertion_byte_pos = 0u;

    if (!array || !to_value) {
        return false;
    }

    target = array_impl_of(array);

    if (target->length >= target->capacity) {
        return false;
    }

    if (index > target->length) {
        index = target->length;
    }

    insertion_byte_pos = index * target->stride;

    for (size_t i = (target->length*target->stride) ; i > insertion_byte_pos ; i--) {
        target->data[i+target->stride] = target->data[i];
    }

    bytewise_copy(target->data + insertion_byte_pos, to_value, target->stride);

    return true;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#ifdef UNITTESTING

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(array_i32_insertion,
        {
            size_t capacity;
            size_t existing_length;
            size_t insert_at;
            i32 inserted_val;

            bool expect_insertion;
        },
        {
            i32 *array = array_create(make_system_allocator(), sizeof(*array), data->capacity);
            array_impl_of(array)->length = data->existing_length;

            bool success = array_insert_value(array, data->insert_at, &data->inserted_val);
            tst_assert_equal(success, data->expect_insertion, "insertion success of %d");

            if (success) {
                tst_assert_equal(data->inserted_val, array[data->insert_at], "value in array of %d");
                tst_assert_equal(data->existing_length+1, array_impl_of(array)->length, "length of %ld");
            } else {
                tst_assert_equal(data->existing_length, array_impl_of(array)->length, "length of %ld");
            }

            array_destroy(make_system_allocator(), (void **) &array);
        }
)

// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_CASE(array_i32_insertion_base, array_i32_insertion,
        .capacity = 24,
        .existing_length = 12,
        .insert_at = 4,
        .inserted_val = 42,
        .expect_insertion = true,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void array_execute_unittests(void)
{
    tst_run_test_case(array_i32_insertion_base);
}
#endif
