#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    pid_t process; // process identifier
    int return_value;

    switch (process = fork())
    {
        case -1:    // fork() finished with error
            exit(return_value);

        case 0:     // child's code
            execv(argv[1], argv + 2);    // calling function (argv[1] - name; argv[2], ... - arguments)

        default:    // parent's code
            printf("Parent's waiting...\n");
            wait(0);
            printf("\nChild's work finished with code %d.\n", WEXITSTATUS(return_value));
            printf("Parent's turn.\n");
    }

    return 0;
}