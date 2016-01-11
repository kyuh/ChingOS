#define _BSD_SOURCE


#include "cmd_parse.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

typedef struct {
    char *inputFilename;
    char *outputFilename;
    char **argv;
    int argc;
} TentativeCmdInfo;


StringArray sepStringWithQuotes(char *buf, char separator, bool include_quote_mark){
    StringArray stArr = createStringArray(10);

    bool quote_entered = false;
    char *last_token_pos = buf;
    char *cur_pos = buf;

    while (true){
        /* Note start of a quoted phrase,
        and get rid of the quotation mark if so chosen
        (i.e. when parsing out the space-separated tokens in practice).

        Handle the beginning quotation mark here,
        handle the end quote in the next if-statement clause.

        (Assume the quoted phrases are separated from the rest of the text
        by another space, e.g. 
            grep "array" >out.txt       is valid
            grep"array">out.txt         is not
        */

        if (*cur_pos == '\"'){
            quote_entered = !quote_entered;

            if (include_quote_mark && quote_entered){
                last_token_pos = cur_pos + 1;
            }
        } else if ((*cur_pos == separator && !quote_entered) || *cur_pos == '\n' || *cur_pos == '\0') {


            // Exclude the ending quotation mark of the previous
            // statement, if necessary
            int token_nChars;
            if (include_quote_mark && *(cur_pos - 1) == '\"'){
                token_nChars = (cur_pos - 1) - last_token_pos;
            } else {
                token_nChars = cur_pos - last_token_pos;
            }


            // Account for null character
            int token_alloc_size = token_nChars + 1;

            char *token_string = safeMalloc(token_alloc_size * sizeof(char));
            memcpy(token_string, last_token_pos, token_nChars * sizeof(char));
            token_string[token_nChars] = '\0';

            stringArrayInsert(&stArr, token_string);

            if (*cur_pos == '\n' || *cur_pos == '\0'){

                // Check for unclosed quotation
                if (quote_entered){
                    fprintf(stderr, "Unclosed quote in command\n");
                    exit(-1);
                }

                break;
            }

            // Mark the start of new command
            last_token_pos = cur_pos + 1;
        }
        cur_pos++;
    }

    // Filter out any empty tokens
    StringArray stArr_filtered = createStringArray(10);
    for (int i = 0; i < stArr.size; i++){
        char *s = stringArrayGet(&stArr, i);
        if (s[0]){
            stringArrayInsert(&stArr_filtered, s);
        }
    }

    //printf("STARR SIZE: %d\n", stArr.size);
    //printf("STARR FILTERED SIZE: %d\n", stArr_filtered.size);

    return stArr_filtered;
}


TentativeCmdInfo parseSingleCmd(char *cmd_blob){
    StringArray sa_prelim_argv = sepStringWithQuotes(cmd_blob, ' ', true);

    TentativeCmdInfo tci;
    tci.inputFilename = NULL;
    tci.outputFilename = NULL;

    StringArray sa_argv = createStringArray(10);

    int i = 0;
    while (i < sa_prelim_argv.size){
        char *cur_str = stringArrayGet(&sa_prelim_argv, i);

        if (cur_str[0] == '<'){

            // If this is the only character in the token,
            // then the next string is the item to be redirected
            if (cur_str[1] == '\0'){
                if (i == sa_prelim_argv.size - 1){
                    fprintf(stderr, "Reached end of line while parsing\n");
                    exit(-1);
                }

                tci.inputFilename = strdup(stringArrayGet(&sa_prelim_argv, i+1));
                i += 2;
            } else {
                tci.inputFilename = strdup(cur_str + 1);    //lop off the <
                i += 1;
            }

        } else if (cur_str[0] == '>'){
            if (cur_str[1] == '\0'){
                if (i == sa_prelim_argv.size - 1){
                    fprintf(stderr, "Reached end of line while parsing\n");
                    exit(-1);
                }

                tci.outputFilename = strdup(stringArrayGet(&sa_prelim_argv, i+1));
                i += 2;
            } else {
                tci.outputFilename = strdup(cur_str + 1);    //lop off the <
                i += 1;
            }

        } else {
            stringArrayInsert(&sa_argv, cur_str);
            i += 1;
        }
    }

    tci.argv = stringArrayToNormalPlusNull(&sa_argv);
    tci.argc = sa_argv.size;

    return tci;
}


CmdChain parseCmds(char *buf) {
    StringArray cmd_string_blobs = sepStringWithQuotes(buf, '|', false);
    int n = cmd_string_blobs.size;

    TentativeCmdInfo *tentative_cmd_info_list = safeMalloc(n * sizeof(TentativeCmdInfo));
    for (int i = 0; i < n; i++){
        tentative_cmd_info_list[i] = parseSingleCmd( stringArrayGet(&cmd_string_blobs, i) );
    }

    // Only first cmd should (optionally) have an input redirect
    for (int i = 0; i < n-1; i++){
        if (tentative_cmd_info_list[i].outputFilename){
            fprintf(stderr, "Invalid output redirect\n");
            exit(-1);
        }
    }

    // Only last cmd should (optionally) have an output redirect
    for (int i = 1; i < n; i++){
        if (tentative_cmd_info_list[i].inputFilename){
            fprintf(stderr, "Invalid input redirect\n");
            exit(-1);
        }
    }

    // Assemble full info
    CmdChain cc;
    cc.nCmds = n;

    CmdInfo *cmd_info_list = safeMalloc(n * sizeof(CmdInfo));
    for (int i = 0; i < n; i++){
        cmd_info_list[i].argv = tentative_cmd_info_list[i].argv;
        cmd_info_list[i].argc = tentative_cmd_info_list[i].argc;
    }
    cc.cmds = cmd_info_list;

    char *inputFilename = tentative_cmd_info_list[0].inputFilename;
    char *outputFilename = tentative_cmd_info_list[n-1].outputFilename;

    if (inputFilename)
        cc.inputStream = open(inputFilename, O_RDONLY);
    else
        cc.inputStream = STDIN_FILENO;

    if (outputFilename) //open or create file, create with 644
        cc.outputStream = open(outputFilename, O_WRONLY | O_CREAT, 
            S_IRUSR | S_IWUSR |
            S_IRGRP |
            S_IROTH);
    else
        cc.outputStream = STDOUT_FILENO;

    return cc;
}
