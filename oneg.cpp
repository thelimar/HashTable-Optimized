#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <intrin.h>

char* PoemEnter (int* nChars, int* nLines, FILE* source)
{
	fseek (source, '\0', SEEK_END);
	long source_size = ftell (source);
	rewind (source);

	char* text_buffer = (char*) calloc (source_size + 2, sizeof(char));
	if (!text_buffer)
	{
		return NULL;
	}

	if (source_size * sizeof(char) != fread (text_buffer, sizeof(char), source_size, source))
	{
		return NULL;
	}

	for (int index = 0; index < source_size; index++)
	{
		if (text_buffer[index] == '\n')
		{
			text_buffer[index] = '\0';
			(*nLines)++;
		}
	}

	text_buffer[source_size] = '\r';

	char* parsed_text_buffer = (char*) _aligned_malloc (*nLines * 32 * sizeof(char), 16);

	if (!parsed_text_buffer)
		return NULL;

	for (int i = 0; i < *nLines * 32 * sizeof(char); i++)
		parsed_text_buffer[i] = '\0';

	int parsed_index = 0;
	int source_index = 0;
	int current_index = 0;

	while (source_index < source_size + 1)
	{
		current_index = 0;

		while (current_index < 32)
		{
			if (text_buffer[source_index + current_index] == '\r')
				break;

			parsed_text_buffer[parsed_index + current_index] = text_buffer[source_index + current_index];
			current_index++;
		}

		source_index += current_index;
		parsed_index += current_index;

		while (text_buffer[source_index] != '\r')
			source_index++;

		source_index += 2;

		while (current_index < 32)
		{
			parsed_text_buffer[parsed_index] = '\0';
			parsed_index++;
			current_index++;
		}
	}

	*nChars = source_size + 1;
	(*nLines)++;

	free (text_buffer);

	return parsed_text_buffer;
}

void PoemWriteSource (char* poem_in, int nChars, FILE* output)
{

	char message[] = "Ne bey, original ne isporchen:\n\n";
	fputs(message, output);

	for (int i = 0; i < nChars; i++)
	{
		fprintf(output, "%s", poem_in + i);
		i = i + strlen(poem_in + i);
	}
}