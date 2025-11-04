

#ifndef UNSTANDARD_PATH_H__
#define UNSTANDARD_PATH_H__

#include "array.h"

#define PATH_DELIM '/'

#define PATH ARRAY(char)

#define path_get(path_, index_, out_value_) \
        array_get(path_, index_, out_value_)

#define path_length(path_) \
        array_length(path_)

#define path_capacity(path_) \
        array_capacity(path_)

PATH path_from_cstring(struct allocator alloc, const char *path_cstring, char delimiter, size_t limit);
void path_destroy(struct allocator alloc, PATH *path);

void path_clear(PATH path);
void path_ensure_capacity(struct allocator alloc, PATH *path, size_t additional_capacity);

void path_up(PATH path);
void path_append(PATH path, const char *appended);
void path_prepend(PATH path, const char *prefix);

#ifdef UNITTESTING
void path_execute_unittests(void);
#endif

#endif
