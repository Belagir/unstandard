
#include <ustd/allocation.h>

#include <stdlib.h>

static void *system_malloc(allocator alloc, size_t nb_bytes);
static void system_free(allocator alloc, void *ptr);

allocator make_system_allocator(void)
{
	return (allocator) { .malloc = &system_malloc, .free = &system_free, .allocator_data = nullptr };
}

static void *system_malloc(allocator alloc, size_t nb_bytes)
{
	(void) alloc;
	return malloc(nb_bytes);
}

static void system_free(allocator alloc, void *ptr)
{
	(void) alloc;
	free(ptr);
}
