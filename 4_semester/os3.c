#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void printUID() {
    printf("\tREAL UID = %d\n", getuid());
    printf("\tEFFECTIVE UID = %d\n", geteuid());
}

int main (int argc, char **argv) {
    printf("BEFORE USING SETUID\n");
    printUID();

    if (argc < 2) {
        printf("NO FILE NAME\n");
        exit(1);
    }

    FILE* file;

    file = fopen(argv[1], "r");
    if (file == NULL){
        perror("COULD NOT OPEN FILE");
    } else {
        printf("FILE WAS OPENED SUCCESSFULLY\n");
    }
    fclose(file);

    setuid(getuid());
    printf("AFTER USING SETUID\n");
    printUID();

    file = fopen(argv[1], "r");
    if (file == NULL){
        perror("COULD NOT OPEN FILE");
    } else {
        printf("FILE WAS OPENED SUCCESSFULLY\n");
    }
    fclose(file);

    return 0;
}