
#include <ustd/common.h>

// -------------------------------------------------------------------------------------------------
bool character_is_num(char c)
{
    return (c >= '0') && (c <= '9');
}

// -------------------------------------------------------------------------------------------------
i32 character_compare(const void *lhs, const void *rhs)
{
    char c_lhs = *(char *) lhs;
    char c_rhs = *(char *) rhs;

    return (c_lhs > c_rhs) - (c_lhs < c_rhs);
}

// -------------------------------------------------------------------------------------------------
size_t c_string_length(const char *str, size_t limit, bool keep_terminator)
{
    size_t str_length = 0u;

    if (!str) {
        return 0u;
    }

    while ((str[str_length] != '\0') && (str_length < limit)) {
        str_length += 1;
    }

    str_length += (size_t) keep_terminator;

    return str_length;
}
