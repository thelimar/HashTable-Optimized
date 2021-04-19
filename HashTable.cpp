#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "OneWayLinkedList.h"
#include "oneg.h"
#include <intrin.h>


enum HashParams
{
	HASH_TABLE_SIZE = 12097,
	MAX_WORD_LEN = 20,
};


struct HashTable
{
	size_t size;
	List* DataArray;
	unsigned int (*HashFunc) (char* str, __m128i* drop_buffer);
};


HashTable HashTableBuild (unsigned int (*HashFunc) (char* str, __m128i* drop_buffer), size_t size);
int HashTableDestruct (HashTable* dis);
int HashTableVerificator (HashTable dis);
int HashTableLoad (HashTable* dis, char* buffer, int nLines, __m128i* drop_buffer);
int CollisionDump (HashTable dis, FILE* output);
char* HashFind (HashTable dis, char* str_to_find, __m128i* drop_buffer);

unsigned int HashFuncOne (char* str);
unsigned int HashFuncLen (char* str);
unsigned int HashFuncSum (char* str);
unsigned int HashFuncSumDivLen (char* str);
unsigned int HashFuncDedSum (char* str);
unsigned int HashFuncROL (char* str);
unsigned int HashFuncROR (char* str);
unsigned int HashFuncMurMur2 (char* str, __m128i* drop_buffer);

int SSEstrlen (char* str);

HashTable HashTableBuild (unsigned int (*HashFunc) (char* str, __m128i* drop_buffer), size_t size)
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

int HashTableLoad (HashTable* dis, char* buffer, int nLines, __m128i* drop_buffer)
{
	for (int i = 0; i < nLines / 2; i++)
	{
		ListPush (&dis->DataArray[(unsigned int) dis->HashFunc (buffer, drop_buffer) % dis->size], buffer, buffer + strlen (buffer) + 1);
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


char* HashFind (HashTable dis, char* str_to_find, __m128i* drop_buffer)
{
	unsigned int hash = dis.HashFunc (str_to_find, drop_buffer) % dis.size;
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

int SSEstrlen (char* str)
{
	int len = 0;
	int seg_len = 16;
	char zero_str[16] = "\0";

	while (seg_len == 16)
	{
		seg_len = _mm_cmpistri (*((__m128i*) str), *((__m128i*) zero_str), _SIDD_CMP_EQUAL_EACH);
		len += seg_len;
		str += 16;
	}

	return len;
}

unsigned int HashFuncMurMur2 (char* str, __m128i* drop_buffer)
{
	int len = SSEstrlen (str);

	const unsigned int mask = 0x5bd1e995;
	const unsigned int seed = 0;
	const int r = 24;

	unsigned int hash = seed ^ len;
	const unsigned char* data = (const unsigned char*) str;

	__m128i k_s = _mm_set_epi32 (data[0], data[4], data[8], data[12]);
	k_s = _mm_or_si128 (_mm_slli_epi32 (_mm_set_epi32 (data[1], data[5], data[9], data[13]), 8), k_s);
	k_s = _mm_or_si128 (_mm_slli_epi32 (_mm_set_epi32 (data[2], data[6], data[10], data[14]), 16), k_s);
	k_s = _mm_or_si128 (_mm_slli_epi32 (_mm_set_epi32 (data[3], data[7], data[11], data[15]), 24), k_s);

	if (!drop_buffer)
		return NULL;

	while (len >= 16)
	{
		k_s = _mm_and_si128 (k_s, _mm_set1_epi32 (mask));
		__m128i k_s_copy = k_s;
		k_s = _mm_srli_epi32 (k_s, r);
		k_s = _mm_xor_si128 (k_s, k_s_copy);
		k_s = _mm_and_si128 (k_s, _mm_set1_epi32 (mask));

		_mm_store_si128 (drop_buffer, k_s);

		hash *= mask;
		hash ^= ((unsigned int*) drop_buffer)[3];

		hash *= mask;
		hash ^= ((unsigned int*) drop_buffer)[2];

		hash *= mask;
		hash ^= ((unsigned int*) drop_buffer)[1];

		hash *= mask;
		hash ^= ((unsigned int*) drop_buffer)[0];

		len -= 16;
		data += 16;
		k_s = *((__m128i*) data);
	}

	k_s = _mm_mullo_epi32 (k_s, _mm_set1_epi32 (mask));
	__m128i k_s_copy = k_s;
	k_s_copy = _mm_srli_epi32 (k_s_copy, r);
	k_s = _mm_xor_si128 (k_s, k_s_copy);
	k_s = _mm_mullo_epi32 (k_s, _mm_set1_epi32(mask));

	_mm_store_si128 (drop_buffer, k_s);
	int offset = 3;

	while (len >= 4)
	{
		hash *= mask;
		hash ^= ((unsigned int*) drop_buffer) [offset];

		data += 4;
		offset--;
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