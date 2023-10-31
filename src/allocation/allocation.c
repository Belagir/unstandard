
#include <ustd/allocation.h>

#include <stdlib.h>

allocator allocator_system(void)
{
	return (allocator) { .malloc = &malloc, .free = &free };
}
