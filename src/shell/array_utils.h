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


StringArray createStringArray(int starting_cap);

void stringArrayDeleteLast(StringArray *stArr);

void stringArrayResize(StringArray *stArr);

void stringArrayInsert(StringArray *stArr, ST_ARR_TYPE elt);

ST_ARR_TYPE stringArrayGet(StringArray *stArr, size_t idx);

char **stringArrayToNormalPlusNull(StringArray *stArr);

#endif // ARRAY_UTILS_H
