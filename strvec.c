#include "strvec.h"
#include <stdlib.h>
#include <string.h>

void vec_init(StrVec* vec) {
    vec->size = 0;
    vec->len = 0;
}

void vec_append(StrVec* vec, char* element) {
    if(vec->size == 0) {
        vec->size = 10;
        vec->elements = malloc(sizeof(char*) * vec->size);
    }

    if(vec->size == vec->len) {
        vec->size *= 2;
        vec->elements = realloc(vec->elements, sizeof(char*) * vec->size);
    }

    vec->elements[vec->len] = element;
    vec->len += 1;
}

char* vec_get(StrVec* vec, int index) {
    if(index >= vec->len) {
        return "\0";
    }
    return vec->elements[index];
}

int vec_set(StrVec* vec, int index, char* element) {
    if(index >= vec->len) {
        return -1;
    }

    vec->elements[index] = element;
    return index;
}

bool vec_includes(StrVec* vec, char* element) {
    for(int i=0;i<vec->len;i++) {
        if(strcmp(element, vec->elements[i]) == 0) {
            return true;
        }
    }

    return false;
}

void vec_free(StrVec* vec) {
    free(vec->elements);
}

void vec_clear(StrVec* vec) {
    vec_free(vec);
    vec->size = 0;
    vec->elements = 0;
}