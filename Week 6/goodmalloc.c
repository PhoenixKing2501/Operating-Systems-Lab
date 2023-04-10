#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct _element
{
    int data;
    int prev;
    int next;
} element;

typedef struct _list
{
    int head;
    int tail;
    int size;
} list;

typedef struct _tablerow
{
    char name[20];
    list li;
} tablerow;

tablerow * table;   // maybe we should make a stack
element * mem;
list freeList;



void createMem(size_t size)
{
    mem = malloc(size);
    if (mem == NULL)
    {
        perror("malloc");
        exit(1);
    }

    int eleNum = size / sizeof(element);
    for (int i = 0; i < eleNum; i++)
    {
        mem[i].next = i + 1;
        mem[i].prev = i - 1;
    }
    mem[eleNum-1].next = -1;
    
    freeList.head = 0;
    freeList.tail = eleNum - 1;
    freeList.size = eleNum;
}

bool createList(const char * name, int num_elements)
{
    if(num_elements > freeList.size)
    {
        printf("Not enough memory to create list %s", name);
        return false;
    }

    int head = freeList.head;
    int tail = head;
    for (int i = 1; i < num_elements; i++)
    {
        tail = mem[tail].next;
    }

    tablerow r = {.name = strdup(name), .li = {.head = head, .tail = tail, .size = num_elements}};
    // Add to table

    freeList.head = mem[tail].next;
    freeList.size -= num_elements;

    return true;
}

bool assignVal(char * list_name, int idx, int val)
{
    // Find list
    list l;
    if (idx >= l.size)
    {
        fprintf(stderr, "Index out of bounds");
        return false;
    }
    mem[l.head + idx].data = val;
    return true;
}

bool freeElem(char * list_name)
{
    // Find list and pop from table
    list l;
    mem[freeList.tail].next = l.head;
    freeList.size += l.size;
    freeList.tail = l.tail;
    return true;
}