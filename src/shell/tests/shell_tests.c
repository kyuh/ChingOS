#include <assert.h>
#include "../shell_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include "../cmd_parse.c"

bool testSafeMalloc1() {
    int *arr = safeMalloc(2 * sizeof(int));
    if (arr){
        return true;
    } else {
        return false;
    }
}

void testPipeParsing1(){
    StringArray stArr = sepCmdsByPipe("grep | blah | plz");
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
    }
}

void testPipeParsing2(){
    StringArray stArr = sepCmdsByPipe("grep | blah \" whoo | whee \" plz");
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
    }
}


int main() {
    assert(testSafeMalloc1());
    testPipeParsing1();
    testPipeParsing2();
    printf("Success\n");
    return 0;
}