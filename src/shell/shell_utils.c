/* Utils for custom shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include "shell_utils.h"

void* safeMalloc(size_t nBytes) {
    void *ptr = malloc(nBytes);
    if (!ptr){
        fprintf(stderr, "Memory allocation error, aborting\n");
        exit(-1);
    }
    return ptr;
}

