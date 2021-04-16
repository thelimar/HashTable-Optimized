#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif

#define IsListOK(list) if (ListVerificator (list))                                                                 \
					   {                                                                                           \
                           printf ("LIST ERROR #%d\r\n"                                                            \
								   "Caught it on line %d, func %s", ListVerificator (list), __LINE__, __FUNCSIG__);\
						   return (ListVerificator (list));                                                        \
					   }


#define list_data_format "%s"

#define strequ(str1, str2) !strcmp(str1, str2)

enum ListErrors
{
	SIZE_TOO_BIG   = 1,
	SIZE_TOO_SMALL = 2,
};

typedef char* list_data;

struct ListNode
{
	list_data data_one;
	list_data data_two;
	ListNode* next;
};

struct List
{
	size_t size;
	ListNode* head;
	ListNode* tail;
};

List ListBuild ()
{
	List dis{};
	dis.size = 0;
	dis.head = nullptr;
	dis.tail = nullptr;

	return dis;
}

ListNode* ListNodeBuild (list_data node_data_one, list_data node_data_two)
{
	ListNode* node = (ListNode*) calloc (1, sizeof (ListNode));
	if (node)
	{
		node->data_one = node_data_one;
		node->data_two = node_data_two;
		return node;
	}
	else
		return NULL;
}

void ListNodeDestruct (ListNode* dis)
{
	if (dis->next)
		ListNodeDestruct (dis->next);

	dis->data_one = NULL;
	dis->data_two = NULL;
	free (dis);
}

void ListDestruct (List* dis)
{	
	if (dis->head)
		ListNodeDestruct (dis->head);
	dis->size = 0;
}

int ListVerificator (List dis)
{
	if (dis.size == 0 && (dis.head != nullptr || dis.tail != nullptr))
		return SIZE_TOO_SMALL;

	ListNode* next_node = dis.head;
	for (unsigned int i = 0; i < dis.size - 1; i++)
	{
		if (next_node && !next_node->next)
			return SIZE_TOO_BIG;
		if (next_node)
			next_node = next_node->next;
	}
	if (next_node && next_node->next)
		return SIZE_TOO_SMALL;

	return 0;
}

int ListPush (List* dis, list_data node_data_one, list_data node_data_two)
{
	assert (dis);
	DBG IsListOK (*dis)

	ListNode* new_node = ListNodeBuild (node_data_one, node_data_two);
	if (!dis->size)
	{
		dis->head = new_node;
		dis->tail = new_node;
		dis->size = 1;
	}
	else
	{
		dis->tail->next = new_node;
		(dis->size)++;
		dis->tail = new_node;
	}

	return 0;
}

int ListDeleteHead (List* dis)
{
	assert (dis);
	DBG IsListOK (*dis)

	ListNode* temp_node = dis->head;
	if (temp_node)
	{
		dis->head = temp_node->next;
		free(dis->head);
		(dis->size)--;
	}

	return 0;
}

int ListDump (List dis)
{
	ListNode* next_node = dis.head;
	for (unsigned int i = 0; i < dis.size; i++)
	{
		if (next_node)
		{
			printf(list_data_format ", " list_data_format ", ", next_node->data_one, next_node->data_two);
			next_node = next_node->next;
		}
	}
	printf ("\r\n");

	return 0;
}

list_data ListFind (List dis, list_data find_data)
{
	ListNode* next_node = dis.head;
	for (int i = 0; i < dis.size; i++)
	{
		if (strequ (next_node->data_one, find_data))
			return next_node->data_two;
		else
			next_node = next_node->next;
	}

	return NULL;
}