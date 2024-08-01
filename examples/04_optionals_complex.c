#include <stdio.h>
#include <stdlib.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

enum Days {
    DAY_MONDAY,
    DAY_TUESDAY,
    DAY_WEDNESDAY,
    DAY_THURSDAY,
    DAY_FRIDAY,
    DAY_SATURDAY,
    DAY_SUNDAY,
};

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
    printf("Hello %s %s\n", person->name, person->lastname);
}

Optional hire_john_doe(enum Days day) {
    if (day == DAY_SUNDAY || day == DAY_SATURDAY) {
        return Common_optional_none();
    }

    return Common_optional_with((void*) new_person(
        (const char*) "John",
        (const char*) "Doe"
    ));
}

static void hiring_example(void) {
    printf("hiring_example()\n");

    Optional opt_john_doe = hire_john_doe(DAY_SUNDAY);

    Person worker = Common_optional_unpack_default(&opt_john_doe, &(struct person_t) {
        .name = "No",
        .lastname = "Worker available"
    });

    display_person(worker);

    if (Common_optional_is_some(&opt_john_doe)) {
        Person real_worker = Common_optional_unpack(&opt_john_doe);
        LCOMMON_FREE(real_worker);
    }
}

static void freeing_example(void) {
    printf("\nfreeing_example()\n");

    Optional opt_sample_worker = Common_optional_with(new_person("Example", "Worker"));

    defer({
        Common_optional_free_data(&opt_sample_worker);
        Common_optional_set_none(&opt_sample_worker);

        LCOMMON_ASSERT(
            Common_optional_is_none(&opt_sample_worker),
            "optional should've been marked as none"
        );
    });

    const Person worker = Common_optional_unpack(&opt_sample_worker);
    LCOMMON_ASSERT(worker != NULL, "worker should have data");

    display_person(worker);
}

static inline Optional obtain_array(void);

// define this constant to test when the optional gets null.
#ifndef WORKERS_LIST_OPTIONAL_TOGGLE
static inline Optional obtain_array(void) {
    return Common_optional_with(Common_dynamic_array_init());
}
#else
static inline Optional obtain_array(void) {
    return Common_optional_none();
}
#endif

static inline void display_persons(Optional *opt_workers_array) {
    LCOMMON_ASSERT(Common_optional_is_some(opt_workers_array), "display_persons() must receive a persons list");

    DynamicArray workers = Common_optional_unpack(opt_workers_array);

    Common_foreach(workers, struct person_t, worker) {
        display_person(worker);
    }};
}

static void deferred_usage_example(void) {
    printf("\ndeferred_usage_example()\n");

    Optional opt_workers_array = obtain_array();
    
    // unpack will throw an error if it's disabled
    DynamicArray workers = Common_optional_unpack(&opt_workers_array);

    // freeing every element in the array and also disabling
    // the optional by setting it to none.
    defer({
        Common_dynamic_array_free(workers);
        Common_optional_set_none(&opt_workers_array);
    });

    Common_dynamic_array_append(workers, new_person("First", "Person"));
    Common_dynamic_array_append(workers, new_person("Second", "Person"));

    display_persons(&opt_workers_array);
}

int main() {
    hiring_example();
    freeing_example();
    deferred_usage_example();
    return 0;
}