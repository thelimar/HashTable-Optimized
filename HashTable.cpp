#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "OneWayLinkedList.h"
#include "oneg.h"

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

HashTable HashTableBuild (unsigned int (*HashFunc) (char* str), size_t size)
{
	HashTable dis_hashtable = { size, 0, HashFunc};
	dis_hashtable.DataArray = (List*) calloc (size, sizeof (List));

	if (dis_hashtable.DataArray)
	{
		for (int i = 0; i < size; i++)
			dis_hashtable.DataArray[i] = ListBuild ();
	}
	return dis_hashtable;
}

int HashTableDestruct (HashTable* dis)
{
	if (!dis) return 0;
	for (unsigned int i = 0; i < dis->size; i++)
		ListDestruct (&dis->DataArray[i]);

	free (dis->DataArray);
	return 0;
}

int HashTableVerificator (HashTable dis)
{
	if (dis.DataArray)
	{
		for (unsigned int i = 0; i < dis.size; i++)
		{
			if (ListVerificator (dis.DataArray[i]))
				return ListVerificator (dis.DataArray[i]);
		}
		return 0;
	}
	return 0;
}

int HashTableLoad (HashTable* dis, char* buffer, int nLines)
{
	for (int i = 0; i < nLines / 2; i++)
	{
		ListPush (&dis->DataArray[(unsigned int) dis->HashFunc (buffer) % dis->size], buffer, buffer + strlen (buffer) + 1);
		buffer += strlen (buffer) + 1;
		buffer += strlen (buffer) + 1;
	}

	return 0;
}

int CollisionDump (HashTable dis, FILE* output)
{
	for (unsigned int i = 0; i < dis.size; i++)
	{
		fprintf(output, "%d\r\n", dis.DataArray[i].size);
	}

	return 0;
}


char* HashFind (HashTable dis, char* str_to_find)
{
	unsigned int hash = dis.HashFunc (str_to_find) % dis.size;
	if (dis.DataArray[hash].size == 0)
		return NULL;
	else
		return ListFind (dis.DataArray[hash], str_to_find);
}


unsigned int HashFuncOne (char* str)
{
	return 1;
}

unsigned int HashFuncLen (char* str)
{
	return strlen (str);
}

unsigned int HashFuncSum (char* str)
{
	int sum = 0;
	for (int i = 0; str[i] != '\0' ; i++)
	{
		sum += str[i];
	}

	return sum;
}

unsigned int HashFuncSumDivLen (char* str)
{
	int sum = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		sum += str[i];
	}

	return sum / strlen (str);
}

unsigned int HashFuncDedSum (char* str)
{
	int sum = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		sum += str[i];
	}

	return sum - *str;
}

unsigned int HashFuncROR (char* str)
{
	unsigned int sum = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		sum = (sum << 31) | (sum >> 1);
		sum =  sum xor str[i];
	}

	return sum;
}

unsigned int HashFuncROL (char* str)
{
	unsigned int sum = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		sum = (sum >> 31) | (sum << 1);
		sum = sum xor str[i];
	}

	return sum;
}

unsigned int HashFuncMurMur2 (char* str)
{
	int len = strlen (str);
	const unsigned int mask = 0x5bd1e995;
	const unsigned int seed = 0;
	const int r = 24;

	unsigned int hash = seed ^ len;
	const unsigned char* data = (const unsigned char*) str;

	unsigned int k = 0;

	while (len >= 4)
	{
		k = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= mask;
		k ^= k >> r;
		k *= mask;

		hash *= mask;
		hash ^= k;

		data += 4;
		len -= 4;
	}

	switch (len)
	{
	case 3:
		hash ^= data[2] << 16;
	case 2:
		hash ^= data[1] << 8;
	case 1:
		hash ^= data[0];
		hash *= mask;
	};

	hash ^= hash >> 13;
	hash *= mask;
	hash ^= hash >> 15;

	return hash;
}