#pragma once

#ifndef _GOODMALLOC_HPP_
#define _GOODMALLOC_HPP_

#include <cstdint>

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

#endif // _GOODMALLOC_HPP_
