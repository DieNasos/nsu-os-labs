#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_STRINGS 10
#define PARENT 0
#define CHILD 1

void initMutex();
void _lock();
void _unlock();
void destroyMutex();
void initConditionVariable();
void _signal(int who);
void _sleep(int who);
void _done(int who);
void startChildThread();
void joinChildThread();

pthread_t thread;
pthread_mutex_t mutex;
pthread_cond_t cond;

int parentPrinted = 0, childPrinted = 0;

void* childPrintStrings(void *args) {
    int i;
    for (i = 1; i <= NUM_STRINGS; i++) {
        _lock();
        printf("\tCHILD: %d\n", i);
        _signal(CHILD);
        if (i == NUM_STRINGS) {
            _done(CHILD);
        }
        _unlock();
        _sleep(CHILD);
    }
}

void parentPrintStrings() {
    int i;
    for (i = 1; i <= NUM_STRINGS; i++) {
        _lock();
        printf("PARENT: %d\n", i);
        _signal(PARENT);
        if (i == NUM_STRINGS) {
            _done(PARENT);
        }
        _unlock();
        _sleep(PARENT);
    }
}

int main() {
    initMutex();
    initConditionVariable();
    startChildThread();
    parentPrintStrings();
    joinChildThread();
    destroyMutex();
    return 0;
}

void initMutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);
}

void _lock() {
    pthread_mutex_lock(&mutex);
}

void _unlock() {
    pthread_mutex_unlock(&mutex);
}

void destroyMutex() {
    pthread_mutex_destroy(&mutex);
}

void initConditionVariable() {
    pthread_cond_init(&cond, NULL);
}

void _signal(int who) {
    if (who == PARENT) {
        parentPrinted = 1;
    } else {
        childPrinted = 1;
    }
    pthread_cond_signal(&cond);
}

void _sleep(int who) {
    if (parentPrinted == 2 || childPrinted == 2) {
        return;
    }
    if (who == PARENT) {
        parentPrinted = 0;
        while (childPrinted == 0) {
            pthread_cond_wait(&cond, &mutex);
        }
    } else {
        childPrinted = 0;
        while (parentPrinted == 0) {
            pthread_cond_wait(&cond, &mutex);
        }
    }
}

void _done(int who) {
    if (who == PARENT) {
        parentPrinted = 2;
    } else {
        childPrinted = 2;
    }
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