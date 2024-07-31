#ifndef LIBCOMMON_H_
#define LIBCOMMON_H_

#include <stddef.h>
#include <stdlib.h>

#ifndef WITH_LIBCOMMON_DEFINITIONS
#define _LIBCOMMON_EXPORT extern
#else
#define _LIBCOMMON_EXPORT
#endif

// assertion
#define LCOMMON_ASSERT(condition, reason) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Assertion '%s' failed at line %d due to: %s\n", #condition, __LINE__, reason); \
            abort(); \
        } \
    } while (0)

// helper data types
#define LCOMMON_TRUE 1
#define LCOMMON_FALSE 0

_LIBCOMMON_EXPORT int Common_is_true(int n);
_LIBCOMMON_EXPORT int Common_is_false(int n);

// allocation utilities

// malloc but already checks for null pointer and throws an error.
_LIBCOMMON_EXPORT void *Common_smalloc(size_t len);

// realloc but already checks for null pointer and throws an error.
_LIBCOMMON_EXPORT void *Common_srealloc(void *ptr, size_t len);

// helper for freeing data and then set it as null.
#define LCOMMON_FREE(x) \
    free((x)); \
    (x) = NULL;

// dynamic arrays
typedef struct dynamic_array_t {
    size_t cap;
    size_t len;
    void **elements;
} *DynamicArray;

// initialises a new dynamic array structure.
_LIBCOMMON_EXPORT DynamicArray Common_dynamic_array_init(void);

// append to a dynamic array x element.
_LIBCOMMON_EXPORT void Common_dynamic_array_append(DynamicArray array, void *element);

// frees a dynamic array.
_LIBCOMMON_EXPORT void Common_dynamic_array_free(DynamicArray array);

// optionals (util for avoiding usage of NULL)
typedef struct optional_t {
    void *data;
    int is_none;
} Optional;

// creates an optional value with actual data.
_LIBCOMMON_EXPORT Optional Common_optional_with(void *data);

// creates an optional value with no data (similar to null).
_LIBCOMMON_EXPORT Optional Common_optional_none(void);

// checks if the given optional is none or not.
_LIBCOMMON_EXPORT int Common_optional_is_none(Optional *optional);

// checks if the given optional contains data.
_LIBCOMMON_EXPORT int Common_optional_is_some(Optional *optional);

// attempts to unpack data on the given optional but throws an error if
// `Common_optional_is_none(optional)` returns 1.
_LIBCOMMON_EXPORT void *Common_optional_unpack(Optional *optional);

// attempts to unpack data on the given optional but returns a
// default value if the optional is marked as none.
_LIBCOMMON_EXPORT void *Common_optional_unpack_default(
    Optional *optional,
    void *default_value
);

// sets data on a given optional no matter if it's already used or not.
_LIBCOMMON_EXPORT void Common_optional_set_data(
    Optional *optional,
    void *data
);

// sets optional->is_none as true but keeps optional->data intact, this optional
// will be treated as if it's not containing data even if it's. In order to
// avoid this, you may be interested in seeing: `Common_optional_free()`
_LIBCOMMON_EXPORT void Common_optional_set_none(Optional *optional);

// marks a given optional with or without data as a "none" optional.
// also frees the data if it's found that optional->data != NULL
_LIBCOMMON_EXPORT void Common_optional_free(Optional *optional);

// defer macro-based implementation
// thanks to https://gist.github.com/baruch/f005ce51e9c5bd5c1897ab24ea1ecf3b
#ifdef LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#ifdef __clang__
// clang not supported
#error libcommon's defer's not supporting clang atm
#endif
#define defer_merge(a,b) a##b
#define defer_varname(a) defer_merge(defer_scopevar_, a)
#define defer_funcname(a) defer_merge(defer_scopefunc_, a)
#define defer(BLOCK) void defer_funcname(__LINE__)(__attribute__((unused)) int *a) BLOCK; __attribute__((cleanup(defer_funcname(__LINE__)))) int defer_varname(__LINE__)
#endif

#endif