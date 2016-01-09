#include "cmd_parse.h"
#include <stdbool.h>


typedef struct {
    FILE *inputFile;
    FILE *outputFile;
    char **argv;
    int argc;
} TentativeCmdInfo;


StringArray sepCmdsByPipe(char *buf){
    StringArray stArr = createStringArray(10);

    bool quote_entered = false;
    char *last_token_pos = buf;
    char *cur_pos = buf;

    while (true){
        if (*cur_pos == '\"'){
            quote_entered = !quote_entered;
        } else if ((*cur_pos == '|' && !quote_entered) || *cur_pos == '\0') {

            int token_nChars = cur_pos - last_token_pos;
            // Account for null character
            int token_alloc_size = token_nChars + 1;

            char *token_string = safeMalloc(token_alloc_size * sizeof(char));
            memcpy(token_string, last_token_pos, token_nChars * sizeof(char));
            token_string[token_nChars] = '\0';

            stringArrayInsert(&stArr, token_string);

            if (!(*cur_pos)){

                // Check for unclosed quotation
                if (quote_entered){
                    fprintf(stderr, "Unclosed quote in command\n");
                    exit(-1);
                }

                return stArr;
            }

            // Quick check to make sure that there aren't two pipes in a row
            // (More in-depth checking tbd elsewhere, but having
            // this will mess up the parser now)
            if (*(cur_pos + 1) == '|'){
                fprintf(stderr, "Cannot have two pipes in a row\n");
                exit(-1);
            }

            // Mark the start of new command
            last_token_pos = cur_pos + 1;
        }
        cur_pos++;
    }
}



#if 0
CmdChain parseCmds(char *buf) {
    StringArray cmd_string_blobs = sepCmdsByPipe(buf);
    int n = cmd_string_blobs.size;

    TentativeCmdInfo *tentative_cmd_info_list = safeMalloc(n * sizeof(TentativeCmdInfo));
    for (int i = 0; i < n; i++){
        tentative_cmd_info_list[i] = parseSingleCmd( stringArrayGet(&cmd_string_blobs, i) );
    }

    // Only first cmd should (optionally) have an input redirect
    for (int i = 0; i < n-1; i++){
        if (tentative_cmd_info_list[i].outputFile){
            fprintf(stderr, "Invalid output redirect\n");
            exit(-1);
        }
    }

    // Only last cmd should (optionally) have an output redirect
    for (int i = 1; i < n; i++){
        if (tentative_cmd_info_list[i].inputFile){
            fprintf(stderr, "Invalid input redirect\n");
            exit(-1);
        }
    }

    // Assemble full info
    CmdChain cc;
    cc->nCmds = n;

    CmdInfo *cmd_info_list = safeMalloc(n * sizeof(CmdInfo));
    for (int i = 0; i < n; i++){
        cmd_info_list[i].argv = tentative_cmd_info_list[i].argv;
        cmd_info_list[i].argc = tentative_cmd_info_list[i].argc;
    }
    cc->cmds = cmd_info_list;

    cc->inputStream  = tentative_cmd_info_list[0].inputFile;
    cc->outputStream = tentative_cmd_info_list[n-1].outputFile;

    return cc;
}

#endif