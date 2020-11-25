#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN_NUM_OF_STEPS 1000000

typedef struct {
    double value;
} DoubleWrapper;

double joinThread(pthread_t* threads, int id) {
    DoubleWrapper* threadReturnValue;

    int status = pthread_join(threads[id], (void**)&threadReturnValue);
    if (status != 0) {
        printf("ERROR :: COULD NOT JOIN THREAD %d :: STATUS = %d\n", id, status);
    }

    double value = threadReturnValue->value;

    free(threadReturnValue);

    return value;
}

void* countPartOfPi(void *args) {
    int numOfThreads = ((int*) args)[0];
    int threadID = ((int*) args)[1];
    int numOfStepsPerThread = ((int*) args)[2];

    DoubleWrapper* partOfPi = (DoubleWrapper*) malloc(sizeof(DoubleWrapper));
    partOfPi->value = 0.0;

    int denominator = 1;
    for (int i = 0; i < threadID; i++) {
        denominator += 2;
    }

    int coefficient = (threadID % 2 == 0) ? 1 : -1;
    int stepsPassed = 0;

    for (int i = 1; i <= numOfStepsPerThread; i++) {
        partOfPi->value += (double) coefficient / denominator;
        denominator += 2 * numOfThreads;
        stepsPassed += numOfThreads;
        coefficient = ((stepsPassed + threadID) % 2 == 0) ? 1 : -1;
    }

    partOfPi->value *= 4;

    return (void*) partOfPi;
}

double countPi(int numOfThreads, int globalNumOfSteps) {
    pthread_t* threads = (pthread_t*) malloc (sizeof(pthread_t) * numOfThreads);
    int numOfStepsPerThread = globalNumOfSteps / numOfThreads;
    int **threadsArgs = (int**) malloc(sizeof(int*) * numOfThreads);

    for (int i = 0; i < numOfThreads; i++) {
        threadsArgs[i] = (int*) malloc(sizeof(int) * 3);
        threadsArgs[i][0] = numOfThreads;
        threadsArgs[i][1] = i;
        threadsArgs[i][2] = numOfStepsPerThread;

        int status = pthread_create(&threads[i], NULL, countPartOfPi, (void*) threadsArgs[i]);
        if (status != 0) {
            printf("ERROR :: COULD NOT CREATE THREAD %d :: STATUS = %d\n", i, status);
            exit(-1);
        }
    }

    double countedPi = 0.0;

    for (int i = 0; i < numOfThreads; i++) {
        countedPi += joinThread(threads, i);
        free(threadsArgs[i]);
    }

    free(threadsArgs);

    return countedPi;
}

int main(int argc, char** argv) {
    int numOfThreads = atoi(argv[1]);
    int globalNumOfSteps = numOfThreads;

    while (globalNumOfSteps < MIN_NUM_OF_STEPS) {
        globalNumOfSteps += numOfThreads;
    }

    double countedPi = countPi(numOfThreads, globalNumOfSteps);

    printf("NUM OF THREADS: %d\n", numOfThreads);
    printf("NUM OF STEPS: %d\n", globalNumOfSteps);
    printf("COUNTED VALUE OF PI: %f\n", countedPi);

    return 0;
}