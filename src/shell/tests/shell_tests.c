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

void testCmdParsing1(){
    printf("\n\n");
    StringArray stArr = sepStringWithQuotes("grep | blah | plz", '|');
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
    }
}

void testCmdParsing2(){
    printf("\n\n");
    StringArray stArr = sepStringWithQuotes("grep | blah \" whoo | whee \" plz", '|');
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
    }
}


void testCmdParsing2a(){
    printf("\n\n");
    StringArray stArr = sepStringWithQuotes("grep | blah whoo ||||||||| whee plz", '|');
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
        //printf("%d\n", stringArrayGet(&stArr, i)[0]);
    }
}


void testCmdParsing3(){
    printf("\n\n");
    StringArray stArr = sepStringWithQuotes("grep Allow < output.txt", ' ');
    printf("SIZE: %d\n", stArr.size);
    for (int i = 0; i < stArr.size; i++){
        printf("%s\n", stringArrayGet(&stArr, i));
    }
}

void testCmdParsing4(){
    printf("\n\n");
    StringArray stArr = sepStringWithQuotes("grep | blah sorgs meh <halp.txt | plz", '|');
    printf("SIZE: %d\n", stArr.size);

    TentativeCmdInfo tci = parseSingleCmd(stringArrayGet(&stArr, 1));

    printf("ARGC: %d\n", tci.argc);
    printf("%s\n", tci.inputFilename);

    for (int i = 0; i < tci.argc; i++){
        printf("%s\n", tci.argv[i]);
    }

}


int main() {
    assert(testSafeMalloc1());
    testCmdParsing1();
    testCmdParsing2();
    testCmdParsing2a();
    testCmdParsing3();
    testCmdParsing4();
    printf("Success\n");
    return 0;
}