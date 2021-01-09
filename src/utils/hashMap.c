/*
Author- Jacob Gaeta
This program creates a hashmap and defines functions
that modify or access information in the hashmap.
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


int hashFunction1(const char* key) {
	int r = 0;
	int i;
	for (i = 0; key[i] != '\0'; i++) {
		r += key[i];
	}
	return r;
}

int hashFunction2(const char* key) {
	int r = 0;
	int i;
	for (i = 0; key[i] != '\0'; i++) {
		r += (i + 1) * key[i];
	}
	return r;
}


// Creates a new hash table link with a copy of the key string.
HashLink* hashLinkNew(const char* key, int value, HashLink* next) {
	HashLink* link = malloc(sizeof(HashLink));
	link->key = malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(link->key, key);
	link->value = value;
	link->next = next;
	return link;
}


// Free the allocated memory for a hash table link created with hashLinkNew.
static void hashLinkDelete(HashLink* link) {
	assert(link != NULL);
	assert(link->key != NULL);
	free(link->key);
	free(link);
}

/*
 Initializes a hash table map, allocating memory for a link pointer table with
 the given number of buckets.
 */
void hashMapInit(HashMap* map, int capacity) {
	map->capacity = capacity;
	map->size = 0;
	map->table = malloc(sizeof(HashLink*) * capacity);

	for (int i = 0; i < capacity; i++) {
		map->table[i] = NULL;
	}
}


// Removes all links in the map and frees all allocated memory. 
void hashMapCleanUp(HashMap* map) {
	assert(map != NULL);

	HashLink* current;
	HashLink* next;
	int capacity = map->capacity;

	for (int i = 0; i < capacity; i++) {
		current = map->table[i];
		if (current != NULL) {
			do {
				next = current->next;
				hashLinkDelete(current);
				current = next;
			} while (current != NULL);
		}
	}
	map->size = 0;
	free(map->table);
}

/*
 Creates a hash table map, allocating memory for a link pointer table with
 the given number of buckets.
 */
HashMap* hashMapNew(int capacity) {
	HashMap* map = malloc(sizeof(HashMap));
	hashMapInit(map, capacity);
	return map;
}

/*
 Removes all links in the map and frees all allocated memory, including the
 map itself.
 */
void hashMapDelete(HashMap* map) {
	hashMapCleanUp(map);
	free(map);
}

/*
 Returns a pointer to the value of the link with the given key. Returns NULL if
 no link with that key is in the table.
 */
int* hashMapGet(HashMap* map, const char* key) {
	assert(map != NULL);
	assert(key != NULL);

	int index = HASH_FUNCTION(key) % hashMapCapacity(map);
	HashLink* traverse = map->table[index];

	if (traverse != NULL) {
		do {
			if (strcmp(traverse->key, key) == 0) {
				return &(traverse->value);
			}
			traverse = traverse->next;
		} while (traverse != NULL);
	}

	return NULL;
}

/*
 Resizes the hash table to have a number of buckets equal to the given
 capacity. After allocating the new table, all of the links are rehashed.
 */
void resizeTable(HashMap* map, int capacity) {
	HashMap* newMap = hashMapNew(capacity);
	int prevCapacity = hashMapCapacity(map);

	for (int i = 0; i < prevCapacity; i++) {
		HashLink* current = map->table[i];

		if (current != NULL) {
			do {
				hashMapPut(newMap, current->key, current->value);
				current = current->next;
			} while (current != NULL);
		}
	}

	hashMapCleanUp(map);
	map->table = newMap->table;
	map->size = newMap->size;
	map->capacity = newMap->capacity;
	newMap->table = NULL;
	free(newMap);
}

// Function that updates the given key-value pair in hash table
void hashMapUpdate(HashMap* map, const char* key, int index, int value) {
	if (!hashMapContainsKey(map, key)) {
		HashLink* newLink = hashLinkNew(key, value, NULL);

		assert(newLink != NULL);

		if (map->table[index] == NULL) {
			map->table[index] = newLink;
		}
		else {
			HashLink* traverse = map->table[index];

			while (traverse->next != NULL) {
				traverse = traverse->next;
			}
			traverse->next = newLink;
		}
	}
	
	else {
		int* getVal = hashMapGet(map, key);
		(*getVal) += value;
	}
}

/*
If a link with the given key already exists, it just updates the value. Otherwise, 
it will create a new link with the given key and value and add it to the table
bucket's linked list.
 */
void hashMapPut(HashMap* map, const char* key, int value) {
	float loadFactor;
	int mapCapacity = hashMapCapacity(map);
	int index = HASH_FUNCTION(key) % hashMapCapacity(map);

	hashMapUpdate(map, key, index, value);
		
	map->size++;
	
	loadFactor = hashMapTableLoad(map);

	if (loadFactor >= MAX_TABLE_LOAD) {
		resizeTable(map, 2 * mapCapacity);
	}
}

/*
 Removes and frees the link with the given key from the table. If no such link
 exists, it does nothing.
 */
void hashMapRemove(HashMap* map, const char* key) {
	assert(map != NULL);
	assert(key != NULL);

	int index = HASH_FUNCTION(key) % hashMapCapacity(map);

	HashLink* current = map->table[index];
	HashLink* previous = NULL;

	if (current != NULL && strcmp(current->key, key) == 0) {
		map->table[index] = current->next;
		hashLinkDelete(current);
		map->size--;
	}
	else if (current != NULL && strcmp(current->key, key) != 0) {

		while (strcmp(current->key, key) != 0) {
			previous = current;
			current = current->next;
		}

		if (previous != NULL) {
			previous->next = current->next;
		}
		hashLinkDelete(current);
		map->size--;
	}

}


// Returns 1 if a link with the given key is in the table and 0 otherwise.
int hashMapContainsKey(HashMap* map, const char* key) {
	assert(map != NULL);
	assert(key != NULL);

	int capacity = hashMapCapacity(map);
	int index = HASH_FUNCTION(key) % capacity;

	HashLink* traverse = map->table[index];

	if (traverse != NULL) {
		do {
			if (strcmp(traverse->key, key) == 0) {
				return 1;
			}
			traverse = traverse->next;
		} while (traverse != NULL);
	}

	return 0;
}


// Returns the number of links in the table. 
int hashMapSize(HashMap* map) {
	assert(map != 0);

	return map->size;
}


// Returns the number of buckets in the table.
int hashMapCapacity(HashMap* map) {
	assert(map != 0);

	return map->capacity;
}


// Returns the number of table buckets without any links.
int hashMapEmptyBuckets(HashMap* map) {
	int accum = 0;
	int capacity = hashMapCapacity(map);

	for (int i = 0; i < capacity; i++) {
		if (!map->table[i]) {
			accum++;
		}
	}

	return accum;
}


// Returns the ratio of (number of links) / (number of buckets) in the table.
float hashMapTableLoad(HashMap* map) {
	assert(map != 0);

	float size = (float)hashMapSize(map);
	float capacity = (float)hashMapCapacity(map);

	return size / capacity;
}


// Prints all the links in each of the buckets in the table.
void hashMapPrint(HashMap* map) {

	for (int i = 0; i < map->capacity; i++) {
		HashLink* traverse = map->table[i];
		if (traverse != NULL) {
			printf("\nBucket %i ->", i);
			while (traverse != NULL) {
				printf("(%s: %d)", traverse->key, traverse->value);
				traverse = traverse->next;
				if (traverse != NULL) {
					printf("->");
				}
			}
		}
	}
	printf("\n");
}
