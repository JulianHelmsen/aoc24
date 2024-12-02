#ifndef AOC_DA_H
#define AOC_DA_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// TODO: assert(da.data != NULL);
#define da_append(da, e)\
    do{\
        if((da).size == (da).capacity) {\
            size_t new_cap = (da).capacity * 2; \
            if(new_cap < 10) \
                new_cap = 10;\
            void* data = realloc((da).data, new_cap * sizeof((da).data[0]));\
            if(data == NULL) {\
                perror("realloc");\
                exit(1); \
            }\
            (da).capacity = new_cap; \
            (da).data = data; \
        }\
        (da).data[(da).size++] = (e); \
    }while(0)

#define da_reserve(da, cap) \
    do { \
        if((da).capacity >= (cap)) \
            break;\
        void* data = realloc((da).data, (cap) * sizeof((da).data[0]));\
        if(data == NULL) {\
            perror("realloc");\
            exit(1);\
        }\
        (da).data = data;\
        (da).capacity = (cap); \
    }while(0)

#endif // AOC_DA_H
