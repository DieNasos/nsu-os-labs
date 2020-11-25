#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    pid_t process; // process identifier

    switch (process = fork())
    {
        case -1:    // fork() finished with error
            return 1;

        case 0:     // child's code
            execl("/bin/cat", "cat", "os9.c", NULL);    // calling '/bin/cat' + sending arguments as list

        default:    // parent's code
            printf("Parent's waiting...\n");
            wait(0);
            printf("\nParent's turn.\n");
    }

    return 0;
}