class Pool {
    private:
        class shmPool {
            private:
                struct Container {
                    containerMap* cont;
                };
                class Chunk {
                    public:
                        Chunk()
                        Chunk(Chunk&);
                        ~Chunk() {}
                        void* alloc(size_t size);
                        void free (void* p,size_t size);
                    private:
                        int shmId_;
                        int semId_;
                        int lock_()
                };
                int key_;
                char* path_;
                Chunk** chunks_;
                size_t segs_;
                size_t segSize_;
                Container* contPtr_;
                int contSemId_;
            public:
                shmPool();
                ~shmPool();
                size_t maxSize();
                void* alloc(size_t size);
                void free(void* p, size_t size);
                int shmPool::lockContainer()
                int unLockContainer()
                containerMap* getContainer()
                void shmPool::setContainer(containerMap* container)
        };

    private:
        static shmPool shm_;
        size_t elemSize_;
    public:
        Pool(size_t elemSize);
        ~Pool() {}
        size_t maxSize();
        void* alloc(size_t size);
        void free(void* p, size_t size);
        int lockContainer();
        int unLockContainer();
        containerMap* getContainer();
        void setContainer(containerMap* container);
};
inline bool operator==(const Pool& a,const Pool& b)
{
    return(a.compare(b));
}
