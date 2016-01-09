// The main loop for the command shell.

#define _POSIX_C_SOURCE 199506L

#include "shell_utils.h"
#include "cmd_parse.h"
#include "child_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define PATH_MAX 4096
#define CMD_MAX 1024
#define DEBUG 0

// The main loop for the command shell.
int main(void)
{
	while (1) {
		// Get working directory.
		char username[CMD_MAX + 1];
		char path[PATH_MAX + 1];
		// Get the current username.
		if (getlogin_r(username, sizeof(username)) != 0) {
			printf("Getting username failed!\n");
			exit(1);
		}
		// Get the current directory via a Unix system call.
		if (getcwd(path, sizeof(path)) == NULL) {
			printf("Getting path failed!\n");
			exit(1);
		}
	
		char* prompt = strncat(username, ":", 2);
		prompt = strncat(prompt, path, 4097);
		prompt = strncat(prompt, ">", 2);
		printf("%s ", prompt);

		// Now block on user input waiting for user input.
		char command[CMD_MAX + 1];
		if (fgets(command, sizeof(command), stdin) == NULL) {
			printf("Getting user input failed!\n");
		}

		// Check if the user wants to use the built-in functions cd or exit.
		char buf1[4];
		char buf2[5];		
		if (strncmp(strncpy(buf1, command, 3), "cd ", 4) == 0) {
			// Get the rest of the string.
			char buffer[CMD_MAX + 1];
			char* new_path = strncpy(buffer, command + 3, sizeof(buffer));
			// Strip newlines.
			char* tmp;
			if ((tmp = strrchr(new_path, '\n')) != NULL) {
				*tmp = 0;
			}
			if (chdir(new_path) != 0) {
				// printf("%d\n", chdir(new_path));
				printf("Changing directories failed!\n");
				exit(1);
			}
		}
		else if (strncmp(strncpy(buf2, command, 4), "exit", 4) == 0) {
			exit(0);
		}
		else {
			// If the command does not fall into any of the above cases, give the
			// command to our parser.
			CmdChain chain = parseCmds(command);

            //check for valid input and output streams
            if(chain.inputStream != -1 && chain.outputStream != -1) {
			    // Handle piping and other things.
			    startChildren(chain);
            } else {
                //this is probably just because the user typed a file that does not exist
                //the program should still continue
                printf("Failed to open input or output stream: %s\n", strerror(errno));
            }
		}		
	}
	return 0;
}

