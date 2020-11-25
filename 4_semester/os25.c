#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv) {
    /*
     * Pipe is an entity in which we can put and retrieve data,
     * for which there are two file descriptors associated with the pipe:
     * one for writing to the pipe, the other for reading.
     * To create a pipe, we use the pipe() system call.
     */
    int fd[2];  // file descriptors for the pipe
    pipe(fd);   // creating pipe

    char ch;    // symbol
    char* str = argv[1];    // unput string
    int length = strlen(str);   // getting string length

    pid_t process;    // process identifier
    switch (process = fork())
    {
        // trying to create child process
        case -1:    // fork() finished with error
            return -1;

        case 0:
            // child's code
            close(fd[1]);   // closing useless descriptor
            while (read (fd[0], &ch, 1))
            {
                // reading and printing enlarged symbols of input string
                // toupper('a') == 'A'
                printf("%c", toupper(ch));
            }
            printf("\n");
            close(fd[0]);   // closing the second descriptor

        default:
            // parent's code
            close(fd[0]);   // closing useless descriptor
            write (fd[1], str, length);  // writing input string to the pipe
            close(fd[1]);   // closing the second descriptor
    }

    return 0;
}