#pragma once

#include "OneWayLinkedList.h"

enum HashParams
{
	HASH_TABLE_SIZE = 12097,
	MAX_WORD_LEN = 32,
};


struct HashTable
{
	size_t size;
	List* DataArray;
	unsigned int (*HashFunc) (__m256i* str, int strlen, __m256i* drop_buffer);
};


HashTable HashTableBuild(unsigned int (*HashFunc) (__m256i* str, int strlen, __m256i* drop_buffer), size_t size);
int HashTableDestruct(HashTable* dis);
int HashTableVerificator(HashTable dis);
int HashTableLoad(HashTable* dis, __m256i* buffer, int nLines, __m256i* drop_buffer);
int CollisionDump(HashTable dis, FILE* output);
__m256i* HashFind(HashTable dis, __m256i* str_to_find, int strlen, __m256i* drop_buffer);

unsigned int HashFuncOne(char* str);
unsigned int HashFuncLen(char* str);
unsigned int HashFuncSum(char* str);
unsigned int HashFuncSumDivLen(char* str);
unsigned int HashFuncDedSum(char* str);
unsigned int HashFuncROL(char* str);
unsigned int HashFuncROR(char* str);
unsigned int HashFuncMurMur2(__m256i* str, int strlen, __m256i* drop_buffer);

int SSEstrlen(char* str);