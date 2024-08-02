#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define WITH_LIBCOMMON_DEFINITIONS
#define LIBCOMMON_ENABLE_EXPERIMENTAL_DEFER
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

void Common_dynamic_array_destroy(DynamicArray array) {
    LCOMMON_FREE(array->elements);
    LCOMMON_FREE(array);
}

void Common_dynamic_array_free(DynamicArray array) {
    for (size_t i = 0; i < array->len; ++i) {
        LCOMMON_FREE(array->elements[i]);
    }
    
    Common_dynamic_array_destroy(array);
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

Optional Common_optional_from(void *payload) {
    return payload == NULL
        ? Common_optional_none()
        : Common_optional_with(payload);
}

Optional *Common_optional_alloc_from(void *payload) {
    Optional *self = Common_dsmalloc(Optional);
    Optional opt = Common_optional_from(payload);

    memcpy(
        (void*) self,
        (const void*) &opt,
        sizeof(Optional)
    );

    memset((void*) &opt, 0, sizeof(struct optional_t));

    return self;
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

void *Common_optional_to_raw(Optional *optional) {
    return Common_optional_is_some(optional)
        ? Common_optional_unpack(optional)
        : NULL;
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

void Common_optional_destroy(Optional *optional) {
    if (Common_optional_is_some(optional)) {
        Common_optional_set_none(optional);
    }

    LCOMMON_FREE(optional);
}

void Common_optional_free(Optional *optional) {
    Common_optional_free_data(optional);
    Common_optional_destroy(optional);
}

OptionalArray Common_optional_array_init(void) {
    OptionalArray ret = Common_smalloc(sizeof(struct optional_array_t));

    ret->cap = 10;
    ret->len = 0;
    ret->elements = Common_smalloc(sizeof(struct optional_t*) * ret->cap);

    return ret;
}

void Common_optional_array_set_data_at(
    OptionalArray array,
    const unsigned int n,
    void *data
) {
    Common_foreach(array, Optional, cur, {
        if (i == n) {
            Common_optional_set_data(cur, data);
            break;
        }
    });
}

void Common_optional_array_set_none_at(OptionalArray array, const unsigned int n) {
    Common_foreach(array, Optional, cur, {
        if (i == n) {
            Common_optional_set_none(cur);
            break;
        }
    });
}

void Common_optional_array_free_data_at(OptionalArray array, const unsigned int n) {
    Common_foreach(array, Optional, cur, {
        if (i == n) {
            Common_optional_free_data(cur);
            Common_optional_set_none(cur);
            break;
        }
    });
}

void Common_optional_array_destroy(OptionalArray array) {
    LCOMMON_FREE(array->elements);
    LCOMMON_FREE(array);
}

void Common_optional_array_free(OptionalArray array) {
    for (size_t i = 0; i < array->len; ++i) {
        Optional *opt_value = array->elements[i];
        LCOMMON_ASSERT(opt_value, "must be able to obtain elements from OptionalArray");
        Common_optional_free(opt_value);
    }

    Common_optional_array_destroy(array);
}

void Common_optional_array_append(OptionalArray array, Optional *optional) {
    array->elements[array->len++] = optional;
    if (array->len >= array->cap) {
        array->cap *= 2;
        array->elements = Common_srealloc(array->elements, sizeof(struct optional_t*) * array->cap);
    }
}

LCOMMON_BOOL Common_streql(const char *a, const char *b) {
    LCOMMON_BOOL ret = LCOMMON_TRUE;
    for (int i = 0; a[i] != '\0'; ++i) {
        if (a[i] != b[i]) {
            ret = LCOMMON_FALSE;
            break;
        }
    }

    return ret;
}

size_t Common_strcount(const char *s) {
    int i = 0;
    for (; s[i] != '\0'; ++i);
    return i;
}

char *__private__Common_strmerge(const char *separator, const char *first, ...) {
    va_list vsprint;
    va_start(vsprint, first);
    defer({ va_end(vsprint); });

    size_t cap = 124;
    char *result = Common_smalloc(cap);

    memcpy(result, (const void*) first, strlen(first) + 1);
    strcat(result, separator);

    char *cur;

    int j = -1;
    int i = j + 1;

    for (; (cur = va_arg(vsprint, char*)) != LCOMMON_TERMINATOR; ++i, ++j) {
        size_t counter = 0;
        counter += Common_strcount(result) + 1;
        counter += Common_strcount(cur) + 1;

        if (i > 1) {
            counter += Common_strcount(separator) + 1;
        }

        if (counter >= cap) {
            cap *= 2;
            result = Common_srealloc(result, cap);
        }

        strcat(result, cur);
        strcat(result, separator);
    }

    char first_with_sep[Common_strcount(first) + Common_strcount(separator) + 1];
    sprintf(first_with_sep, "%s%s", first, separator);

    // removing seps when there's actually va args that have been iterated at
    // and removing it if the result is just the first one + an uneeded sep.
    if (j >= 0 || Common_streql(result, first_with_sep)) {
        result[strlen(result) - strlen(separator)] = '\0';
    }

    // using the right amount of memory.
    result = Common_srealloc(result, strlen(result) + 1);

    return result;
}