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
	HashTableLoad (&Table, dict_buffer, nLines);
	fclose (dict);

	FILE* output = fopen ("HashMurMur2.txt", "wb");
	CollisionDump (Table, output);
	fclose (output);

	char input_str[MAX_WORD_LEN] = "perspective";

	printf ("%s\r\n", HashFind (Table, input_str));
	for (int i = 0; i < 10000000; i++)
	{
		HashFind (Table, input_str);

		*input_str = 'a';
		HashFind (Table, input_str);

		*input_str = 'b';
		HashFind (Table, input_str);

		*input_str = 'c';
		HashFind (Table, input_str);

		*input_str = 'd';
		HashFind (Table, input_str);

		*input_str = 'e';
		HashFind (Table, input_str);

		*input_str = 'f';
		HashFind (Table, input_str);
	}

	HashTableDestruct (&Table);

	return 0;
}