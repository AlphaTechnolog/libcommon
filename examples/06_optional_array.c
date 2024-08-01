// NOTE: this example is quite difficult to understand since we're creating a structure which
// will look somewhat similar to this one (as how could be typed in a ficticious language)
// `DynamicArray<*Optional<*{ name: String, lastname: String, friend: *Optional<{ name: String.... }> }>>`
// yes! recursivity inside Optional arrays lol.

#include <stdio.h>

#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include "../include/libcommon.h"

// if true, we'll create an infinite friends tree between the persons in the
// optional array, and we'll test how the program behaves that way.
#define TEST_WITH_INFINITE_RECURSIVITY
#define MAX_RECURSIVITY_LEVEL 5

typedef struct person_t {
    const char *name;
    const char *lastname;
    
    // ok here we're gonna include a recursive pointer just to add more
    // difficulty, but this will end with us having to stop using
    // Common_optional_array_free() but Common_optional_array_destroy()
    // since we're gonna have to manually free this element to be able
    // to also free the friend pointer.
    Optional *friend;
} *Person;

static Person new_person(const char *name, const char *lastname, Optional *friend) {
    Person person = Common_smalloc(sizeof(struct person_t));

    person->name = name;
    person->lastname = lastname;
    person->friend = friend;

    return person;
}

static void populate_array(OptionalArray persons_array) {
    Person john = new_person("John", "Doe", Common_optional_alloc_none());
    Person matt = new_person("Matt", "Doe", Common_optional_alloc_with(john));
    Person mario = new_person("Mario", "Doe", Common_optional_alloc_with(matt));

    // this will end in infinite recursivity since now john has as friends
    // everyone accordingly to its friends tree.

#ifdef TEST_WITH_INFINITE_RECURSIVITY
    Common_optional_set_data(john->friend, (void*) mario);
#endif

    Common_optional_array_append(persons_array, Common_optional_alloc_with(john));
    Common_optional_array_append(persons_array, Common_optional_alloc_none());
    Common_optional_array_append(persons_array, Common_optional_alloc_with(matt));
    Common_optional_array_append(persons_array, Common_optional_alloc_with(mario));
    Common_optional_array_append(persons_array, Common_optional_alloc_none());
}

static inline void repeat_char(int times, char el) {
    for (int i = 0; i < times; ++i)
        printf("%c", el);
}

static void print_friend(int level, Person person) {
    if (Common_optional_is_none(person->friend) || level >= MAX_RECURSIVITY_LEVEL) {
        return;
    }

    Person unpacked_friend = Common_optional_unpack(person->friend);
    repeat_char(level, ' ');

    printf(
        "-> Friend of %s: %s %s\n",
        person->name,
        unpacked_friend->name,
        unpacked_friend->lastname
    );

    if (level + 1 >= MAX_RECURSIVITY_LEVEL) {
        fprintf(stderr, "Warning: Exceeding maximum recursivity level in the friends tree\n");
        fprintf(stderr, "         Stopping printing friends.\n");
    }

    // recursively printing the friends
    print_friend(level + 1, unpacked_friend);
}

#define W_CONTINUE(i) \
    { \
        fprintf(stderr, "Warning: skipping index %ld: empty optional found\n", i + 1); \
        continue; \
    }

static void display_persons(OptionalArray persons_array) {
    Common_foreach(persons_array, Optional, opt_person) {
        if (Common_optional_is_none(opt_person)) W_CONTINUE(i);
        Person person = Common_optional_unpack(opt_person);
        printf("-> Person #%ld: %s %s\n", i + 1, person->name, person->lastname);
        print_friend(1, person);
    } Common_endforeach;
}

int main() {
    OptionalArray persons_array = Common_optional_array_init();

    // as noted previously, we'll have to manually free every friend, so we're gonna
    // end using Common_optional_array_destroy() to free the array->elements and the array
    // pointers, but we've to also free all the optionals.
    defer({
        Common_foreach(persons_array, Optional, opt_person) {
            if (Common_optional_is_some(opt_person)) {
                Person person = (Person) Common_optional_unpack(opt_person);

                // IMPORTANT NOTE: Here we set to none the optional, since at this moment
                // the friend could already be freed by another Common_optional_free() so
                // we've to tell Common_optional_free() that there's no valid data anyways
                // and it will just skip the data freeing and just free the optional data.
                Common_optional_set_none(person->friend);
                Common_optional_free(person->friend);
            }
            
            Common_optional_free(opt_person);
        } Common_endforeach;

        Common_optional_array_destroy(persons_array);
    });

    populate_array(persons_array);
    display_persons(persons_array);

    return 0;
}