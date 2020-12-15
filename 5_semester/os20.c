#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define SIZE_OF_STRINGS_IN_LIST 80

typedef struct str_list {
    pthread_rwlock_t node_lock;
    int node_id;
    char string[SIZE_OF_STRINGS_IN_LIST];
    struct str_list *previous;
    struct str_list *next;
} StringsList;

pthread_t thread;

StringsList *listTail = NULL;

int numOfNodes = 0;
int isFinish = 0;

void startChildThread();
void joinChildThread();
void initLock(pthread_rwlock_t *lock);
void destroyLock(pthread_rwlock_t *lock);
void readLock(pthread_rwlock_t *lock);
void writeLock(pthread_rwlock_t *lock);
void unlock(pthread_rwlock_t *lock);

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

    writeLock(&listTail->node_lock);

    StringsList *left = head;
    StringsList *right = head->next;
    char tmp_string[SIZE_OF_STRINGS_IN_LIST];

    while (left->next != NULL) {
        while (right != NULL) {
            writeLock(&(left->node_lock));
            writeLock(&(right->node_lock));

            printf("CHILD: SWAPPING NODES %d (%s) AND %d (%s)...\n",
                   left->node_id, left->string, right->node_id, right->string);

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

            sleep(1);

            unlock(&(right->node_lock));
            unlock(&(left->node_lock));

            right = right->next;
        }
        left = left->next;
        right = left->next;
    }

    unlock(&listTail->node_lock);
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

    initLock(&(next_node->node_lock));

    return next_node;
}

void fillList() {
    char next_string[SIZE_OF_STRINGS_IN_LIST];

    while (1) {
        gets(next_string);

        if (strlen(next_string) == 0) {
            break;
        }

        if (listTail != NULL) {
            readLock(&listTail->node_lock);
        }

        listTail = initNextNode(listTail, next_string);
        numOfNodes++;

        unlock(&listTail->node_lock);
    }

    isFinish = 1;
}

int main() {
    startChildThread();
    fillList();
    printList(getListHead(listTail));
    freeList(getListHead(listTail));
    return 0;
}

void* sortListEveryFiveSeconds(void *args) {
    while (!isFinish) {
        printf("CHILD: SORTING LIST...\n");
        sortList(getListHead(listTail), numOfNodes);
        printf("CHILD: LIST IS SORTED\n");
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

void initLock(pthread_rwlock_t *lock) {
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(lock, &attr);
}

void destroyLock(pthread_rwlock_t *lock) {
    pthread_rwlock_destroy(lock);
}

void readLock(pthread_rwlock_t *lock) {
    pthread_rwlock_rdlock(lock);
}

void writeLock(pthread_rwlock_t *lock) {
    pthread_rwlock_trywrlock(lock);
}

void unlock(pthread_rwlock_t *lock) {
    pthread_rwlock_unlock(lock);
}