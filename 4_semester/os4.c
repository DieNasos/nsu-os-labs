#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

typedef struct List {

	char* _string;
	int _stringLength;
	struct List* _prevNode;
	struct List* _nextNode;

} List;

void pushNodeToList(List* listTale, char* string, int stringLength) {

	if ((listTale->_prevNode == NULL) && (listTale->_string == NULL)) {

		listTale->_string = string;
		listTale->_stringLength = stringLength;
		return;
	}

	List* newTale = (List*)malloc(sizeof(List));
	newTale->_string = string;
	newTale->_stringLength = stringLength;
	newTale->_prevNode = listTale;
	newTale->_nextNode = NULL;

	listTale->_nextNode = newTale;
}

void popNodeFromList(List* listTale) {

	free(listTale->_string);
	listTale = listTale->_prevNode;
	free(listTale->_nextNode);
	listTale->_nextNode = NULL;
}

List* findListTale(List* listNode) {

	List* currentListNode = listNode;

	while (currentListNode->_nextNode != NULL) {
		currentListNode = currentListNode->_nextNode;
	}

	return currentListNode;
}

List* findListHead(List* listNode) {

	List* currentListNode = listNode;

	while (currentListNode->_prevNode != NULL) {
		currentListNode = currentListNode->_prevNode;
	}

	return currentListNode;
}

void freeList(List* listTale) {

	while (listTale->_prevNode != NULL) {
		popNodeFromList(listTale);
	}

	free(listTale->_string);
	free(listTale);
}

List* createList() {

	List* newListHead = (List*)malloc(sizeof(List));
	newListHead->_string = NULL;
	newListHead->_stringLength = 0;
	newListHead->_prevNode = NULL;
	newListHead->_nextNode = NULL;

	return newListHead;
}

void printString(int stringID, char* string, int stringLength) {

	int i;

	printf("STRING %d: ", stringID);

	for (i = 0; i < stringLength; i++) {
		printf("%c", string[i]);
	}

	printf("\n");
}


void printAllStrings(List* listHead) {

	int counter = 0;

	do {

		printString(counter, listHead->_string, listHead->_stringLength);
		counter++;
		listHead = listHead->_nextNode;

	} while (listHead != NULL);
}

void scanStrings(List* listHead) {

	char stringBuffer[BUFFER_SIZE];
	char* string;
	int i;

	while(gets(stringBuffer)) {

		string = (char*)malloc(strlen(stringBuffer));

		for (i = 0; i < strlen(stringBuffer); i++) {
			string[i] = stringBuffer[i];
		}

		if (stringBuffer[0] == '.') {

			if (strlen(stringBuffer) == 1) {
				break;
			}
		}

		pushNodeToList(findListTale(listHead), string, strlen(stringBuffer));
	}
}

void scanAndPrintStrings() {

	List* list = createList();
	
	scanStrings(list);
	printAllStrings(findListHead(list));
	freeList(list);
}

int main() {

	scanAndPrintStrings();
	return 0;
}
