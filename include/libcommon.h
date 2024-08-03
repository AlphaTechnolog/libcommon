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
typedef int LCOMMON_BOOL;
#define LCOMMON_TRUE 1
#define LCOMMON_FALSE 0
#define LCOMMON_TERMINATOR (void*) 0

_LIBCOMMON_EXPORT LCOMMON_BOOL Common_is_true(int n);
_LIBCOMMON_EXPORT LCOMMON_BOOL Common_is_false(int n);

// allocation utilities

// malloc but already checks for null pointer and throws an error.
_LIBCOMMON_EXPORT void *Common_smalloc(size_t len);

// realloc but already checks for null pointer and throws an error.
_LIBCOMMON_EXPORT void *Common_srealloc(void *ptr, size_t len);

// same as `Common_smalloc` but already does sizeof() for you.
#define Common_dsmalloc(type) Common_smalloc(sizeof(type));

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

// frees a dynamic array but not the elements.
_LIBCOMMON_EXPORT void Common_dynamic_array_destroy(DynamicArray array);

// frees a dynamic array and its elements.
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

// creates an optional value by checking if it's null or not, if null
// Common_optional_none() will be called, else Common_optional_with() is the one
_LIBCOMMON_EXPORT Optional Common_optional_from(void *payload);

// creates an allocated optional value with actual data.
_LIBCOMMON_EXPORT Optional *Common_optional_alloc_with(void *data);

// creates an optional value with no data but allocated.
_LIBCOMMON_EXPORT Optional *Common_optional_alloc_none(void);

// creates an optional from (see Common_optional_from()) but allocated.
_LIBCOMMON_EXPORT Optional *Common_optional_alloc_from(void *payload);

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

// converts the given optional into a raw value, so if the optional is none
// it will return a raw NULL value, else, it will attempt to unpack the optional
// and then proceed to return the unpacked data.
_LIBCOMMON_EXPORT void *Common_optional_to_raw(Optional *optional);

// sets data on a given optional no matter if it's already used or not.
_LIBCOMMON_EXPORT void Common_optional_set_data(
    Optional *optional,
    void *data
);

// sets optional->is_none as true but keeps optional->data intact, this optional
// will be treated as if it's not containing data even if it's. In order to
// avoid this, you may be interested in seeing: `Common_optional_free_data()`
_LIBCOMMON_EXPORT void Common_optional_set_none(Optional *optional);

// frees the data if a given optional has something at optional->data
_LIBCOMMON_EXPORT void Common_optional_free_data(Optional *optional);

// frees an allocated Optional pointer without freeing the data.
_LIBCOMMON_EXPORT void Common_optional_destroy(Optional *optional);

// marks a given optional with or without data as a "none" optional.
// also frees the data if it's found that optional->data != NULL
// NOTE: Use this when you call Common_optional_alloc_with or Common_optional_alloc_none
_LIBCOMMON_EXPORT void Common_optional_free(Optional *optional);

// Optional specific dynamic arrays implementation

typedef struct optional_array_t {
    size_t len;
    size_t cap;
    Optional **elements;
} *OptionalArray;

// creates a new optional array (allocated).
_LIBCOMMON_EXPORT OptionalArray Common_optional_array_init(void);

// sets data into the N optional in the given optional array.
_LIBCOMMON_EXPORT void Common_optional_array_set_data_at(
    OptionalArray array,
    const unsigned int n,
    void *data
);

// marks the optional at N in the optional array as none
_LIBCOMMON_EXPORT void Common_optional_array_set_none_at(
    OptionalArray array,
    const unsigned int n
);

// frees a complete optional array, and also the allocated memory of every suboptional
// but not the allocated memory of the data inside the children's optionals.
_LIBCOMMON_EXPORT void Common_optional_array_destroy(OptionalArray array);

// frees a complete optional array and every optional inside
_LIBCOMMON_EXPORT void Common_optional_array_free(OptionalArray array);

// appends a new element to a given OptionalArray, requires an *Optional<void*>
// must be used with `Common_optional_alloc_with()`.
_LIBCOMMON_EXPORT void Common_optional_array_append(OptionalArray array, Optional *optional);

// macro to iterate through an Arrays

#define Common_foreach(array, type, variablename, body) \
    for (size_t i = 0; i < (array)->len; ++i) { \
        LCOMMON_ASSERT((array)->elements[i] != NULL, "should be able to obtain elements from a growable array"); \
        type *variablename = (type*) (array)->elements[i]; \
        body; \
    }

// strings helpers

// checks if a == b
_LIBCOMMON_EXPORT LCOMMON_BOOL Common_strql(const char *a, const char *b);

// counts the amount of characters present in a string
_LIBCOMMON_EXPORT size_t Common_strcount(const char *s);

// creates a new allocated string joining them all with the given separator. The caller is
// responsible for freeing its memory by calling LCOMMON_FREE() on it after usage.
_LIBCOMMON_EXPORT char *__private__Common_strmerge(const char *separator, const char *first, ...);
#define Common_strmerge(...) __private__Common_strmerge(__VA_ARGS__, LCOMMON_TERMINATOR)

// creates a new allocated string joining every element from the given dynamic array using
// a given separator. The called is responsible for freeing its allocated memory by calling
// LCOMMON_FREE() on it, after the desired usage.
_LIBCOMMON_EXPORT char *Common_strmerge_from_array(
    const char *separator,
    const DynamicArray dynamic_array
);

// given an OptionalArray<*const char>, this returns an allocated string which joins every
// Optional that's not a None value, the caller is responsible for freeing its allocated
// memory after the desired usage.
_LIBCOMMON_EXPORT char *Common_strmerge_from_optional_array(
    const char *separator,
    const OptionalArray optional_array
);

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
