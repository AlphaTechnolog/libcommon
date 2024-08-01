#include <stdio.h>
#include <string.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

static inline LCOMMON_BOOL streql(const char *a, const char *b) {
    return !strcmp(a, b);
}

static Optional envvar(const char *variable) {
    char *result = getenv(variable);
    if (result == NULL || streql(result, "")) {
        return Common_optional_none();
    }

    return Common_optional_with(result);
}

static char *with_default(const char *username) {
    size_t size = strlen("/home/") + strlen(username) + 1;
    char *result = Common_smalloc(size);
    snprintf(result, size, "/home/%s", username);
    return result;
}

int main() {
    Optional opt_home = envvar("HOME");
    Optional opt_username = envvar("USER");

    if (Common_optional_is_none(&opt_username)) {
        fprintf(stderr, "unable to obtain username!\n");
        exit(1);
    }

    char *username = Common_optional_unpack(&opt_username);
    char *home = Common_optional_unpack_default(&opt_home, with_default(username));

    defer({
        if (Common_optional_is_none(&opt_home)) {
            LCOMMON_FREE(home);
        }
    });

    printf("home is %s\n", home);

    return 0;
}