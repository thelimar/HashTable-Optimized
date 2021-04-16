#pragma once

#include "OneWayLinkedList.h"

enum HashParams
{
	HASH_TABLE_SIZE = 12097,
	MAX_WORD_LEN = 20,
};


struct HashTable
{
	size_t size;
	List* DataArray;
	unsigned int (*HashFunc) (char* str);
};


HashTable HashTableBuild (unsigned int (*HashFunc) (char* str), size_t size);
int HashTableDestruct (HashTable* dis);
int HashTableVerificator (HashTable dis);
int HashTableLoad (HashTable* dis, char* buffer, int nLines);
int CollisionDump (HashTable dis, FILE* output);
char* HashFind (HashTable dis, char* str_to_find);

unsigned int HashFuncOne (char* str);
unsigned int HashFuncLen (char* str);
unsigned int HashFuncSum (char* str);
unsigned int HashFuncSumDivLen (char* str);
unsigned int HashFuncDedSum (char* str);
unsigned int HashFuncROL (char* str);
unsigned int HashFuncROR (char* str);
unsigned int HashFuncMurMur2 (char* str);