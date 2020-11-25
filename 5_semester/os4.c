#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* printMessage(void *args) {
    // printing message in infinite cicle
    while (1) {
        printf("This is message. ");
    }
}

int main() {
    pthread_t thread;

    int status = pthread_create(&thread, NULL, printMessage, NULL);

    if (status != 0) {
        printf("ERROR :: COULD NOT CREATE THREAD :: STATUS = %d\n", status);
        exit(-1);
    }

    // parent thread's sleeping for 2 seconds
    sleep(2);

    // interrupting work of child thread
    pthread_cancel(thread);

    return 0;
}