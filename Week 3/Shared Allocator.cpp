template<class T>class SharedAllocator {
    private:
        Pool pool_;    // pool of elements of sizeof(T)
    public:
        typedef T value_type;
        typedef unsigned int  size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        pointer address(reference r) const { return &r; }
        const_pointer address(const_reference r) const {return &r;}
        SharedAllocator() throw():pool_(sizeof(T)) {}
        template<class U> SharedAllocator
            (const SharedAllocator<U>& t) throw():
                        pool_(sizeof(T)) {}
        ~SharedAllocator() throw() {};
        // space for n Ts
        pointer allocate(size_t n, const void* hint=0)
        {
            return(static_cast<pointer> (pool_.alloc(n)));
        }
        // deallocate n Ts, don't destroy
        void deallocate(pointer p,size_type n)
        {
            pool_.free((void*)p,n);
            return;
        }
        // initialize *p by val
        void construct(pointer p, const T& val) { new(p) T(val); }
        // destroy *p but don't deallocate
        void destroy(pointer p) { p->~T(); }
        size_type max_size() const throw()
        {
            pool_.maxSize();
        }
        template<class U>    
        // in effect: typedef SharedAllocator<U> other
        struct rebind { typedef SharedAllocator<U> other; };
};

template<class T>bool operator==(const SharedAllocator<T>& a,
    const SharedAllocator<T>& b) throw()
{
        return(a.pool_ == b.pool_);
}
template<class T>bool operator!=(const SharedAllocator<T>& a,
    const SharedAllocator<T>& b) throw()
{
        return(!(a.pool_ == b.pool_));
}
