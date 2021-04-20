#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "oneg.h"
#include "HashTable.h"


int main()
{
	setlocale(LC_ALL, "");

	HashTable Table = HashTableBuild (HashFuncMurMur2, HASH_TABLE_SIZE);
	FILE* dict = fopen("ENRUS.txt", "rb");
	int nLines = 0;
	int nChars = 0;
	__m256i* dict_buffer = (__m256i*) PoemEnter (&nChars, &nLines, dict);

	__m256i* drop_buffer = (__m256i*) _aligned_malloc (8 * sizeof (int), 16);

	if (!drop_buffer)
	{
		printf("Cant't allocate memory for drop buffer\r\n");
		return -1;
	}

	HashTableLoad (&Table, dict_buffer, nLines, drop_buffer);
	fclose (dict);

	FILE* output = fopen("HashMurMur2.txt", "wb");
	CollisionDump (Table, output);
	fclose (output);

	char input_str[MAX_WORD_LEN] = "perspective";
	int len_input = strlen (input_str);
	printf ("%s\r\n", (char*) HashFind (Table, (__m256i*) input_str, len_input, drop_buffer));
	for (int i = 0; i < 0.9 * 10000000; i++)
	{
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'a';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'b';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'c';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'd';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'e';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);

		*input_str = 'f';
		HashFind (Table, (__m256i*) input_str, len_input, drop_buffer);
	}

	HashTableDestruct (&Table);

	_aligned_free (drop_buffer);
	_aligned_free (dict_buffer);

	return 0;
}