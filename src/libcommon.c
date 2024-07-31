#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WITH_LIBCOMMON_DEFINITIONS
#include "../include/libcommon.h"

static inline void die(const char *prefix) {
    perror(prefix);
    exit(1);
}

void *Common_smalloc(size_t len) {
    void *ptr;
    if (!(ptr = malloc(len)))
        die("malloc");

    return ptr;
}

void *Common_srealloc(void *ptr, size_t len) {
    void *ret;
    if (!(ret = realloc(ptr, len)))
        die("realloc");

    return ret;
}

int Common_is_true(int n) {
    return n == LCOMMON_TRUE;
}

int Common_is_false(int n) {
    return n == LCOMMON_FALSE;
}

DynamicArray Common_dynamic_array_init(void) {
    DynamicArray ret = Common_smalloc(sizeof(struct dynamic_array_t));

    ret->cap = 10;
    ret->len = 0;
    ret->elements = Common_smalloc(sizeof(void*) * ret->cap);

    return ret;
}

void Common_dynamic_array_append(DynamicArray array, void *element) {
    array->elements[array->len++] = element;
    if (array->len >= array->cap) {
        array->cap *= 2;
        array->elements = Common_srealloc(array->elements, sizeof(void*) * array->cap);
    }
}

void Common_dynamic_array_free(DynamicArray array) {
    for (size_t i = 0; i < array->len; ++i) {
        LCOMMON_FREE(array->elements[i]);
    }

    LCOMMON_FREE(array->elements);
    LCOMMON_FREE(array);
}

Optional Common_optional_with(void *data) {
    return (Optional) {
        .data = data,
        .is_none = LCOMMON_FALSE
    };
}

Optional *Common_optional_alloc_with(void *data) {
    Optional *opt = Common_smalloc(sizeof(struct optional_t));

    memcpy(
        (void*) opt,
        (const void*) &(struct optional_t) {data, LCOMMON_FALSE},
        sizeof(struct optional_t)
    );

    return opt;
}

Optional Common_optional_none(void) {
    return (Optional) {
        .data = NULL,
        .is_none = LCOMMON_TRUE
    };
}

Optional *Common_optional_alloc_none(void) {
    Optional *opt = Common_smalloc(sizeof(struct optional_t));

    memcpy(
        (void*) opt,
        (const void*) &(struct optional_t) {NULL, LCOMMON_TRUE},
        sizeof(struct optional_t)
    );

    return opt;
}

int Common_optional_is_none(Optional *optional) {
    return Common_is_true(optional->is_none);
}

int Common_optional_is_some(Optional *optional) {
    return !Common_optional_is_none(optional);
}

void *Common_optional_unpack(Optional *optional) {
    LCOMMON_ASSERT(Common_optional_is_some(optional), "given optional should've data");
    return optional->data;
}

void *Common_optional_unpack_default(Optional *optional, void *default_value) {
    if (Common_optional_is_none(optional)) {
        return default_value;
    }

    return optional->data;
}

void Common_optional_set_data(Optional *optional, void *data) {
    optional->data = data;

    if (Common_optional_is_none(optional)) {
        optional->is_none = LCOMMON_FALSE;
    }
}

void Common_optional_set_none(Optional *optional) {
    optional->is_none = LCOMMON_TRUE;
}

void Common_optional_free_data(Optional *optional) {
    if (Common_optional_is_some(optional)) {
        if (optional->data != NULL) {
            LCOMMON_FREE(optional->data);
        }
    }
}

void Common_optional_free(Optional *optional) {
    Common_optional_free_data(optional);
    Common_optional_set_none(optional);
    LCOMMON_FREE(optional);
}