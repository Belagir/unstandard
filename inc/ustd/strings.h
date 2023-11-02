
#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <ustd/range.h>

#define STRING_TYPE const char

#define string range
#define string_static(__length) range_static(__length, const char)

#define string_from(__s) range_static_create_fit(const char, __s)

#define string_val(__s, __i) range_val(__s, __i, const char)

#define string_destroy(__alloc, __s) range_dynamic_destroy(__alloc, __s)

#endif
