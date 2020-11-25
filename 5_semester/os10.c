#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define COMMON 0
#define PARENT 1
#define CHILD 2

void initMutexes();
void destroyMutexes();
void startChildThread();
void joinChildThread();
void lockMutex(int mutexID);
void unlockMutex(int mutexID);

pthread_t thread;

pthread_mutex_t mutexes[3];
// 0 == "nobody's (common) mutex"
// 1 == "parent's mutex"
// 2 == "child's mutex"

void* childPrintStrings(void *args) {
    for (int i = 1; i <= 10; i++) {
        lockMutex(PARENT);
        printf("CHILD THREAD: this is string №%d\n", i);
        unlockMutex(CHILD);
        lockMutex(COMMON);
        unlockMutex(PARENT);
        lockMutex(CHILD);
        unlockMutex(COMMON);
    }
    unlockMutex(CHILD);
}

void parentPrintStrings() {
    for (int i = 1; i <= 10; i++) {
        printf("PARENT THREAD: this is string №%d\n", i);
        lockMutex(COMMON);
        unlockMutex(PARENT);
        lockMutex(CHILD);
        unlockMutex(COMMON);
        lockMutex(PARENT);
        unlockMutex(CHILD);

    }
    unlockMutex(PARENT);
}

int main() {
    initMutexes();
    lockMutex(PARENT);
    startChildThread();
    sleep(1);
    parentPrintStrings();
    joinChildThread();
    destroyMutexes();
    return 0;
}

void initMutexes() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mutexes[i], &attr);
    }
}

void destroyMutexes() {
    for (int i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mutexes[i]);
    }
}

void startChildThread() {
    lockMutex(CHILD);
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

void lockMutex(int mutexID) {
    pthread_mutex_lock(&mutexes[mutexID]);
}

void unlockMutex(int mutexID) {
    pthread_mutex_unlock(&mutexes[mutexID]);
}