// #include "My_Allocator.hpp"

template <typename T>
void My_Allocator<T>::init()
{
	if (shmid == -1)
	{
		shmid = shmget(IPC_PRIVATE, MAX_ALLOC + sizeof(int), IPC_CREAT | 0666);
		if (shmid == -1)
		{
			throw std::bad_alloc();
		}
		ptr = shmat(shmid, nullptr, 0);
		if (ptr == reinterpret_cast<void *>(-1))
		{
			throw std::bad_alloc();
		}
		cnt = reinterpret_cast<int *>(ptr);
		*cnt = 0;
		offset = sizeof(int);
	}
	++(*cnt);
}

template <typename T>
My_Allocator<T>::My_Allocator() : std::allocator<T>()
{
	init();
}

template <typename T>
My_Allocator<T>::~My_Allocator()
{
	if (--(*cnt) == 0)
	{
		shmdt(cnt);
		shmctl(shmid, IPC_RMID, nullptr);
	}
}

// Might be problem... Check it out
template <typename T>
typename My_Allocator<T>::pointer My_Allocator<T>::allocate(size_type n, const void *hint)
{
	if (offset + n * sizeof(T) > MAX_ALLOC)
	{
		throw std::bad_alloc();
	}

	pointer p = reinterpret_cast<pointer>(reinterpret_cast<char *>(ptr) + offset);
	offset += n * sizeof(T);
	return p;
}

template <typename T>
void My_Allocator<T>::deallocate(pointer p, size_type n)
{
	// Do nothing
}

template <typename T>
bool My_Allocator<T>::operator==(const My_Allocator &other) const
{
	return true;
}

template <typename T>
bool My_Allocator<T>::operator!=(const My_Allocator &other) const
{
	return false;
}

template <typename T>
My_Allocator<T>::My_Allocator(const My_Allocator &other) : std::allocator<T>(other)
{
	init();
}

template <typename T>
My_Allocator<T> &My_Allocator<T>::operator=(const My_Allocator &other)
{
	if (this != &other)
	{
		std::allocator<T>::operator=(other);
		init();
	}
	return *this;
}

template <typename T>
My_Allocator<T>::My_Allocator(My_Allocator &&other) : std::allocator<T>(std::move(other))
{
	init();
}

template <typename T>
My_Allocator<T> &My_Allocator<T>::operator=(My_Allocator &&other)
{
	if (this != &other)
	{
		std::allocator<T>::operator=(std::move(other));
		init();
	}
	return *this;
}
