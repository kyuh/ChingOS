#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <shell_utils.h>

typedef struct {
    int input;
    int output;
} Pipe;

/*
 * This function forks the process and starts the program with the
 * specified input and output streams for stdin and stdout.  
 * 
 * The parent process will retain all of the stream FDs still open
 * This will close the write FD for the inputStream, and the read FD
 * for the outputStream
 * 
 * (recieve data from input, output it into output)
 */
void forkChild(char **argv, Pipe *inputStream, Pipe *outputStream) {
    //don't do anything if we're the child process
    if(fork()) {
        //set input and output of the process
        dup2(inputStream->output, STDIN_FILENO);
        dup2(outputStream->input, STDOUT_FILENO);

        //close unneeded descriptors
        close(inputStream->input);
        close(outputStream->output);

        //execute the other program
        execvp(argv[0], argv);
    }
}

/*
 * Closes all of the pipes passed in.
 * each element of the pipes array should be 
 * count is the number of pipes, not FDs
 */
void closeStreams(Pipe *pipes, int count) {
    for(int i = 0; i < count; i++) {
        close(pipes[i].input);
        close(pipes[i].output);
    }
}

/*
 *
 */
void startChildren(CmdChain commands) {
    
}
