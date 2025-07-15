
#include <ustd_impl/array_impl.h>

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

    return &(new_array->data);
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

bool array_insert_value(void *array, size_t index, const void *value)
{
    struct array_impl *target = nullptr;
    size_t insertion_byte_pos = 0u;

    if (!array || !value) {
        return false;
    }

    target = array_impl_of(array);

    if ((target->length >= target->capacity) || (index > target->length)) {
        return false;
    }

    insertion_byte_pos = index * target->stride;

    for (size_t i = (target->length*target->stride) ; i >= insertion_byte_pos ; i--) {
        target->data[i+target->stride] = target->data[i];
    }

    bytewise_copy(target->data + insertion_byte_pos, value, target->stride);
    target->length += 1;

    return true;
}

// -------------------------------------------------------------------------------------------------

bool array_push(void *array, const void *value)
{
    struct array_impl *target = array_impl_of(array);
    return array_insert_value(array, target->length, value);
}

// -------------------------------------------------------------------------------------------------

bool array_remove(void *array, size_t index)
{
    struct array_impl *target = nullptr;

    if (!array) {
        return false;
    }

    target = array_impl_of(array);

    if (index >= target->length) {
        return false;
    }

    target->length -= 1;
    for (size_t i = (index * target->stride) ; i < (target->length * target->stride) ; i++) {
        target->data[i] = target->data[i + target->stride];
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

bool array_pop(void *array)
{
    struct array_impl *target = nullptr;

    if (!array) {
        return false;
    }

    target = array_impl_of(array);
    return array_remove(array, target->length-1);
}

// -------------------------------------------------------------------------------------------------

void array_clear(void *array)
{
    struct array_impl *target = nullptr;

    if (!array) {
        return;
    }

    target = array_impl_of(array);
    target->length = 0;
}

// -------------------------------------------------------------------------------------------------

bool array_get(void *array, size_t index, void *out_value)
{
    struct array_impl *target = nullptr;

    if (!array) {
        return false;
    }

    target = array_impl_of(array);

    if (index >= target->length) {
        return false;
    }

    if (out_value) {
        bytewise_copy(out_value,
                target->data + (index*target->stride),
                target->stride);
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

size_t array_length(const void *array)
{
    return array_impl_of((void *) array)->length;
}

// -------------------------------------------------------------------------------------------------

size_t array_capacity(const void *array)
{
    return array_impl_of((void *) array)->capacity;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#if 1
// #ifdef UNITTESTING

#include <ustd/testutilities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(array_i32_insertion,
        {
            size_t capacity;
            size_t existing_length;
            size_t insert_at;
            i32 inserted_val;

            i32 *starting_content;
            i32 *expected_content;

            bool expect_insertion;
        },
        {
            i32 *array = array_create(make_system_allocator(), sizeof(*array), data->capacity);
            array_impl_of(array)->length = data->existing_length;
            for (size_t i = 0 ; i < data->existing_length ; i++) {
                array[i] = data->starting_content[i];
            }
            for (size_t i = data->existing_length ; i < data->capacity ; i++) {
                array[i] = 0;
            }

            bool success = array_insert_value(array, data->insert_at, &data->inserted_val);
            tst_assert_equal(success, data->expect_insertion, "insertion success of %d");

            if (success) {
                tst_assert_equal(data->inserted_val, array[data->insert_at], "value in array of %d");
                tst_assert_equal(data->existing_length+1, array_impl_of(array)->length, "length of %ld");
            } else {
                tst_assert_equal(data->existing_length, array_impl_of(array)->length, "length of %ld");
            }

            for (size_t i = 0 ; i < data->capacity ; i++) {
                tst_assert_equal_ext(data->expected_content[i], array[i], "%d", "at index %d", i);
            }
            array_destroy(make_system_allocator(), (void **) &array);
        }
)

// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_CASE(array_i32_insertion_base, array_i32_insertion,
        .capacity = 12,
        .existing_length = 10,
        .insert_at = 4,
        .inserted_val = 42,

        .starting_content = (i32[12]) { 1, 2, 3, 4, 5, 6, 8, 9, 0, 0 },
        .expected_content = (i32[12]) { 1, 2, 3, 4, 42, 5, 6, 8, 9, 0 },

        .expect_insertion = true,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_SCENARIO(array_i32_removal,
        {
            size_t capacity;
            size_t existing_length;
            size_t remove_at;

            i32 *starting_content;
            i32 *expected_content;

            bool expect_removal;
        },
        {
            i32 *array = array_create(make_system_allocator(), sizeof(*array), data->capacity);

            array_impl_of(array)->length = data->existing_length;
            for (size_t i = 0 ; i < data->existing_length ; i++) {
                array[i] = data->starting_content[i];
            }
            for (size_t i = data->existing_length ; i < data->capacity ; i++) {
                array[i] = 0;
            }
            bool success = array_remove(array, data->remove_at);
            tst_assert_equal(data->expect_removal, success, "removal success of %d");


            if (success) {
                tst_assert_equal(data->existing_length-1, array_impl_of(array)->length, "length of %ld");
            } else {
                tst_assert_equal(data->existing_length, array_impl_of(array)->length, "length of %ld");
            }

            for (size_t i = 0 ; i < data->capacity ; i++) {
                tst_assert_equal_ext(data->expected_content[i], array[i], "%d", "at index %d", i);
            }

            array_destroy(make_system_allocator(), (void **) &array);
        }
)

// -------------------------------------------------------------------------------------------------

tst_CREATE_TEST_CASE(array_i32_removal_base, array_i32_removal,
        .capacity = 10,
        .existing_length = 8,
        .remove_at = 4,

        .starting_content = (i32[10]) { 1, 2, 3, 4, 5, 6, 7, 8, 0, 0 },
        .expected_content = (i32[10]) { 1, 2, 3, 4, 6, 7, 8, 8, 0, 0 },

        .expect_removal = true,
)
tst_CREATE_TEST_CASE(array_i32_removal_fail, array_i32_removal,
        .capacity = 10,
        .existing_length = 6,
        .remove_at = 8,

        .starting_content = (i32[10]) { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0 },
        .expected_content = (i32[10]) { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0 },

        .expect_removal = false,
)
tst_CREATE_TEST_CASE(array_i32_removal_last, array_i32_removal,
        .capacity = 10,
        .existing_length = 6,
        .remove_at = 5,

        .starting_content = (i32[10]) { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0 },
        .expected_content = (i32[10]) { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0 },

        .expect_removal = true,
)
tst_CREATE_TEST_CASE(array_i32_removal_one, array_i32_removal,
        .capacity = 10,
        .existing_length = 1,
        .remove_at = 0,

        .starting_content = (i32[10]) { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        .expected_content = (i32[10]) { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

        .expect_removal = true,
)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void array_execute_unittests(void)
{
    tst_run_test_case(array_i32_insertion_base);
    tst_run_test_case(array_i32_removal_base);
    tst_run_test_case(array_i32_removal_fail);
    tst_run_test_case(array_i32_removal_last);
    tst_run_test_case(array_i32_removal_one);
}
#endif
