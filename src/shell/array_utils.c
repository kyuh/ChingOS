#include <stdlib.h>
#include <string.h>
#include "array_utils.h"



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

