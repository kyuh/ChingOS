#include <assert.h>
#include "shell_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include "cmd_parse.h"
#include "array_utils.h"

bool testSafeMalloc1() {
    int *arr = safeMalloc(2 * sizeof(int));
    if (arr){
        return true;
    } else {
        return false;
    }
}
#if 0
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

    printf("ARGV:\n");
    for (int i = 0; i < tci.argc; i++){
        printf("%s\n", tci.argv[i]);
    }

}
#endif


void testCmdParsing5(){
    printf("\n\n");
    CmdChain cc = parseCmds("grep <halp.txt | blah sorgs meh | plz > whoo.txt");

    printf("INPUT: %d\n", cc.inputStream);
    printf("OUTPUT: %d\n", cc.outputStream);
    printf("nCmds: %d\n", cc.nCmds);

    for (int i = 0; i < cc.nCmds; i++){
        for (int j = 0; j < cc.cmds[i].argc; j++){
            printf("%s\n", cc.cmds[i].argv[j]);
        }
        printf("DELIMITER\n");
    }
}


void testCmdParsing6(){
    printf("\n\n");
    CmdChain cc = parseCmds("ls\n");

    printf("INPUT: %d\n", cc.inputStream);
    printf("OUTPUT: %d\n", cc.outputStream);
    printf("nCmds: %d\n", cc.nCmds);

    for (int i = 0; i < cc.nCmds; i++){
        for (int j = 0; j < cc.cmds[i].argc; j++){
            printf("i = %d\n",i);
            printf("j = %d\n",j);
            printf("%s\n", cc.cmds[i].argv[j]);
        }
    }

    printf("%d\n", cc.nCmds);
    printf("%d\n", cc.cmds[0].argc);

    char *s = cc.cmds[0].argv[0];
    printf("STR: %s\n", cc.cmds[0].argv[0]);
    while (*s){
        printf("%d\n", *s);
        s++;
    }
    printf("%d\n", *s);
}


void testCmdParsing7(){
    printf("\n\n");
    CmdChain cc = parseCmds("grep \"array\" >out.txt\n");

    printf("INPUT: %d\n", cc.inputStream);
    printf("OUTPUT: %d\n", cc.outputStream);
    printf("nCmds: %d\n", cc.nCmds);

    for (int i = 0; i < cc.nCmds; i++){
        for (int j = 0; j < cc.cmds[i].argc; j++){
            printf("i = %d\n",i);
            printf("j = %d\n",j);
            printf("%s\n", cc.cmds[i].argv[j]);
        }
    }

    printf("%d\n", cc.nCmds);
    printf("%d\n", cc.cmds[0].argc);

    char *s = cc.cmds[0].argv[0];
    printf("STR: %s\n", cc.cmds[0].argv[0]);
    while (*s){
        printf("%d\n", *s);
        s++;
    }
    printf("%d\n", *s);
}


int main() {
    assert(testSafeMalloc1());
/*    testCmdParsing1();
    testCmdParsing2();
    testCmdParsing2a();
    testCmdParsing3();
    testCmdParsing4();*/
    testCmdParsing5();
    testCmdParsing6();
    testCmdParsing7();
    printf("Success\n");
    return 0;
}
