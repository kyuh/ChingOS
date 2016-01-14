#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "child_process.h"

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
    if(pid == 0) {
        //set input and output of the process

        //only set the streams if we're reading or writing from a pipe
        if(inputStream.output != 0) {
            close(inputStream.input);
            
            //duplicate the descriptor in
            dup2(inputStream.output, STDIN_FILENO);
            
            //now close the thing we copied from 
            close(inputStream.output);
        }
        if(outputStream.input != 1)
        {
            close(outputStream.output);

            //duplicate the new descriptor
            dup2(outputStream.input, STDOUT_FILENO);
            
            //now close the thing we copied from 
            close(outputStream.input);
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
        fprintf(stderr, "Failed to create fork, shell will quit now: %s\n", strerror(errno));
        exit(errno);
    }
}

//create a pipe; abort the program if this operation fails
void safePipe(int* fds)
{
    int success = pipe(fds);
    if(success == -1) {
        //we cannot create a pipe
        //this is a bad situation, and it is probably not safe to continue the shell
        fprintf(stderr, "Failed to create pipe, shell will quit now: %s\n", strerror(errno));
        exit(errno);
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
    //previous and next pipes
    Pipe previous;
    Pipe next;
    
    //set the input stream to specified fd
    //this is a dummy value, since this fd only has one end we are
    //aware of
    previous.input = -1;
    previous.output = commands.inputStream;


    //now start each child
    for(int i = 0; i < commands.nCmds; i++) {
        //allocate the pipe for output, or if this is the last stream
        //handle redirection
        if(i == commands.nCmds - 1) {
            next.input = commands.outputStream;
            next.output = -1;
        } else {
            //allocate a new next stream
            pipe((int*)(&next));
        }

        //the forked child will close the streams itself
        forkChild(commands.cmds[i].argv, previous, next);

        //we must also close the streams as the parent
        //and advance to the next stream
        //don't close out own stdin stream
        if(previous.output != 0) {
            close(previous.input);
            close(previous.output);
        }

        previous = next;
    }

    //clean up the final pipe
    //don't close our own stdout stream
    if(previous.input != 1)
    {
        close(previous.input);
        close(previous.output);
    }


    //now wait for the children to finish running
    //for our simple shell, we can just wait on all children
    while(wait(NULL) > 0) { ; }
}
