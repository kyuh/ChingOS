#include <stdlib.h>
#include <string.h>

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H


typedef char* ST_ARR_TYPE;

// Sized array of char arrays
typedef struct {
    ST_ARR_TYPE *data;
    int size;
    int capacity;
} StringArray;


StringArray createStringArray(int starting_cap){
    StringArray stArr;
    stArr.data = malloc(starting_cap * sizeof(ST_ARR_TYPE));
    stArr.size = 0;
    stArr.capacity = starting_cap;
    return stArr;
}

void stringArrayDeleteLast(StringArray *stArr){
    stArr->data[ stArr->size ] = NULL;      // optional
    stArr->size -= 1;
}

void stringArrayResize(StringArray *stArr){
    ST_ARR_TYPE *new_buffer = malloc(2 * stArr->capacity * sizeof(ST_ARR_TYPE));
    memcpy(new_buffer, stArr->data, stArr->capacity * sizeof(ST_ARR_TYPE));

    free(stArr->data);
    stArr->data = new_buffer;
    stArr->capacity *= 2;
}

void stringArrayInsert(StringArray *stArr, ST_ARR_TYPE elt){
    if (stArr->size == stArr->capacity){
        stringArrayResize(stArr);
    }
    stArr->data[stArr->size] = elt;
    stArr->size += 1;
}

ST_ARR_TYPE stringArrayGet(StringArray *stArr, size_t idx){
    return stArr->data[idx];
}

#endif // ARRAY_UTILS_H
