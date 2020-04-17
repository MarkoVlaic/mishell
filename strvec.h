#ifndef STRVEC_H
#define STRVEC_H

#include <sys/types.h>
#include <stdbool.h>

// A vector of strings that is variable in size
typedef struct StrVec {
    int len;
    size_t size;
    char** elements;
} StrVec;

void vec_init();
void vec_append(StrVec* vec, char* element);
char* vec_get(StrVec* vec, int index);
int vec_set(StrVec* vec, int index, char* element);
bool vec_includes(StrVec* vec, char* element);
void vec_free(StrVec* vec);
void vec_clear(StrVec* vec);

#endif