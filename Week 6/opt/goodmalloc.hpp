#pragma once

#ifndef _GOODMALLOC_HPP_
#define _GOODMALLOC_HPP_

#include <cstdint>

using ptr_t = int32_t;

void fn_beg();
void fn_end();
bool createMem(size_t size);
bool createList(const char *name, int32_t num_elements);
bool assignVal(const char *list_name, int32_t idx, int32_t val);
int32_t getVal(const char *list_name, int32_t idx);
int32_t freeElem(const char *list_name);
int32_t freeElem();
void printList(const char *list_name);
void deleteMem();
void printTable();

ptr_t listBegin(const char *list_name);
ptr_t listPtr(const char *list_name, int32_t idx);
ptr_t listEnd(const char *list_name);
ptr_t listNext(ptr_t ptr);
ptr_t listPrev(ptr_t ptr);
int32_t listGetElem(ptr_t ptr);
void listSetElem(ptr_t ptr, int32_t val);

#endif // _GOODMALLOC_HPP_
