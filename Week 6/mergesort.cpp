#include <iostream>
#include <cstdlib>
#include <cstring>
#include <time.h>

//------------------------------------------------

void fn_beg();
void fn_end();
bool createMem(size_t size);
bool createList(const char * name, int num_elements);
bool assignVal(const char * list_name, int idx, int val);
int getVal(const char * list_name, int idx);
int freeElem(const char * list_name);
int freeElem();

struct Element
{
    int prev;
    int next;
    int data;

    Element() : prev(-1), next(-1), data(0) {}
    Element(int p, int n, int d = 0) : prev(p), next(n), data(d) {}
    ~Element() = default;
};

struct List
{
    int head;
    int tail;
    int size;

    List() : head(-1), tail(-1), size(0) {}
    List(int h, int t, int s) : head(h), tail(t), size(s) {}
    ~List() = default;
};

struct Tablerow
{
    char name[1<<8];
    List li;

    Tablerow() : name(""), li() {}
    Tablerow(const char * n, List l) : li(l) 
    {
        strcpy(name, n);
    }
    ~Tablerow() = default;
};

struct Table
{
    Tablerow tab[1<<10];
    int size;

    Table() : size(0) {}
    ~Table() = default;

    int find(const char * name)
    {
        for (int i = size-1; i >= 0; --i)   // Search from the end
        {
            if (strcmp(tab[i].name, name) == 0)
            {
                return i;
            }
            // else if(strcmp(name, "fn_call") == 0)   // Search upto the special entry
            // {
            //     return -1;
            // }
        }
        fprintf(stderr, "Table::find: %s not found\n", name);
        return -1;
    }

    int findInScope(const char * name)
    {
        for (int i = size-1; i >= 0; --i)   // Search from the end
        {
            if (strcmp(tab[i].name, name) == 0)
            {
                return i;
            }
            else if(strncmp(tab[i].name, "fn_call", 8) == 0)   // Search upto the special entry
            {
                return -1;
            }
        }
        fprintf(stderr, "Table::findInScope: %s not found\n", name);
        return -1;
    }

};


Table T;
Element * mem;
List freeList;


void fn_beg()
{
    // Add a special entry to table
    T.tab[T.size++] = Tablerow("fn_call", List());
}

void fn_end()
{
    // Remove everything upto the special entry from table using freeElem()
    // freeElem() also has a separate use case: it can be called in any scope to remove all entries in that scope
    freeElem();
}



bool createMem(size_t size)
{
    // Allocate memory
    int eleNum = size / sizeof(Element);
    mem = new(std::nothrow) Element[eleNum];
    if(mem == nullptr) return false;

    // Initialize memory
    for (int i = 0; i < eleNum; i++)
    {
        mem[i].next = i + 1;
        mem[i].prev = i - 1;
    }
    mem[eleNum-1].next = -1;
    
    // Initialize freeList
    freeList.head = 0;
    freeList.tail = eleNum - 1;
    freeList.size = eleNum;

    return true;
}

bool createList(const char * name, int num_elements)
{   
    if(num_elements > freeList.size)
    {
        fprintf(stderr, "createList: Not enough memory to create %s\n", name);
        return false;
    }

    std::cout << "createList: " << name << ": " << T.size << std::endl;
    if(T.findInScope(name) != -1)
    {
        fprintf(stderr, "createList: %s already exists in current function scope\n", name);
        return false;
    }

    // Find start and end pointers from the freeList
    int head = freeList.head;
    int tail = head;
    for (int i = 1; i < num_elements; ++i)
    {
        tail = mem[tail].next;
    }

    // Add to table
    T.tab[T.size++] = Tablerow(name, List(head, tail, num_elements));

    // Update freeList
    freeList.head = mem[tail].next;
    freeList.size -= num_elements;

    return true;
}

bool assignVal(const char * list_name, int idx, int val)
{
    // Find the table row
    int row = T.find(list_name);
    if(row == -1) return false;

    List l = T.tab[row].li;

    // Check if index is within the list size
    if (idx >= l.size)
    {
        fprintf(stderr, "assignVal: Index out of bounds in %s\n", list_name);
        return false;
    }

    // Assign value in the memory
    mem[l.head + idx].data = val;
    return true;
}

int getVal(const char * list_name, int idx)
{
    // Find the table row
    int row = T.find(list_name);
    if(row == -1)
    {
        return -1;
    }

    List l = T.tab[row].li;

    // Check if index is within the list size
    if (idx >= l.size)
    {
        fprintf(stderr, "getVal: Index out of bounds in list %s\n", list_name);
        return -1;
    }

    // Get value from the memory
    return mem[l.head + idx].data;
}

int freeElem(const char * list_name)
{
    // Find the table row (searching upto the special entry)
    int row = T.find(list_name);
    if(row == -1) return 0;

    List l = T.tab[row].li;

    // Update freeList
    mem[freeList.tail].next = l.head;
    freeList.size += l.size;
    freeList.tail = l.tail;

    // Remove from table
    T.tab[row] = T.tab[--T.size];

    return 1;
}

int freeElem()
{
    // Delete everything upto the special entry or upto the first entry (in case current scope is global scope)
    int deleted = 0;
    while(strcmp(T.tab[T.size-1].name, "fn_call") != 0 || T.size == 0)
    {
        --T.size;
        ++deleted;
    }
    if(T.tab[T.size-1].name == "fn_call")
    {
        --T.size;
        ++deleted;
    }

    return deleted;
}


void printTable()
{
    std::cout << "Table:" << std::endl;
    std::cout << "Name \t Head \t Tail \t Size" << std::endl;
    for (int i = 0; i < T.size; ++i)
    {
        std::cout << T.tab[i].name << " " << T.tab[i].li.head << " " << T.tab[i].li.tail << " " << T.tab[i].li.size << std::endl;
    }
}

//------------------------------------------------





void merge(const char * big_list, const char * left, const char * right, int left_size, int right_size)
{
    int i = 0, j = 0, k = 0;
    while (i < left_size && j < right_size)
    {
        int ret = getVal(left, i);
        int ret2 = getVal(right, j);
        if(ret == -1 || ret2 == -1) 
        {
            fprintf(stderr, "merge\n");
            exit(0);
        }
        if (getVal(left, i) < getVal(right, j))
        {
            assignVal(big_list, k, getVal(left, i));
            ++i;
        }
        else
        {
            assignVal(big_list, k, getVal(right, j));
            ++j;
        }
        ++k;
    }

    while (i < left_size)
    {
        assignVal(big_list, k, getVal(left, i));
        ++i;
        ++k;
    }

    while (j < right_size)
    {
        assignVal(big_list, k, getVal(right, j));
        ++j;
        ++k;
    }
}

void mergesort(const char * list_name, int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;

        printTable();
        std::cout << "mergesort: " << list_name << " " << start << " " << end << std::endl;

        createList("left", mid - start + 1);
        createList("right", end - mid);

        for (int i = start; i <= mid; ++i)
        {
            int ret = getVal(list_name, i);
            if(ret == -1)
            {
                fprintf(stderr, "mergesort1\n");
                exit(0);
            }
            assignVal("left", i - start, getVal(list_name, i));
        }

        for (int i = mid + 1; i <= end; ++i)
        {
            int ret = getVal(list_name, i);
            if(ret == -1)
            {
                fprintf(stderr, "mergesort2\n");
                exit(0);
            }
            assignVal("right", i - mid - 1, getVal(list_name, i));
        }

        fn_beg();
        mergesort("left", start, mid);
        fn_end();

        fn_beg();
        mergesort("right", mid + 1, end);
        fn_end();

        fn_beg();
        merge(list_name, "left", "right", mid - start + 1, end - mid);
        fn_end();
    }
}


int main()
{
    // Create a list of random numbers of size 100 KB
    srand(time(NULL));
    size_t size = 100 * 1024;
    createMem(size);
    createList("mylist", 10);
    for (int i = 0; i < 10; ++i)
    {
        assignVal("mylist", i, rand() % 1000);
    }

    // Sort the list
    fn_beg();
    mergesort("mylist", 0, 9);
    fn_end();

    // Print the sorted list
    for (int i = 0; i < 10; ++i)
    {
        std::cout << getVal("mylist", i) << " ";
    }
    std::cout << std::endl;



    return 0;
}