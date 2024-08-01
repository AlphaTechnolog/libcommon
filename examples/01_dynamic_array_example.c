#include <stdio.h>
#include <string.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

typedef struct person_t {
    const char *name;
    const char *lastname;
} *Person;

static Person new_person(const char *name, const char *lastname) {
    Person ret = Common_smalloc(sizeof(struct person_t));

    ret->name = name;
    ret->lastname = lastname;

    return ret;
}

static inline void display_person(Person person) {
    printf("-> %s %s\n", person->name, person->lastname);
}

int main() {
    printf("Persons demonstration\n");

    DynamicArray persons = Common_dynamic_array_init();
    defer({ Common_dynamic_array_free(persons); });

    Common_dynamic_array_append(persons, new_person("John", "Doe"));
    Common_dynamic_array_append(persons, new_person("John2", "Doe2"));

    Common_foreach(persons, struct person_t, person) {
        printf("-> %s %s\n", person->name, person->lastname);
    }};

    return 0;
}