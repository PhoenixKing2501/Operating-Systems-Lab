#pragma once

#ifndef _GOODMALLOC_HPP_
#define _GOODMALLOC_HPP_

void fn_beg();
void fn_end();
bool createMem(size_t size);
bool createList(const char * name, int num_elements);
bool assignVal(const char * list_name, int idx, int val);
int getVal(const char * list_name, int idx);
int freeElem(const char * list_name);
int freeElem();

#endif // _GOODMALLOC_HPP_
