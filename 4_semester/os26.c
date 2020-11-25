#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char** argv) {
    char *cmd = "cat text"; // == print content of 'text' file in console
    char ch;    // symbol
	FILE *file;

	/*
     * The popen() function creates a child process in which the cmd command is executed.
     * This one call replaces the calls to the pipe() and fork() functions.
     */
	if (file = popen(cmd, "r")) {
	    // now the child process is doing cmd-task
        while (fread(&ch, 1, 1, file)) {
            // reading one symbol of text's content
            // printing it elarged
            printf("%c", toupper(ch));
        }
	} else {
        printf("ERROR :: COULD NOT OPEN PIPE-FILE");
        return 1;
	}

	if (pclose(file)) {
	    printf("ERROR :: COULD NOT CLOSE PIPE-FILE");
	    return 1;
	} else {
	    // success
	    return 0;
	}
}