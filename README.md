# HashTable-Optimized
Optimizing English to Russian dictionary project based on hash table using SSE instructions

# Table of contents
1. [Introduction](#intro)
2. [Unoptimized project](#unopt)
3. [Getting started](#start)
4. [HashFunc optimization](#hashfunc)
5. [HashFind optimization](#hashfind)
6. [Conclusion](#conclusion)
7. [Links](#links)

<a name="intro"></a>
## 1. Introduction

I have made a project that uses chained hash table with English words from a dictionary as keys and their corresponding Russian translation as values. With a simple function of finding a value with a key we get a very simplistic word-by-word translator. After getting a functioning version of a project I decided to optimize it using SSE instructions.

<a name="unopt"></a>
## 2. Unoptimized project

To start with let's make it clear on the initial version of a project. The main structure being used is HashTable. It has 3 fields: its size, its hash function and an array of linked lists. When we are filling the hash table we are taking the English word, computing its hash and then pushing the Russian word into list with index equal to the remainder of the division of the hash by hash table's size. 

![Hash table structure](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/HashStruct.jpg?raw=true)

The logical question is: why do we use lists? Isn't it simpler and faster just to have an array of strings? The thing is sometimes different words will have the exact remainder of the division of the hash by hash table's size - this is called **_collision_**. In order to deal with it we are using linked list, so, in case the spot in array for the value is already occupied, we can just link the new value to the older one.
So, due to the way we handle collisions the efficiency of our hash table depends heavily on the **_spread_** of a hash function used. This means that the more frequently the hash functions returns the same value for different words the slower is our find function. That's why for this project we have to choose the most spread-efficient hash function out there. In my case, I have chosen MurmurHash2. You can find the source code of it [here](https://en.wikipedia.org/wiki/MurmurHash#:~:text=MurmurHash%20is%20a%20non%2Dcryptographic%20hash,used%20in%20its%20inner%20loop). You can see the collision graph for my hash table that used MurmurHash2 to store 51381 word pairs. (you can find the dictionary I used [here](https://github.com/thelimar/HashTable-Optimised/blob/main/ENRUS.TXT))

![CollisionGraph](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/MurMur.png?raw=true)

<a name="start"></a>
## 3. Getting started

So, now we are ready to face the problem – optimizing our project. The key tool that will help us in this complicated task is **_profiler_**. Profiler is a tool that shows you how much program working time some function is taking, memory and CPU usage, etc. I am coding in Visual Studio 2019 Community edition and it has built in _free_ profiler! You can access it with Debug -> Performance Profiler or with Alt+F2. Another thing we need is a stress test – a program that will emulate long usage of our hash table. When writing a stress test it is important to remember what are the most called by user functions of your program, so you can call them more in order to emulate user's behavior more precisely. In my case, the most used function is finding a value with a key, so in the stress test I call 70 million times with "different" arguments, as user wouldn't search one and the same word over and over again. It looks like this:
```
	char input_str[MAX_WORD_LEN] = "perspective";

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
```

Alright, now when profiler report will focus exactly on our most used by user find function rather than loading table which usually happens only one time per session we are finally ready to profile our project. Another important thing, though, is to switch off all debug functions such as verifiers, as they won't be present in the final build. All compilation from now on will be performed in Release mode with Visual Studio's /O2 optimization that favors speed. (because what's the use of optimizing something if compiler can make it better than you)

![OptimisationChosen](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/OptimisationChosen.jpg?raw=true)

The Instrumentation option of Visual Studio's profiler gives us the following results on the initial project version:

![InitialReport](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/InitialReport.jpg?raw=true)

As you can see, with the total working time of 43.3 seconds, 40% and 30% of this time is taken by HashFind and HashFuncMurMur2 respectively. Let's start with optimizing HashFuncMurMur2 as it doesn't call any other function and eats up almost the same amount of time as the leader – HashFind.

<a name="hashfunc"></a>
## 4. HashFunc optimization

This is the initial code of HashFuncMurMur2:
```
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
```

After what seemed like an eternity of debugging I finally came up with this code that uses SSE instructions:
```
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
```
As you can see I even had to change the prototype because I needed buffer to store 4 of my integers from _m128i_, and in order not to allocate and free memory every time the hash function is called (which takes up much longer time than we win with optimization) I decided to have buffer as an argument, so the allocation will happen only once. 
So, the Profiler Report says that now program runs faster by... 2.5 seconds which is roughly 1.07 times faster.

![SSEMurMurReport](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/Intermediate.jpg?raw=true)

Well, ![HonestWork](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/HonestWork.jpg?raw=true)

We are probably seeing such pitiful result as the words from dictionary are not very long and MururHash2 basically already deals with 4 letters simultaneously, so the advantage of using SSE is not that noticeable.
Currently, I don't have any idea how to optimize MurmurHash2 any further. Rewriting strlen() with SSE only made the program slightly slower (by ~1 second). So, now we have nothing to do but move on to optimizing HashFind!

<a name="hashfind"></a>
## 5. HashFind optimization

This is the initial code for HashFind and ListFind (the second one is inlined by the compiler, so that's why we don't see it in profiler report)
```
char* HashFind (HashTable dis, char* str_to_find, __m128i* drop_buffer)
{
	unsigned int hash = dis.HashFunc (str_to_find, drop_buffer) % dis.size;
	if (dis.DataArray[hash].size == 0)
		return NULL;
	else
		return ListFind (dis.DataArray[hash], str_to_find);
}

list_data ListFind (List dis, list_data find_data)
{
	ListNode* next_node = dis.head;
	for (int i = 0; i < dis.size; i++)
	{
		if (!strcmp (next_node->data_one, find_data))
			return next_node->data_two;
		else
			next_node = next_node->next;
	}

	return NULL;
}
```
One of the slowest code fragments is definitely strcmp(), so I rewrote it with SSE4.2 the most efficient way possible:
```
list_data ListFind (List dis, list_data find_data)
{
	ListNode* next_node = dis.head;
	for (int i = 0; i < dis.size; i++)
	{
		if ((_mm_cmpistri (*((__m128i*) next_node->data_one), *((__m128i*) find_data), _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY) == 16))
			return next_node->data_two;
		else
			next_node = next_node->next;
	}

	return NULL;
}
```
And that's where the miracle comes. The report showed 29 seconds acceleration and now the program runs 3.45 times faster!

![FinalReport](https://github.com/thelimar/HashTable-Optimised/blob/main/Images/FinalReport.jpg?raw=true)

Attempts to speed the program up even more by rewriting HashFind on pure assembler resulted in either slowing program down or breaking it completely, so the optimization can be considered over at this point.

<a name="conclusion"></a>
## 6. Conclusion

In the end, the usage of SSE instructions gave us **3.69 times accelaration** on top of \O2 optimization.

<a name="links"></a>
## 7. Links

2. [MurmurHash wikipedia page](https://en.wikipedia.org/wiki/MurmurHash#:~:text=MurmurHash%20is%20a%20non%2Dcryptographic%20hash,used%20in%20its%20inner%20loop)
3. [Visual Studio](https://visualstudio.microsoft.com)
4. [The place I took the dictionary from](http://blog.kislenko.net/show.php?id=136)
