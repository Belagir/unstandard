
#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <ustd/range.h>

#define STRING_TYPE const char

#define string range_of(STRING_TYPE)
#define string_static(__length) range_static(__length, STRING_TYPE)

#define string_create_static(__s) range_static_create(count_of(((string_base_type[]) {__s})) - 1, string_base_type, __s)

#define string_val(__s, __i) range_val(__s, __i, STRING_TYPE)

#define string_destroy(__alloc, __s) range_dynamic_destroy(__alloc, __s)

#endif
