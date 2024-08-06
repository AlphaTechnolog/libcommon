# Libcommon

libcommon is a simple C library which provides abstractions for dynamic arrays, optionals, defer statements, etc.

## Linking against libcommon

First you will have to build the library, this example assumes you have a folder (git initialised) with something like this:

```
src
  main.c
extern
```

so you will have to do something like this to build the project

```sh
cat << "EOF" > src/main.c
#include <stdio.h>
#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
#include <libcommon.h>

typedef struct person_t {
    const char *name;
    const char *lastname;
} *Person;

Person new_person(const char *name, const char *lastname) {
    Person self = Common_dsmalloc(Person);
    self->name = name;
    self->lastname = lastname;
    return self;
}

int main() {
    Optional opt_person = Common_optional_alloc_with((void*) new_person("John", "Doe"));
    defer({ Common_optional_free(opt_person); });

    if (Common_optional_is_some(opt_person)) {
        const Person john = Common_optional_unpack(opt_person);
        printf("Hello %s %s\n", john->name, john->lastname);
    }
}
EOF

cd extern
git submodule add https://github.com/alphatechnolog/libcommon libcommon
cd libcommon
make
cd ../../

cc -c -o main.o src/main.c -Iextern/libcommon/include -O3 -Werror -Wall
cc -o application main.o -Lextern/libcommon/lib -lcommon
./application
```

So this was an example of using libcommon's optionals and the defer statement provided by libcommon... take in mind that enabling
libcommon can be dangerous if you're targeting another compilers diff than gcc, defer atm is only supported for gcc due to macros
reasons, am still working on porting it for another compilers :(

## Documentation

Pretty WIP rn but you can take a look at the [examples](./examples) in the source code
