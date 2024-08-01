#include <stdio.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

int main() {
    char *string = Common_strmerge(", ", 2, "hello", "world");
    defer({ LCOMMON_FREE(string); });

    printf("string is %s\n", string);

    return 0;
}
