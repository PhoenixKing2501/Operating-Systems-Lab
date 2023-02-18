template <typename T>
Graph<T>::Graph(key_t key, size_t size)
	: m_size(size)
{
	shmId = shmget(key, size * size * sizeof(T), IPC_CREAT | 0666);
	if (shmId < 0)
	{
		perror("shmget");
		throw std::bad_alloc{};
	}

	void *ptr = shmat(shmId, nullptr, 0);

	if (ptr == reinterpret_cast<void *>(-1))
	{
		perror("shmat");
		throw std::bad_alloc{};
	}

	m_data = reinterpret_cast<T *>(ptr);
}

template <typename T>
Graph<T>::~Graph()
{
	shmdt(m_data);
	shmctl(shmId, IPC_RMID, nullptr);
}

template <typename T>
void Graph<T>::init(const T &val)
{
	std::fill(m_data, m_data + m_size * m_size, val);
}

template <typename T>
size_t Graph<T>::getSize() const
{
	return m_size;
}

template <typename T>
T &Graph<T>::operator()(size_t i, size_t j)
{
	if (i >= m_size or j >= m_size)
	{
		throw std::out_of_range("Index out of range");
	}
	return m_data[i * m_size + j];
}

template <typename T>
const T &Graph<T>::operator()(size_t i, size_t j) const
{
	if (i >= m_size or j >= m_size)
	{
		throw std::out_of_range("Index out of range");
	}
	return m_data[i * m_size + j];
}
