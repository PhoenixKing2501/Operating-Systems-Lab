template <typename T>
Graph<T>::Graph(key_t key, size_t sz)
{
	this->shmId = shmget(key, VEC_LEN * VEC_LEN * sizeof(T) + sizeof(size_t), IPC_CREAT | 0666);
	if (shmId < 0)
	{
		perror("shmget");
		throw std::bad_alloc{};
	}

	void *ptr = shmat(this->shmId, nullptr, 0);

	if (ptr == reinterpret_cast<void *>(-1))
	{
		perror("shmat");
		shmctl(this->shmId, IPC_RMID, nullptr);
		throw std::bad_alloc{};
	}

	this->size = reinterpret_cast<size_t *>(ptr);
	*this->size = sz;
	this->data = reinterpret_cast<T *>(this->size + 1);
}

template <typename T>
void Graph<T>::remove_shm()
{
	shmdt(this->data);
	shmctl(this->shmId, IPC_RMID, nullptr);
}

template <typename T>
void Graph<T>::init(const T &val)
{
	std::fill(this->data, this->data + *this->size * *this->size, val);
}

template <typename T>
size_t Graph<T>::getSize() const
{
	return *this->size;
}

template <typename T>
void Graph<T>::setSize(size_t size)
{
	*this->size = size;
}

template <typename T>
size_t Graph<T>::getDegree(size_t i) const
{
	if (i >= *this->size)
	{
		throw std::out_of_range("Index out of range");
	}

	size_t degree = 0;
	for (size_t j = 0; j < *this->size; ++j)
	{
		if ((*this)(i, j))
		{
			++degree;
		}
	}
	return degree;
}

template <typename T>
T &Graph<T>::operator()(size_t i, size_t j)
{
	if (i >= *this->size or
		j >= *this->size)
	{
		throw std::out_of_range("Index out of range");
	}
	return this->data[i * *this->size + j];
}

template <typename T>
const T &Graph<T>::operator()(size_t i, size_t j) const
{
	if (i >= *this->size or
		j >= *this->size)
	{
		throw std::out_of_range("Index out of range");
	}
	return this->data[i * *this->size + j];
}
