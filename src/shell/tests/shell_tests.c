#include <assert.h>
#include "../shell_utils.h"
#include <stdbool.h>
#include <stdio.h>

bool testSafeMalloc1() {
    int *arr = safeMalloc(2 * sizeof(int));
    if (arr){
        return true;
    } else {
        return false;
    }
}


int main() {
    assert(testSafeMalloc1());
    printf("Success\n");
    return 0;
}