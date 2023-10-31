
#include <ustd/allocation.h>

#include "ouroboros/ouroboros.h"

static void *static_alloc_malloc(allocator alloc, size_t nb_bytes);
static void static_alloc_free(allocator alloc, void *ptr);

allocator make_static_allocator(byte *mem, size_t length)
{
	ouroboros_t *real_allocator = orbr_create(mem, length);

	return (allocator) { .malloc = &static_alloc_malloc, .free = static_alloc_free, .allocator_data = real_allocator };
}

static void *static_alloc_malloc(allocator alloc, size_t nb_bytes)
{
	return orbr_alloc(alloc.allocator_data, nb_bytes);
}

static void static_alloc_free(allocator alloc, void *ptr)
{
	orbr_free(alloc.allocator_data, ptr);
}
