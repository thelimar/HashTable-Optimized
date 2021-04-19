#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "oneg.h"
#include "HashTable.h"


int main ()
{
	setlocale (LC_ALL, "");

	HashTable Table = HashTableBuild (HashFuncMurMur2, HASH_TABLE_SIZE);
	FILE* dict = fopen ("ENRUS.txt", "rb");
	int nLines = 0;
	int nChars = 0;
	char* dict_buffer = PoemEnter (&nChars, &nLines, dict);

	__m128i* drop_buffer = (__m128i*) _aligned_malloc (4 * sizeof(int), 16);

	if (!drop_buffer)
	{
		printf("Cant't allocate memory for drop buffer\r\n");
		return -1;
	}

	HashTableLoad (&Table, dict_buffer, nLines, drop_buffer);
	fclose (dict);

	FILE* output = fopen ("HashMurMur2.txt", "wb");
	CollisionDump (Table, output);
	fclose (output);

	char input_str[MAX_WORD_LEN] = "perspective";

	printf ("%s\r\n", HashFind (Table, input_str, drop_buffer));
	for (int i = 0; i < 10000000; i++)
	{
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'a';
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'b';
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'c';
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'd';
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'e';
		HashFind (Table, input_str, drop_buffer);

		*input_str = 'f';
		HashFind (Table, input_str, drop_buffer);
	}

	HashTableDestruct (&Table);

	_aligned_free (drop_buffer);

	return 0;
}