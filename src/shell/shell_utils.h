/* Utils for custom shell.
 */

#include <stdio.h>

typedef struct {
    char **argv;
    int argc;
} CmdInfo;

typedef struct {
    FILE *inputStream;
    FILE *outputStream;
    CmdInfo *cmds;
} CmdChain;

