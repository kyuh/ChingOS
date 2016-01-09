#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <shell_utils.h>

typedef struct {
    int output;
    int input;
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
void forkChild(char **argv, const Pipe inputStream, const Pipe outputStream) {
    //don't do anything if we're the child process
    if(fork()) {
        //set input and output of the process
        dup2(inputStream.output, STDIN_FILENO);
        dup2(outputStream.input, STDOUT_FILENO);

        //close unneeded descriptors
        //close(inputStream.input);
        //close(outputStream.output);

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
    //allocate pipes
    //pipes between each command, plus an input and output
    //printf("%d\n", sizeof(Pipe) * (commands.nCmds + 1));
    Pipe * pipes = (Pipe *)malloc(sizeof(Pipe) * (commands.nCmds + 1));
    
    for(int i = 0; i < commands.nCmds + 1; i++) {
        pipe((int*)(pipes + i));
    }

    pipes[0].input = 0;
    pipes[0].output = 0;
    
    pipes[commands.nCmds].input = 1;
    pipes[commands.nCmds].output = 1;

    for(int i = 0; i < commands.nCmds + 1; i++) {
        printf("%d  %d \n", pipes[i].input, pipes[i].output);
    }
    

    //now start each child
    for(int i = 0; i < commands.nCmds; i++) {
        forkChild(commands.cmds[i].argv, pipes[i], pipes[i + 1]);
    }
}
