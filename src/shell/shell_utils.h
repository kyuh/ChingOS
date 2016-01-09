/* Utils for custom shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include "array_utils.h"

typedef struct {
    char **argv;
    int argc;
} CmdInfo;

typedef struct {
    // FILEs are NULL if stdin or stdout (respectively) should be used
    FILE *inputStream;
    FILE *outputStream;
    CmdInfo *cmds;
    int nCmds;
} CmdChain;

void* safeMalloc(size_t nBytes) {
    void *ptr = malloc(nBytes);
    if (!ptr){
        fprintf(stderr, "Memory allocation error, aborting\n");
        exit(-1);
    }
    return ptr;
}