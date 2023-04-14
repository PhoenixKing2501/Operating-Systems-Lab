#include <algorithm>
#include <ctime>
#include <chrono>
#include <iostream>

#include "goodmalloc.hpp"

constexpr int32_t SIZE = 2500;

void merge(
	const char *big_list,
	const char *left,
	const char *right,
	int32_t left_size,
	int32_t right_size)
{
	int32_t i = 0, j = 0, k = 0;

	while (i < left_size and j < right_size)
	{
		int32_t ret1 = getVal(left, i);
		int32_t ret2 = getVal(right, j);

		if (ret1 == -1 or ret2 == -1)
		{
			std::fprintf(stderr, "merge\n");
			exit(0);
		}

		if (getVal(left, i) < getVal(right, j))
		{
			assignVal(big_list, k++, getVal(left, i++));
			// ++i;
		}
		else
		{
			assignVal(big_list, k++, getVal(right, j++));
			// ++j;
		}
		// ++k;
	}

	while (i < left_size)
	{
		assignVal(big_list, k++, getVal(left, i++));
		// ++i;
		// ++k//;
	}

	while (j < right_size)
	{
		assignVal(big_list, k++, getVal(right, j++));
		// ++j;
		// ++k;
	}
}

void mergesort(
	const char *list_name,
	int32_t start, int32_t end)
{
	if (start < end)
	{
		int32_t mid = (start + end) / 2;
		int32_t left_size = mid - start + 1;
		int32_t right_size = end - mid;

		createList("left", left_size);

		for (int32_t i = start; i <= mid; ++i)
		{
			int32_t ret = getVal(list_name, i);
			if (ret == -1)
			{
				std::fprintf(stderr, "mergesort1\n");
				exit(0);
			}
			assignVal("left", i - start, getVal(list_name, i));
		}

		createList("temp", left_size);

		for (int32_t i = 0; i < left_size; ++i)
		{
			assignVal("temp", i, getVal("left", i));
		}

		fn_beg();
		mergesort("temp", 0, left_size - 1);
		fn_end();

		for (int32_t i = 0; i < left_size; ++i)
		{
			assignVal("left", i, getVal("temp", i));
		}

		freeElem("temp");

		createList("right", right_size);
		for (int32_t i = mid + 1; i <= end; ++i)
		{
			int32_t ret = getVal(list_name, i);
			if (ret == -1)
			{
				fprintf(stderr, "mergesort2\n");
				exit(0);
			}
			assignVal("right", i - mid - 1, getVal(list_name, i));
		}

		createList("temp", right_size);

		for (int32_t i = 0; i < right_size; ++i)
		{
			assignVal("temp", i, getVal("right", i));
		}

		fn_beg();
		mergesort("temp", 0, right_size - 1);
		fn_end();

		for (int32_t i = 0; i < right_size; ++i)
		{
			assignVal("right", i, getVal("temp", i));
		}

		freeElem("temp");

		fn_beg();
		merge(list_name, "left", "right", mid - start + 1, end - mid);
		fn_end();
	}
}

int main()
{
	// Create a list of random numbers of size 250 MB
	srand(time(nullptr));

	size_t size = (250ull << 20);
	createMem(size);
	createList("mylist", SIZE);

	int arr[SIZE]{};

	for (int32_t i = 0; i < SIZE; ++i)
	{
		int num = rand() % 1000;
		assignVal("mylist", i, num);
		arr[i] = num;
	}

	// Print the list
	printList("mylist");
	std::cout << std::endl;

	// Sort the list
	auto start = std::chrono::high_resolution_clock::now();
	fn_beg();
	mergesort("mylist", 0, SIZE - 1);
	fn_end();
	auto end = std::chrono::high_resolution_clock::now();

	// sort the array
	std::sort(std::begin(arr), std::end(arr));

	// Print the sorted list
	printList("mylist");
	std::cout << std::endl;

	// Check if the list is sorted
	for (int32_t i = 0; i < SIZE; ++i)
	{
		if (getVal("mylist", i) != arr[i])
		{
			std::cout << "Not sorted" << std::endl;
			return 0;
		}
	}

	std::cout << "Sorted" << std::endl;

	// Print the time taken
	fprintf(stderr, "Time taken: %lld ms\n",
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

	freeElem();

	deleteMem();
}
