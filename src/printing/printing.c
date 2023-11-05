
#include <stdio.h>

#include <ustd/printing.h>

void print_range(const range *r, const char *fmt)
{
	for (size_t i = 0 ; i < r->length ; i++) {
		printf(fmt, *range_at(r, i));
	}
}

void print_new_line(void)
{
	puts("");
}

void print_c_string(const char *strz)
{
	printf("%s", strz);
}