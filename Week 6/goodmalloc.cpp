#include <cstdlib>
#include <cstring>
#include <iostream>

//------------------------------------------------
#include "goodmalloc.hpp"
constexpr int32_t TABLESIZE = (1 << 15);
constexpr int32_t NAMESIZE = (1 << 12);

struct Element
{
	int32_t prev{-1};
	int32_t next{-1};
	int32_t data{0};

	Element() = default;
	Element(int32_t p, int32_t n, int32_t d = 0)
		: prev{p}, next{n}, data{d} {}
	~Element() = default;
};

struct List
{
	int32_t head{-1};
	int32_t tail{-1};
	int32_t size{0};

	List() = default;
	List(int32_t h, int32_t t, int32_t s)
		: head{h}, tail{t}, size{s} {}
	~List() = default;
};

struct Tablerow
{
	char name[NAMESIZE]{};
	List li{};

	Tablerow() = default;
	Tablerow(const char *n, const List &l)
		: li{l} { std::strcpy(name, n); }
	~Tablerow() = default;
};

struct Table
{
	Tablerow tab[TABLESIZE]{};
	int32_t size{0};

	Table() = default;
	~Table() = default;

	int32_t find(const char *name)
	{
		for (int32_t i = size - 1; i >= 0; --i) // Search from the end
		{
			if (std::strcmp(tab[i].name, name) == 0)
			{
				return i;
			}
		}
		// fprintf(stderr, "Table::find: %s not found\n", name);
		return -1;
	}

	int32_t findInScope(const char *name)
	{
		for (int32_t i = size - 1; i >= 0; --i) // Search from the end
		{
			if (std::strcmp(tab[i].name, name) == 0)
			{
				return i;
			}
			else if (std::strncmp(tab[i].name, "__fn_call", 8) == 0) // Search upto the special entry
			{
				return -1;
			}
		}
		// fprintf(stderr, "Table::findInScope: %s not found\n", name);
		return -1;
	}
};

Table T{};
Element *mem{nullptr};
List freeList{};

void fn_beg()
{
	std::fprintf(stderr, "fn_beg() called\n");
	// Add a special entry to table
	if(T.size == TABLESIZE - 1)
	{
		std::fprintf(stderr, "fn_beg: Table full\n");
		exit(EXIT_FAILURE);
	}
	T.tab[T.size++] = Tablerow("__fn_call", List());
}

void fn_end()
{
	std::fprintf(stderr, "fn_end() called\n");
	// Remove everything upto the special entry from table using freeElem()
	// freeElem() also has a separate use case: it can be called in any scope to remove all entries in that scope
	freeElem();
	if (std::strcmp(T.tab[T.size - 1].name, "__fn_call") == 0)
	{
		--T.size;
	}
}

bool createMem(size_t size)
{
	std::fprintf(stderr, "createMem(%d) called\n", size);
	// Allocate memory
	int32_t eleNum = size / sizeof(Element);
	try
	{
		mem = new Element[eleNum];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}

	// Initialize memory
	for (int32_t i = 0; i < eleNum; i++)
	{
		mem[i].next = i + 1;
		mem[i].prev = i - 1;
	}
	mem[eleNum - 1].next = -1;

	// Initialize freeList
	freeList.head = 0;
	freeList.tail = eleNum - 1;
	freeList.size = eleNum;

	return true;
}

bool createList(const char *name, int32_t num_elements)
{
	std::fprintf(stderr, "createList(%s, %d) called\n", name, num_elements);
	if (num_elements > freeList.size)
	{
		std::fprintf(stderr, "createList: Not enough memory to create %s\n", name);
		exit(EXIT_FAILURE);
	}

	if (T.findInScope(name) != -1)
	{
		std::fprintf(stderr, "createList: %s already exists in current function scope\n", name);
		exit(EXIT_FAILURE);
	}

	if (T.size == TABLESIZE - 1)
	{
		std::fprintf(stderr, "createList: Table full\n");
		exit(EXIT_FAILURE);
	}

	// Find start and end pointers from the freeList
	int32_t head = freeList.head;
	int32_t tail = head;
	for (int32_t i = 1; i < num_elements; ++i)
	{
		tail = mem[tail].next;
	}

	// Add to table
	T.tab[T.size++] = Tablerow{name, List{head, tail, num_elements}};

	// Update freeList
	freeList.head = mem[tail].next;
	freeList.size -= num_elements;

	return true;
}

bool assignVal(const char *list_name, int32_t idx, int32_t val)
{
	std::fprintf(stderr, "assignVal(%s, %d, %d) called\n", list_name, idx, val);
	// Find the table row
	int32_t row = T.find(list_name);
	if (row == -1)
		return false;

	List l = T.tab[row].li;

	// Check if index is within the list size
	if (idx >= l.size)
	{
		fprintf(stderr, "assignVal: Index out of bounds in %s\n", list_name);
		exit(EXIT_FAILURE);
	}

	// Assign value in the memory
	// Find the position by parsing linked list
	int32_t ptr = l.head;
	for (int32_t i = 0; i < idx; ++i)
	{
		ptr = mem[ptr].next;
	}
	mem[ptr].data = val;
	return true;
}

int32_t getVal(const char *list_name, int32_t idx)
{
	std::fprintf(stderr, "getVal(%s, %d) called\n", list_name, idx);
	// Find the table row
	int32_t row = T.find(list_name);
	if (row == -1)
	{
		fprintf(stderr, "getVal: %s not found\n", list_name);
		exit(EXIT_FAILURE);
	}

	List l = T.tab[row].li;

	// Check if index is within the list size
	if (idx >= l.size)
	{
		fprintf(stderr, "getVal: Index out of bounds in list %s\n", list_name);
		exit(EXIT_FAILURE);
	}

	// Get value from the memory, Parse the linked list
	int32_t ptr = l.head;
	for (int32_t i = 0; i < idx; ++i)
	{
		ptr = mem[ptr].next;
	}
	return mem[ptr].data;
}

int32_t freeElem(const char *list_name)
{
	std::fprintf(stderr, "freeElem(%s) called\n", list_name);
	// Find the table row
	int32_t row = T.find(list_name);
	if (row == -1)
		return 0;

	List l = T.tab[row].li;

	// Update freeList
	mem[freeList.tail].next = l.head;
	freeList.size += l.size;
	freeList.tail = l.tail;

	// Remove from table
	T.tab[row] = T.tab[--T.size];

	return 1;
}

int32_t freeElem()
{
	std::fprintf(stderr, "freeElem() called\n");
	// Delete everything upto the special entry or
	// upto the first entry (in case current scope is global scope)
	int32_t deleted = 0;
	while (std::strcmp(T.tab[T.size - 1].name, "__fn_call") != 0)
	{
		--T.size;
		++deleted;

		if (T.size == 0)
			return deleted;
	}

	return deleted;
}

void printList(const char *list_name)
{
	std::fprintf(stderr, "printList(%s) called\n", list_name);
	// Find the table row
	int32_t row = T.find(list_name);
	if (row == -1)
		return;

	List l = T.tab[row].li;

	// Print the list
	std::printf("List: %s\n", list_name);
	for (int32_t i = l.head;; i = mem[i].next)
	{
		std::printf("%3d ", mem[i].data);
		if (i == l.tail)
			break;
	}
	std::printf("END\n");
}

void deleteMem()
{
	if (mem)
		delete[] mem;
}

void printTable()
{
	std::printf("-------------Table-----------\n");
	std::printf("Name \t Head \t Tail \t Size\n");
	for (int32_t i = 0; i < T.size; ++i)
	{
		std::printf("%s \t %d \t %d \t %d\n",
					T.tab[i].name,
					T.tab[i].li.head,
					T.tab[i].li.tail,
					T.tab[i].li.size);
	}
	std::printf("-----------------------------\n");
}
