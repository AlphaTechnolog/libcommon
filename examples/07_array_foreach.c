#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

static void dynamic_array_demo(void) {
    printf("dynamic_array_demo()\n");

    DynamicArray integer_array = Common_dynamic_array_init();

    defer({ Common_dynamic_array_destroy(integer_array); });

    int n = 4;
    Common_dynamic_array_append(integer_array, (void*) &n);

    Common_foreach(integer_array, int, cur, {
        printf("-> %d\n", *cur);
    });
}

static inline int streql(const char *a, const char *b) {
    return !strcmp(a, b);
}

static void optional_array_demo(void) {
    printf("\noptional_array_demo()\n");

    OptionalArray array = Common_optional_array_init();

    // `Common_optional_array_free()` is kind of interesting since it
    // frees every optional inside and also frees the data of every non null
    // optional inside the array, also frees the array itself.
    defer({ Common_optional_array_free(array); });

    char *allocated_string = Common_smalloc(strlen("John") + 1);
    snprintf(allocated_string, strlen("John") + 1, "%s", "John");
    LCOMMON_ASSERT(!streql(allocated_string, ""), "shouldn't be an empty string");

    Common_optional_array_append(array, Common_optional_alloc_none());
    Common_optional_array_append(array, Common_optional_alloc_with(allocated_string));

    Common_foreach(array, Optional, opt_element, {
        const char *string = Common_optional_unpack_default(opt_element, "no value");
        printf("-> %ld: %s\n", i + 1, string);
    });
}

int main() {
    dynamic_array_demo();
    optional_array_demo();

    return 0;
}