template <typename T>
Graph<T>::Graph(key_t key, size_t sz)
	: size(sz)
{
	this->shmId = shmget(key, sz * sz * sizeof(T), IPC_CREAT | 0666);
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

	this->data = reinterpret_cast<T *>(ptr);
}

template <typename T>
Graph<T>::~Graph()
{
	shmdt(this->data);
	shmctl(this->shmId, IPC_RMID, nullptr);
}

template <typename T>
void Graph<T>::init(const T &val)
{
	std::fill(this->data, this->data + this->size * this->size, val);
}

template <typename T>
size_t Graph<T>::getSize() const
{
	return this->size;
}

template <typename T>
T &Graph<T>::operator()(size_t i, size_t j)
{
	if (i >= this->size or
		j >= this->size)
	{
		throw std::out_of_range("Index out of range");
	}
	return this->data[i * this->size + j];
}

template <typename T>
const T &Graph<T>::operator()(size_t i, size_t j) const
{
	if (i >= this->size or
		j >= this->size)
	{
		throw std::out_of_range("Index out of range");
	}
	return this->data[i * this->size + j];
}
