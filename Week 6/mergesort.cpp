#include <chrono>
#include <ctime>
#include <iostream>

#include "goodmalloc.hpp"

constexpr int32_t SIZE = 50'000;

void merge(
	const char *big_list,
	const char *left_list,
	const char *right_list)
{
	ptr_t start = listBegin(big_list);
	ptr_t left_start = listBegin(left_list), left_end = listEnd(left_list);
	ptr_t right_start = listBegin(right_list), right_end = listEnd(right_list);

	while (left_start != left_end and right_start != right_end)
	{
		if (listGetElem(left_start) < listGetElem(right_start))
		{
			listSetElem(start, listGetElem(left_start));
			left_start = listNext(left_start);
		}
		else
		{
			listSetElem(start, listGetElem(right_start));
			right_start = listNext(right_start);
		}
		start = listNext(start);

		// if (left_start == left_end or right_start == right_end)
		// 	break;
	}

	// std::cerr << "Merge1\n";

	while (left_start != left_end)
	{
		listSetElem(start, listGetElem(left_start));
		left_start = listNext(left_start);
		start = listNext(start);

		// if (left_start == left_end)
		// 	break;
	}

	// std::cerr << "Merge2\n";

	while (right_start != right_end)
	{
		listSetElem(start, listGetElem(right_start));
		right_start = listNext(right_start);
		start = listNext(start);

		// if (right_start == right_end)
		// 	break;
	}

	// std::cerr << "Merge3\n";
}

void mergesort(
	const char *list_name, int32_t sz,
	ptr_t start, ptr_t end)
{
	// printTable();

	if (sz <= 1)
		return;

	int32_t mid = listPtr(list_name, sz / 2);
	int32_t left_size = sz / 2;
	int32_t right_size = sz - left_size;
	// std::fprintf(stderr, "List: %s, Start: %d, Mid: %d, End: %d\n",
	// 			 list_name, start, mid, end);

	char left[1 << 12]{}, right[1 << 12]{};
	std::sprintf(left, "%s_l", list_name);
	std::sprintf(right, "%s_r", list_name);

	createList(left, left_size);
	// printTable();

	for (ptr_t i = start, j = listBegin(left); i != mid; i = listNext(i), j = listNext(j))
	{
		listSetElem(j, listGetElem(i));

		// if (i == mid)
		// 	break;
	}

	// std::cerr << "left: \n";

	createList(right, right_size);
	// printTable();

	for (ptr_t i = mid, j = listBegin(right); i != end; i = listNext(i), j = listNext(j))
	{
		listSetElem(j, listGetElem(i));

		// if (i == end)
		// 	break;
	}

	// std::cerr << "right: \n";

	fn_beg();
	mergesort(left, left_size, listBegin(left), listEnd(left));
	fn_end();
	fn_beg();
	mergesort(right, right_size, listBegin(right), listEnd(right));
	fn_end();

	fn_beg();
	merge(list_name, left, right);
	fn_end();
}

int main()
{
	// Create a list of random numbers of size 250 MB
	// srand(time(nullptr));

	size_t size = (250ull << 20);
	createMem(size);
	createList("mylist", SIZE);

	for (ptr_t i = listBegin("mylist"); i != listEnd("mylist"); i = listNext(i))
	{
		int num = rand() % (SIZE * 2);
		listSetElem(i, num);
	}

	// Print the list
	printList("mylist");
	std::cout << std::endl;

	// printTable();

	// std::fprintf(stderr, "Start: %d, End: %d\n", listBegin("mylist"), listEnd("mylist"));

	// Sort the list
	auto start = std::chrono::high_resolution_clock::now();
	fn_beg();
	mergesort("mylist", SIZE, listBegin("mylist"), listEnd("mylist"));
	fn_end();
	auto end = std::chrono::high_resolution_clock::now();

	// Print the sorted list
	printList("mylist");
	std::cout << std::endl;

	// Print the time taken
	fprintf(stderr, "Time taken: %lld ms\n",
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	freeElem();

	deleteMem();
}
