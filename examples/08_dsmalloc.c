#include <stdio.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

typedef struct person_t {
    const char *name;
    const char *lastname;
} *Person;

static Person new_person(const char *name, const char *lastname) {
    Person ret = Common_dsmalloc(struct person_t*);

    ret->name = name;
    ret->lastname = lastname;

    return ret;
}

int main() {
    Optional opt_person = Common_optional_with(new_person("John", "Doe"));

    defer({
        Common_optional_free_data(&opt_person);
        Common_optional_set_none(&opt_person);
    });

    if (Common_optional_is_some(&opt_person)) {
        Person john = Common_optional_unpack(&opt_person);
        printf("Hello %s %s\n", john->name, john->lastname);
    }
}