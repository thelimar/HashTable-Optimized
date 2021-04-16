#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

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

enum ListErrors
{
	SIZE_TOO_BIG = 1,
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

List ListBuild ();
ListNode* ListNodeBuild (list_data node_data_one, list_data node_data_two);
void ListNodeDestruct (ListNode* dis);
void ListDestruct (List* dis);
int ListVerificator (List dis);
int ListPush (List* dis, list_data node_data_one, list_data node_data_two);
int ListDeleteHead (List* dis);
int ListDump (List dis);
list_data ListFind (List dis, list_data find_data);
