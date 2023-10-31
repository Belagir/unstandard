
#include <ustd/range.h>

/**
 * @brief
 *
 * @param r
 * @param index
 * @param value
 */
static void range_set(range *r, size_t index, void *value);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
bool range_insert(range *r, size_t index, void *value)
{
    if ((r->length == r->capacity) || (index >= r->capacity)) {
        return false;
    }

    index = min(index, r->length);

    for (size_t i = r->length ; i > index ; i--) {
        bytewise_copy(r->data + (i * r->stride), r->data + (i - 1) * r->stride, r->stride);
    }

    range_set(r, index, value);
    r->length += 1;
}

// -------------------------------------------------------------------------------------------------
bool range_push_back(range *r, void *value)
{
    return range_insert(r, r->length, value);
}

// -------------------------------------------------------------------------------------------------
bool range_push_front(range *r, void *value)
{
    return range_insert(r, 0, value);
}

// -------------------------------------------------------------------------------------------------
void range_clear(range *r)
{
    r->length = 0;
}

// -------------------------------------------------------------------------------------------------
bool range_remove(range *r, size_t index)
{
    if (index >= r->length) {
        return false;
    }

    for (size_t i = index + 1 ; i < r->length ; i++) {
        bytewise_copy(r->data + (i - 1) * r->stride, r->data + (i * r->stride), r->stride);
    }
    r->length -= 1;
}

// -------------------------------------------------------------------------------------------------
bool range_pop_back(range *r)
{
    if (r->length == 0) {
        return false;
    }

    return range_remove(r, r->length - 1);
}

// -------------------------------------------------------------------------------------------------
bool range_pop_front(range *r)
{
    return range_remove(r, 0);
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_create(allocator alloc, size_t size_element, size_t nb_elements_max)
{
    range *r = alloc.malloc(alloc, sizeof(*r) + (size_element * nb_elements_max));
    *r = (range) { .stride = size_element, .capacity = nb_elements_max, .length = 0 };

    return r;
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_destroy(allocator alloc, range *r)
{
    alloc.free(alloc, r);
}

// -------------------------------------------------------------------------------------------------
range *range_dynamic_from(allocator alloc, size_t size_element, size_t nb_elements_max, size_t nb_elements, void *array)
{
    range *r = range_dynamic_create(alloc, size_element, nb_elements_max);
    bytewise_copy(r->data, array, nb_elements * size_element);
    r->length = nb_elements;

    return r;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
static void range_set(range *r, size_t index, void *value)
{
    bytewise_copy(r->data + (index * r->stride), value, r->stride);
}
