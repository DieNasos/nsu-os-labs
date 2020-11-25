#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* printStrings(void *args) {
    int i;
    for (i = 1; i <= 10; i++) {
        printf("CHILD THREAD: this is string №%d\n", i);
    }
}

int main() {
    pthread_t thread;
    int status = pthread_create(&thread, NULL, printStrings, NULL);

    if (status != 0) {
        printf("ERROR :: COULD NOT CREATE THREAD :: STATUS = %d\n", status);
        exit(-1);
    }

    int status_addr;
    status = pthread_join(thread, (void**)&status_addr);

    if (status != 0) {
        printf("ERROR :: COULD NOT JOIN THREAD :: STATUS = %d\n", status);
        exit(-2);
    }

    int i;
    for (i = 1; i <= 10; i++) {
        printf("PARENT THREAD: this is string №%d\n", i);
    }

    return 0;
}