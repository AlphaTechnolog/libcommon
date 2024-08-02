#include <stdio.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

typedef struct person_t {
    const char *name;
    const char *lastname;
} *Person;

static Person new_person(const char *name, const char *lastname) {
    Person person = Common_smalloc(sizeof(struct person_t));

    person->name = name;
    person->lastname = lastname;

    return person;
}

// expects something similar as how in others langs you would have to type like this
// `DynamicArray<*Optional<*struct person_t>>`.
static void print_persons(DynamicArray persons) {
    Common_foreach(persons, Optional, opt_person, {
        if (Common_optional_is_none(opt_person)) {
            fprintf(stderr, "Warning: Skipping person at index: %ld\n", i + 1);
            continue;
        }

        const Person person = Common_optional_unpack(opt_person);
        LCOMMON_ASSERT(person != NULL, "person should've data");

        printf("Person %ld: %s %s\n", i + 1, person->name, person->lastname);
    });
}

// in this example we're gonna use DynamicArray with Optionals, but see 05_optional_array
// for a simplified version which uses an OptionalArray
int main() {
    // you'll have to think of this in something like DynamicArray<Optional<Persons>>
    DynamicArray persons = Common_dynamic_array_init();

    defer({
        Common_foreach(persons, Optional, opt_person, {
            // here Common_optional_free_data simplifies lots of things, because else
            // we would have to also unpack the data and then free it manually...
            // we could also've used Common_optional_free() which will not only free the
            // unpacked data but also the allocated optional, but then we would have to
            // use LCOMMON_FREE() instead of Common_dynamic_array_free to free the array because
            // Common_dynamic_array_free() also frees every element in the list by calling
            // LCOMMON_FREE() on them.
            if (Common_optional_is_some(opt_person)) {
                Common_optional_free_data(opt_person);
                Common_optional_set_none(opt_person);
            }
        });

        Common_dynamic_array_free(persons);
    });

    char *person_names[] = {"John", "Patrick"};

    for (int i = 0; i < 2; ++i) {
        const char *person_name = person_names[i];
        Common_dynamic_array_append(persons, Common_optional_alloc_with(
            new_person(person_name, "Doe")
        ));
    }

    Common_dynamic_array_append(persons, Common_optional_alloc_none());

    Common_dynamic_array_append(persons, Common_optional_alloc_with(
        new_person("Sam", "Doe")
    ));

    print_persons(persons);

    return 0;
}