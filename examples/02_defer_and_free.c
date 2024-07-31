#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

#define S_GOODBYE "Goodbye!"

int main(void) {
    char *sample_string = Common_smalloc(strlen(S_GOODBYE) + 1);
    defer({ LCOMMON_FREE(sample_string); });

    memcpy(sample_string, S_GOODBYE, strlen(S_GOODBYE));
    sample_string[strlen(S_GOODBYE)] = '\0';
    puts(sample_string);

    return 0;
}