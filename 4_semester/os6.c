#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

char* fileName;

void printNewLinesTable(int* newLinesTable, int size) {

	printf("Created new lines table:\n");

	int i;

	for (i = 0; i < size; i++) {

		printf("%d: %d\n", i, newLinesTable[i]);
	}
}

int* getNewLinesTable() {

	// printf("Getting new lines table...\n");

	int fileHandle = open(fileName, O_RDONLY);

	if (fileHandle == -1) {

		perror("ERROR :: getNewLinesTable :: FILE_OPENING_FAILED");
		return NULL;
	}

	// printf("File %s was opened successfully\n", fileName);

	char currentSymbol;

	int newLinesCounter = 0;
	int currentPosition;

	int* newLinesTable = (int*)malloc(sizeof(int));

	while (read(fileHandle, &currentSymbol, 1)) {

		if (currentSymbol == '\n') {

			newLinesCounter++;
			currentPosition = lseek(fileHandle, 0L, SEEK_CUR);

			realloc(newLinesTable, sizeof(int) * newLinesCounter);	// "pushing" new cell to table

			newLinesTable[newLinesCounter - 1] = currentPosition;
		}
	}

	if (close(fileHandle)) {

		perror("ERROR :: getNewLinesTable :: FILE_CLOSING_FAILED");
		return NULL;
	}

	// printf("File %s was closed successfully\n", fileName);

	// printNewLinesTable(newLinesTable, newLinesCounter);

	return newLinesTable;
}

void printLine(int numberOfLine, int* newLinesTable) {

	// printf("Printing line...\n");

	int fileHandle = open(fileName, O_RDONLY);

	if (fileHandle == -1) {

		perror("ERROR :: printLine :: FILE_OPENING_FAILED");
		return;
	}

	// printf("File %s was opened successfully\n", fileName);

	// checking for finish criteria
	if (numberOfLine == 0) {

		printf("Work with text is finished!\n");

		if (close(fileHandle)) {
			perror("ERROR :: getNewLinesTable :: FILE_CLOSING_FAILED");
		}

		// printf("File %s was closed successfully\n", fileName);

		return;
	}

	// error if number of line is bigger than amount of lines in text
	if (numberOfLine > sizeof(newLinesTable) / sizeof(newLinesTable[0]) + 1) {

		printf("ERROR :: printLine :: NUMBER_OF_LINE_IS_OUT_OF_BOUNDS\n");

		if (close(fileHandle)) {
			perror("ERROR :: getNewLinesTable :: FILE_CLOSING_FAILED");
		}

		// printf("File %s was closed successfully\n", fileName);

		return;
	}

	// if number of line is bigger than 1 then finding the end of previous line
	if (numberOfLine > 1) {

		lseek(fileHandle, newLinesTable[numberOfLine - 2], SEEK_CUR);
	}
	// otherwise we want to print the 1st line so going from the beginning of text

	char currentSymbol;

	printf("Line #%d: ", numberOfLine);

	// printing line
	while (read(fileHandle, &currentSymbol, 1)) {

		if (currentSymbol != '\n') {

			printf("%c", currentSymbol);
		}
		else {

			break;
		}
	}

	printf("\n");

	if (close(fileHandle)) {

		perror("ERROR :: getNewLinesTable :: FILE_CLOSING_FAILED");
		return;
	}

	// printf("File %s was closed successfully\n", fileName);
}

void printAllText() {

	printf("Printing all text...\n");

	int fileHandle = open(fileName, O_RDONLY);

	if (fileHandle == -1) {

		perror("ERROR :: printLine :: FILE_OPENING_FAILED");
		return;
	}

	// printf("File %s was opened successfully\n", fileName);

	char currentSymbol;

	// printing text
	while (read(fileHandle, &currentSymbol, 1)) {

		printf("%c", currentSymbol);

	}

	if (close(fileHandle)) {

		perror("ERROR :: getNewLinesTable :: FILE_CLOSING_FAILED");
		return;
	}

	// printf("File %s was closed successfully\n", fileName);
}

void alarmTimeIsOver() {

	printf("\n5 sec is over!\n");
	printAllText();
	printf("Work with text is finished!\n");
	exit(0);
}

void workWithTextFile() {

	// printf("Starting to work with text file...\n");

	int* newLinesTable = getNewLinesTable(fileName);

	if (newLinesTable == NULL) {

		printf("ERROR :: workWithTextFile :: COULD_NOT_GET_NEW_LINES_TABLE\n");
		return;
	}

	int numberOfLine;

	signal(SIGALRM, alarmTimeIsOver);
	alarm(5);

	do {

		printf("Enter number of line to print: ");
		scanf("%d", &numberOfLine);

		alarm(0);

		printLine(numberOfLine, newLinesTable);

	} while (numberOfLine != 0);

	free(newLinesTable);
}

int main() {

	fileName = "in.txt";

	workWithTextFile();

	return 0;
}
