#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define SIZE_OF_STRINGS_IN_LIST 80

typedef struct str_list {
    int node_id;
    char string[SIZE_OF_STRINGS_IN_LIST];
    struct str_list *previous;
    struct str_list *next;
} StringsList;

pthread_t thread;
pthread_mutex_t mutex;

StringsList *listTail = NULL;
int numOfNodes = 0;
int isFinish = 0;

void startChildThread();
void joinChildThread();
void initMutex();
void destroyMutex();
void lockMutex();
void unlockMutex();

void freeList(StringsList *head) {
    while (head != NULL) {
        StringsList *next = head->next;
        free(head);
        head = next;
    }
}

void printList(StringsList *head) {
    if (head == NULL) {
        printf("printList :: ERROR :: head is null\n");
        return;
    }

    do {
        printf("%d: %s\n", head->node_id, head->string);
        head = head->next;
    } while (head != NULL);
}

int compareStrings(char *string1, char *string2) {
    // -1: string1 < string2
    // 0: string1 == string2
    // 1: string1 > string2

    int length1 = strlen(string1);
    int length2 = strlen(string2);
    int min_length = (length1 < length2) ? length1 : length2;

    for (int it = 0; it < min_length; it++) {
        if (string1[it] < string2[it]) {
            return -1;
        } else if (string1[it] == string2[it]) {
            continue;
        } else {
            return 1;
        }
    }

    if (length1 == length2) {
        return 0;
    } else if (length1 < length2) {
        return -1;
    } else {
        return 1;
    }
}

void sortList(StringsList *head, int num_of_nodes) {
    if (head == NULL) {
        return;
    }

    StringsList *left = head;
    StringsList *right = head->next;
    char tmp_string[SIZE_OF_STRINGS_IN_LIST];

    while (left->next != NULL) {
        while (right != NULL) {
            if (compareStrings(left->string, right->string) == -1) {
                for (int it = 0; it < SIZE_OF_STRINGS_IN_LIST; it++) {
                    tmp_string[it] = left->string[it];
                }
                for (int it = 0; it < SIZE_OF_STRINGS_IN_LIST; it++) {
                    left->string[it] = right->string[it];
                }
                for (int it = 0; it < SIZE_OF_STRINGS_IN_LIST; it++) {
                    right->string[it] = tmp_string[it];
                }
            }
            right = right->next;
        }
        left = left->next;
        right = left->next;
    }
}

StringsList* getListTail(StringsList *node) {
    if (node == NULL) {
        return NULL;
    }

    while (node->next != NULL) {
        node = node->next;
    }

    return node;
}

StringsList* getListHead(StringsList *node) {
    if (node == NULL) {
        return NULL;
    }

    while (node->previous != NULL) {
        node = node->previous;
    }

    return node;
}

StringsList* initNextNode(StringsList *current_node, char *next_string) {
    StringsList *next_node = (StringsList*) malloc(sizeof(StringsList));

    if (current_node != NULL) {
        current_node->next = next_node;
    }

    next_node->previous = current_node;
    next_node->next = NULL;
    next_node->node_id = (current_node == NULL) ? 0 : current_node->node_id + 1;

    for (int it = 0; it < SIZE_OF_STRINGS_IN_LIST; it++) {
        next_node->string[it] = next_string[it];
    }

    return next_node;
}

void fillList() {
    char next_string[SIZE_OF_STRINGS_IN_LIST];

    while (1) {
        gets(next_string);

        if (strlen(next_string) == 0) {
            break;
        }

        lockMutex();
        listTail = initNextNode(listTail, next_string);
        numOfNodes++;
        unlockMutex();
    }

    isFinish = 1;
}

int main() {
    initMutex();
    startChildThread();
    fillList();
    printList(getListHead(listTail));
    destroyMutex();
    freeList(getListHead(listTail));
    return 0;
}

void* sortListEveryFiveSeconds(void *args) {
    while (!isFinish) {
        lockMutex();
        printf("CHILD: SORTING...\n");
        sortList(getListHead(listTail), numOfNodes);
        unlockMutex();
        sleep(5);
    }
}

void startChildThread() {
    sleep(1);
    int status = pthread_create(&thread, NULL, sortListEveryFiveSeconds, NULL);
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

void initMutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);
}

void destroyMutex() {
    pthread_mutex_destroy(&mutex);
}

void lockMutex() {
    pthread_mutex_lock(&mutex);
}

void unlockMutex() {
    pthread_mutex_unlock(&mutex);
}