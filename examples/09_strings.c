#include <stdio.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

static void simple_demo(void) {
    char *string = Common_strmerge(", ", "hello", "world");
    defer({ LCOMMON_FREE(string); });
    printf("string is %s\n", string);
}

static void dynamic_array_demo(void) {
    DynamicArray array = Common_dynamic_array_init();
    defer({ Common_dynamic_array_destroy(array); });

    Common_dynamic_array_append(array, (void*) (char*) "hello");
    Common_dynamic_array_append(array, (void*) (char*) "world");

    char *result_string = Common_strmerge_from_array(
        (const char*) ", ",
        (const DynamicArray) array
    );

    defer({ LCOMMON_FREE(result_string); });

    printf("dynamic array string is %s\n", result_string);
}

#define APPEND_OPTIONAL(arr, content) \
    Common_optional_array_append(arr, Common_optional_alloc_with((void*) (char*) content))

#define APPEND_OPTIONAL_NONE(arr) \
    Common_optional_array_append(arr, Common_optional_alloc_none());

static void optional_array_demo(void) {
    OptionalArray array = Common_optional_array_init();
    defer({ Common_optional_array_destroy(array); });\

    APPEND_OPTIONAL(array, "first item");
    APPEND_OPTIONAL_NONE(array);
    APPEND_OPTIONAL(array, "second item");

    char *result_string = Common_strmerge_from_optional_array(
        (const char*) ", ",
        (const OptionalArray) array
    );

    defer({ LCOMMON_FREE(result_string); });

    printf("optional array string is %s\n", result_string);
}

int main() {
    simple_demo();
    dynamic_array_demo();
    optional_array_demo();
    return 0;
}
