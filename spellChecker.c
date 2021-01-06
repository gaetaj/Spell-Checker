/*
Author- Jacob Gaeta
This program utilizes the hashmap in 
"hashMap.c" to implement a spellchecker
program that can check user input for 
spelling errors.
*/
#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 Allocates a string for the next word in the file and returns it. This string
 is null terminated. Returns NULL after reaching the end of the file.
 */
char* nextWord(FILE* file) {
    int maxLength = 16;
    int wordLength = 0;
    char* word = malloc(sizeof(char) * maxLength);

    while (1) {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'') {
            if (wordLength + 1 >= maxLength) {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[wordLength] = c;
            wordLength++;
        }
        else if (wordLength > 0 || c == EOF) {
            break;
        }
    }
    if (wordLength == 0) {
        free(word);
        return NULL;
    }
    word[wordLength] = '\0';
    return word;
}

// Loads the contents of the file into the hash map.
void loadDictionary(FILE* file, HashMap* map) {
	char* word = nextWord(file);

	while (word != NULL) {
		hashMapPut(map, word, 1);
		free(word);
		word = nextWord(file);
	}
	free(word);
}

// Function that runs spellchecker and returns 1 if user enters "quit", returns 0 otherwise
int runSpellChecker(char* inputBuffer, HashMap* map) {

	printf("Enter a word or enter the word quit to exit the program: ");
	scanf("%s", inputBuffer);

	for (int i = 0; inputBuffer[i]; i++) {
		inputBuffer[i] = tolower(inputBuffer[i]);
	}

	if (strcmp(inputBuffer, "quit") == 0) {
		return 1;
	}
	else if (hashMapContainsKey(map, inputBuffer)) {
		printf("The word ");
		printf("%s", inputBuffer);
		printf(" is spelled correctly.\n");
	}
	else {
		printf("The word ");
		printf("%s", inputBuffer);
		printf(" is not spelled correctly.\n");
	}

	return 0;
}

// Checks the spelling of the word provided by the user.
int main(int argc, const char** argv) {
	HashMap* map = hashMapNew(1000);

	printf("dictionary.txt is being opened\n");

	FILE* file = fopen("dictionary.txt", "r");
	clock_t timer = clock();
	loadDictionary(file, map);
	timer = clock() - timer;

	printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);

	fclose(file);
	char inputBuffer[256];
	int quit = 0;
	while (!quit) {

		quit = runSpellChecker(inputBuffer, map);

	}

	hashMapDelete(map);
	return 0;
}
