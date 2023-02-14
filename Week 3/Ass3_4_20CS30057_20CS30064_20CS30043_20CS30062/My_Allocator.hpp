#pragma once

#ifndef _ALLOCATOR_HPP_
#define _ALLOCATOR_HPP_

#include <fcntl.h>
#include <memory>
#include <sys/shm.h>
#include <unistd.h>

constexpr size_t VEC_LEN = 5000;
constexpr size_t GRAPH_SIZE = VEC_LEN * VEC_LEN;

template <typename T>
class My_Allocator : public std::allocator<T>
{
private:
	static int shmid;
	static int *cnt;
	static void *ptr;
	static size_t offset;
	static const size_t MAX_ALLOC;

	static void init();

public:
	// Typedefs
	using value_type = T;
	using pointer = T *;
	using const_pointer = const T *;
	using reference = T &;
	using const_reference = const T &;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	// Member functions
	pointer allocate(size_type n, const void *hint = nullptr);
	void deallocate(pointer p, size_type n);

	// Ctors, dtors, copy, move
	My_Allocator();
	~My_Allocator();
	My_Allocator(const My_Allocator &other);
	My_Allocator &operator=(const My_Allocator &other);
	My_Allocator(My_Allocator &&other);
	My_Allocator &operator=(My_Allocator &&other);

	// Operators
	bool operator==(const My_Allocator &other) const;
	bool operator!=(const My_Allocator &other) const;
};

template <typename T>
int My_Allocator<T>::shmid = -1;

template <typename T>
int *My_Allocator<T>::cnt = nullptr;

template <typename T>
void *My_Allocator<T>::ptr = nullptr;

template <typename T>
size_t My_Allocator<T>::offset = 0;

template <typename T>
const size_t My_Allocator<T>::MAX_ALLOC = 2 * GRAPH_SIZE * sizeof(T);

#include "My_Allocator.inl"

#endif // _ALLOCATOR_HPP_
