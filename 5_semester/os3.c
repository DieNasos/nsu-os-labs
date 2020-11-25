#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t threads[4];

void* printMessage(void *args) {
    printf("%s\n", (char*) args);
}

void joinThread(int id) {
    int status, status_addr;
    status = pthread_join(threads[id], (void**)&status_addr);

    if (status != 0) {
        printf("ERROR :: COULD NOT JOIN THREAD %d :: STATUS = %d\n", id, status);
    }
}

int main() {
    char *messages[] = {"Hello from thread 0!",
                        "Hello from thread 1!",
                        "Hello from thread 2!",
                        "Hello from thread 3!"};

    int i, status;

    for (i = 0; i < 4; i++) {
        status = pthread_create(&threads[i], NULL, printMessage, (void*) messages[i]);

        if (status != 0) {
            printf("ERROR :: COULD NOT CREATE THREAD %d :: STATUS = %d\n", i, status);
            exit(-1);
        }
    }

    for (i = 0; i < 4; i++) {
        joinThread(i);
    }

    return 0;
}