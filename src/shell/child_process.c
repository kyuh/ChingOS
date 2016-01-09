#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <shell_utils.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


/*
 * If a command is not found, other commands will still run to completion (bash does this)
 * The shell will also not exit
 *
 * If something terrible happens like we run out of fds, as before, children will still run 
 * until completion, but the shell will go away and 
 */

//read data from output
//write data to input
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
    pid_t pid = fork();
    //run the command if we're the child process
    if(pid > 0) {
        //set input and output of the process

        //only set the streams if we're reading or writing from a pipe
        if(inputStream.input != 0) {
            close(inputStream.input);
            //we'll also close stdin, because we don't need it
            close(STDIN_FILENO);

            //duplicate the descriptor in
            dup2(inputStream.output, STDIN_FILENO);
        }
        if(outputStream.output != 1)
        {
            close(outputStream.output);
            //we'll also close stdout, because we don't need it
            close(STDOUT_FILENO);

            //duplicate the new descriptor
            dup2(outputStream.input, STDOUT_FILENO);
        }

        //execute the other program
        execvp(argv[0], argv);

        //if we're here, the execution failed
        //we'll tell the user and then terminate
        fprintf(stderr, "failed to execute child process: %s\n", strerror(errno));
        exit(errno);
    } else if(pid == -1) {
        //if the fork failed, we're in a bad place
        //it's probably not safe to continue running the shell at all
        //so we'll just exit the shell and give up
        fprintf(stderr, "Failed to create fork, shell will quit nkw: %s\n", strerror(errno));
    }
}

/*
 * Closes all of the pipes passed in.
 * each element of the pipes array should be 
 * count is the number of pipes, not FDs
 */
void closeStreams(Pipe *pipes, int count) {
    for(int i = 0; i < count; i++) {
        //only close if they're pipes, not our own stdio fds
        if(pipes[i].input >= 2) close(pipes[i].input);
        if(pipes[i].output >= 2) close(pipes[i].output);
    }
}

/*
 * runs the specified chain of commands
 * 
 * If a command is not found or a child process is forked, but fails to execute
 * the given command, the shell will continue, and other commands will still run
 *
 * 
 */
void startChildren(CmdChain commands) {
    //allocate pipes
    //pipes between each command, plus an input and output
    //printf("%d\n", sizeof(Pipe) * (commands.nCmds + 1));
    Pipe * pipes = (Pipe *)safeMalloc(sizeof(Pipe) * (commands.nCmds + 1));
    
    for(int i = 0; i < commands.nCmds + 1; i++) {
        //try allocating the pipe
        if(pipe((int*)(pipes + i)))
        {
            //if the pipe failed to allocate, we're in a bad place
            //it's probably not safe to continue running the shell at all
            //so we'll just exit the shell and give up
            fprintf(stderr, "Failed to create pipe, shell will quit now: %s\n", strerror(errno));
        }
    }
    
    //set the input stream to stdin
    pipes[0].input = 0;
    pipes[0].output = 0;
    
    //set the output stream to stdout
    pipes[commands.nCmds].input = 1;
    pipes[commands.nCmds].output = 1;

    for(int i = 0; i < commands.nCmds + 1; i++) {
        printf("%d  %d \n", pipes[i].input, pipes[i].output);
    }
    

    //now start each child
    for(int i = 0; i < commands.nCmds; i++) {
        forkChild(commands.cmds[i].argv, pipes[i], pipes[i + 1]);
    }

    //now clean up the streams, since the shell no longer needs any FDs of the pipes
    closeStreams(pipes, commands.nCmds + 1);

    //now wait for the children to finish running
    //for our simple shell, we can just wait on all children
    while(wait(NULL) > 0) { ; }
}
