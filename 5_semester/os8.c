#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <limits.h>

int *stepsByThreads;
int stepsBetweenChecks;
int shouldStop; // 0 == no, 1 == yes

typedef struct {
    double value;
} DoubleWrapper;

int max(int* arr, int numOfElements) {
    // finding max element of array
    int max = arr[0];
    for (int i = 1; i < numOfElements; i++) {
        if (max < arr[i]) {
            max = arr[i];
        }
    }
    return max;
}

void interruption(int sig) {
    shouldStop = 1; // true
}

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

    DoubleWrapper* partOfPi = (DoubleWrapper*) malloc(sizeof(DoubleWrapper));
    partOfPi->value = 0.0;

    long denominator = 1;
    for (int i = 0; i < threadID; i++) {
        denominator += 2;
    }

    long stepsPassed = 0;
    int coefficient = (threadID % 2 == 0) ? 1 : -1;

    while (shouldStop == 0) {
        for (int i = 1; i <= stepsBetweenChecks; i++) {
            partOfPi->value += (double) coefficient / denominator;

            if (denominator < LONG_MAX - 2 * numOfThreads) {
                // no overflow
                denominator += 2 * numOfThreads;
            }

            if (stepsPassed > LONG_MAX - numOfThreads) {
                // overflow -> reset
                stepsPassed = (stepsPassed % 2 == 0) ? 0 : 1;
            }

            stepsPassed += numOfThreads;
            coefficient = ((stepsPassed + threadID) % 2 == 0) ? 1 : -1;
        }

        stepsByThreads[threadID] = stepsPassed;
    }

    int maxNumOfSteps = max(stepsByThreads, numOfThreads);

    if (stepsPassed < maxNumOfSteps) {
        // dis-balance -> counting a little bit more
        for (int i = 1; i <= maxNumOfSteps - stepsPassed; i++) {
            partOfPi->value += (double) coefficient / denominator;

            if (denominator < LONG_MAX - 2 * numOfThreads) {
                // no overflow
                denominator += 2 * numOfThreads;
            }

            if (stepsPassed > LONG_MAX - numOfThreads) {
                // overflow -> reset
                stepsPassed = (stepsPassed % 2 == 0) ? 0 : 1;
            }

            stepsPassed += numOfThreads;
            coefficient = ((stepsPassed + threadID) % 2 == 0) ? 1 : -1;
        }
    }

    partOfPi->value *= 4;

    return (void*) partOfPi;
}

double countPi(int numOfThreads) {
    pthread_t* threads = (pthread_t*) malloc (sizeof(pthread_t) * numOfThreads);
    int **threadsArgs = (int**) malloc(sizeof(int*) * numOfThreads);

    stepsByThreads = (int*) malloc(sizeof(int) * numOfThreads);
    shouldStop = 0; // false
    // binding 'interruption' function with interruption signal
    signal(SIGINT, interruption);

    for (int i = 0; i < numOfThreads; i++) {
        threadsArgs[i] = (int*) malloc(sizeof(int) * 2);
        threadsArgs[i][0] = numOfThreads;
        threadsArgs[i][1] = i;

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
    if (argc != 3) {
        printf("ERROR :: REQUIRED ARGUMENTS:\n");
        printf("1) NUMBER OF COUNTING THREADS\n");
        printf("2) NUMBER OF STEPS BETWEEN STOP-CONDITION CHECKS\n");
        return -1;
    }

    int numOfThreads = atoi(argv[1]);
    stepsBetweenChecks = atoi(argv[2]);
    double countedPi = countPi(numOfThreads);

    printf("\nNUM OF THREADS: %d\n", numOfThreads);
    printf("STEPS BETWEEN CHECKS: %d\n", stepsBetweenChecks);
    printf("COUNTED VALUE OF PI: %f\n", countedPi);

    return 0;
}
