
#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <ustd/range.h>

typedef const char string_base_type;

typedef range_of(string_base_type) string;
#define string_static(__length) range_static(__length, string_base_type)

#define string_create_static(__s) range_static_create_fit(string_base_type, __s)

#define string_from_static(__s) ((string *) &(string_static(count_of(((string_base_type[]) {__s})))) string_create_static(__s))

#define string_val(__s, __i) range_val(__s, __i, string_base_type)

#define string_destroy(__alloc, __s) range_dynamic_destroy(__alloc, (range *) __s)

i32 string_cmp(const string *str1, const string *str2);

#endif
