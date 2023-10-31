
#include <ustd/range.h>

range range_create_raw(size_t stride, size_t capacity, allocator alloc)
{
    return (range) {
            .stride = stride,
            .capacity = capacity,
            .alloc = alloc,
            .data = alloc.malloc(stride * capacity)
    };
}

range range_destroy(range r)
{
    r.stride = 0;
    r.capacity = 0;
    r.length = 0;

    r.alloc.free(r.data);
    r.data = NULL;
}

range range_set_elt(range r, size_t pos, byte element[static 1])
{
	if (pos >= r.length) {
		return r;
	}

	bytewise_copy(r.data + (pos * r.stride), element, r.stride);
	return r;
}