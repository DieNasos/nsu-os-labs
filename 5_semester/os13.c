#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_STRINGS 10
#define PARENT 0
#define CHILD 1

void _lock(int who);
void _unlock(int who);
void initSemaphores();
void destroySemaphores();
void startChildThread();
void joinChildThread();

pthread_t thread;
sem_t sems[2];

void* childPrintStrings(void *args) {
    for (int i = 1; i <= NUM_STRINGS; i++) {
        _lock(PARENT);
        printf("\tCHILD: %d\n", i);
        _unlock(CHILD);
    }
}

void parentPrintStrings() {
    for (int i = 1; i <= NUM_STRINGS; i++) {
        printf("PARENT: %d\n", i);
        _unlock(PARENT);
        _lock(CHILD);
    }
}

int main() {
    initSemaphores();
    startChildThread();
    parentPrintStrings();
    joinChildThread();
    destroySemaphores();
    return 0;
}

void initSemaphores() {
    sem_init(&sems[0], 1, 0);
    sem_init(&sems[1], 1, 0);
}

void _lock(int who) {
    sem_wait(&sems[who]);
    // sem > 0 => sem -= 1
    // sem == 0 => waiting
}

void _unlock(int who) {
    sem_post(&sems[who]);
    // sem++
}

void destroySemaphores() {
    sem_destroy(&sems[0]);
    sem_destroy(&sems[1]);
}

void startChildThread() {
    int status = pthread_create(&thread, NULL, childPrintStrings, NULL);
    if (status != 0) {
        printf("ERROR :: COULD NOT CREATE THREAD :: STATUS = %d\n", status);
        exit(-1);
    }
}

void joinChildThread() {
    int status = pthread_join(thread, NULL);
    if (status != 0) {
        printf("ERROR :: COULD NOT JOIN THREAD :: STATUS = %d\n", status);
        exit(-2);
    }
}