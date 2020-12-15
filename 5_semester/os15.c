#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

#define NUM_STRINGS 10
#define PARENT 0
#define CHILD 1

void _wait(int who);
void _signal(int who);
void initSemaphores();
void destroySemaphores();

sem_t *sems;

void childPrintStrings() {
    for (int i = 1; i <= NUM_STRINGS; i++) {
        _wait(PARENT);
        printf("\tCHILD: %d\n", i);
        _signal(CHILD);
    }
}

void parentPrintStrings() {
    for (int i = 1; i <= NUM_STRINGS; i++) {
        printf("PARENT: %d\n", i);
        _signal(PARENT);
        _wait(CHILD);
    }
}

int main() {
    initSemaphores();
    pid_t process = fork();
    if (process == -1) {
        // error
        return -1;
    } else if (process == 0) {
        // child's section
        childPrintStrings();
    } else {
        // parent's section
        parentPrintStrings();
    }
    destroySemaphores();
    return 0;
}

void initSemaphores() {
    sems = mmap(NULL, 2 * sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sems == MAP_FAILED) {
        printf("ERROR :: MAP_FAILED");
        exit(-2);
    }
    sem_init(&sems[0], 1, 0);
    sem_init(&sems[1], 1, 0);
}

void _wait(int who) {
    sem_wait(&sems[who]);
    // sem > 0 => sem -= 1
    // sem == 0 => waiting
}

void _signal(int who) {
    sem_post(&sems[who]);
    // sem++
}

void destroySemaphores() {
    sem_destroy(&sems[0]);
    sem_destroy(&sems[1]);
}