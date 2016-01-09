#include <assert.h>
#include "../array_utils.h"
#include <stdbool.h>
#include <stdio.h>



bool testStrArray(){
    StringArray stArr = createStringArray(1);
    stringArrayInsert(&stArr, "hi");
    stringArrayInsert(&stArr, "plz");
    stringArrayInsert(&stArr, "3");
    stringArrayInsert(&stArr, "44");
    stringArrayInsert(&stArr, "555");
    return (stArr.size == 5 && stArr.capacity == 8);
}

int main() {
    assert(testStrArray());
    printf("Success\n");
    return 0;
}