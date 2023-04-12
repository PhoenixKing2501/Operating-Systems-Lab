#include <iostream>
#include <cstdlib>
#include <cstring>

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
        fprintf(stderr, "Not enough memory to create list %s", name);
        return false;
    }

    if(T.find(name) != -1)
    {
        fprintf(stderr, "List %s already exists in current function scope", name);
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
        fprintf(stderr, "assignVal: Index out of bounds");
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
    if(row == -1) return -1;

    List l = T.tab[row].li;

    // Check if index is within the list size
    if (idx >= l.size)
    {
        fprintf(stderr, "getVal: Index out of bounds");
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


// A function to test
void fn()
{
    createList("list1", 10);
    createList("list2", 10);
}



int main()
{
    if(!createMem(1<<20))
    {
        fprintf(stderr, "Failed to create memory");
        return 1;
    }

    std::cout << "Memory created" << std::endl;

    std::cout << "Head of freeList: " << freeList.head << std::endl;
    std::cout << "Tail of freeList: " << freeList.tail << std::endl;

    if(!createList("list1", 10))
    {
        fprintf(stderr, "Failed to create list1");
        return 1;
    }

    if(!createList("list2", 10))
    {
        fprintf(stderr, "Failed to create list2");
        return 1;
    }

    std::cout << "Lists created" << std::endl;

    std::cout << "Head of list1: " << T.tab[T.find("list1")].li.head << std::endl;
    std::cout << "Tail of list1: " << T.tab[T.find("list1")].li.tail << std::endl;

    std::cout << "Head of list2: " << T.tab[T.find("list2")].li.head << std::endl;
    std::cout << "Tail of list2: " << T.tab[T.find("list2")].li.tail << std::endl;

    std::cout << "Head of freeList: " << freeList.head << std::endl;
    std::cout << "Tail of freeList: " << freeList.tail << std::endl;

    printTable();

    fn_beg();
    fn();
    printTable();
    fn_end();

    if(!assignVal("list1", 0, 10))
    {
        fprintf(stderr, "Failed to assign value to list1");
        return 1;
    }

    if(!assignVal("list1", 1, 20))
    {
        fprintf(stderr, "Failed to assign value to list1");
        return 1;
    }

    if(!assignVal("list2", 0, 30))
    {
        fprintf(stderr, "Failed to assign value to list2");
        return 1;
    }

    if(!assignVal("list2", 1, 40))
    {
        fprintf(stderr, "Failed to assign value to list2");
        return 1;
    }

    std::cout << "Values assigned" << std::endl;

    std::cout << "list1[0] = " << getVal("list1", 0) << std::endl;
    std::cout << "list1[1] = " << getVal("list1", 1) << std::endl;

    std::cout << "list2[0] = " << getVal("list2", 0) << std::endl;
    std::cout << "list2[1] = " << getVal("list2", 1) << std::endl;

    std::cout << "Values retrieved" << std::endl;

    if(!freeElem("list1"))
    {
        fprintf(stderr, "Failed to free list1");
        return 1;
    }

    std::cout << "list1 freed" << std::endl;

    std::cout << "Head of freeList: " << freeList.head << std::endl;
    std::cout << "Tail of freeList: " << freeList.tail << std::endl;

    if(!freeElem("list2"))
    {
        fprintf(stderr, "Failed to free list2");
        return 1;
    }

    std::cout << "List2 freed" << std::endl;

    std::cout << "Head of freeList: " << freeList.head << std::endl;
    std::cout << "Tail of freeList: " << freeList.tail << std::endl;

    return 0;
}