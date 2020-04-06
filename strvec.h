#ifndef STRVEC_H
#define STRVEC_H
#endif

#include <sys/types.h>

typedef struct StrVec {
    int len;
    size_t size;
    char** elements;
} StrVec;

void vec_init();
void vec_append(StrVec* vec, char* element);
char* vec_get(StrVec* vec, int index);
int vec_set(StrVec* vec, int index, char* element);
void vec_free(StrVec* vec);
void vec_clear(StrVec* vec);