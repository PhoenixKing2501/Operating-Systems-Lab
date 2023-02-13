// file: allocator.h
// author: Marc Bumble
// May 12, 2000
// Memory allocator for shared memory access
// Copyright (C) 2000 by Marc D. Bumble

//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <math.h>
#include <bitset>


#include <global_defs.h>
#include <bit_vector.h>
#include <arena.h>

namespace allocator_ns {

  /////////////////////////////////////////////////////////////////////////////
  //////                           Class Chunk
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        Unit of pooled memory composed of multiple elements.
  //////        
  //////        
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  class Chunk {
    // Subtract one here to round up on division remainder in bit_vec allocation
    // The -1 at the end of size and the +1 at the end of bit_vec_sz are just to
    // get the structures to lay on even byte boundries.
    const int esize;		// size of each element in bytes
    const int page_size;	// in bytes
    const int mem_size;         // total element storage size in bytes
    const int nelem;
    const int bit_vec_sz;

    int semid;			// which array of semaphores
    int semnum;			// which specific element of the semaphore array

    int first_block_num;	// global value of first block in chunk,
				// start at 0
    struct Link {Link* next;};
    bit_vec_space::bit_vector bit_vec;
    unsigned char* mem;
    Link* head;	// points to the first block of free memory
    Chunk* prev;  // points to prev chunk
    Chunk* next;  // points to next chunk
    // Link* prev;  // points to prev chunk
    // Borrowed one from the mem array size above to roundup bit_vec size to ceiling.
    // given the pointer, find the block number
    void splice_memory_ptrs(int start_element,int num_of_elements);
  public:
    Chunk(const int& elem_size,const int& memory_size);
    ~Chunk() {};
    // copy constructor
    Chunk(const Chunk& t);
    // assignment operator
    Chunk& operator=(const Chunk& t);
    unsigned char* get_mem() {return mem;}
    Chunk* get_next() {return next;}
    Chunk* get_prev() {return prev;}
    void set_next(Chunk* val) {next=val;}
    void set_prev(Chunk* val) {prev=val;}
    const int get_esize() {return esize;}
    const int get_nelem() {return nelem;}
    const int get_mem_size() {return mem_size;}
    const int get_bit_vec_sz() {return bit_vec_sz;}
    const int get_first_block_num() {return first_block_num;}
    int get_semid() {return semid;}
    int get_semnum() {return semnum;}
    void set_semid(int val) {semid=val;}
    void set_semnum(int val) {semnum=val;}
    void set_first_block_num(int val) {first_block_num=val;}
    int pointer_to_block(unsigned char* p);
    // given the block number, find the pointer
    unsigned char* block_to_pointer(int start_element);
    
    int find(int num_of_elements);
    int find_prev(int start_element);
    void mark(int start_element,int num_of_elements);
    void clear(int start_element,int num_of_elements);
    unsigned char* allocate(int start_element,int num_of_elements);
    void free(void *p,int num_of_elements);
    
  };  // class Chunk



  /////////////////////////////////////////////////////////////////////////////
  //////                           Class Pool
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        Class Pool maintains segments of memory which are stored as a
  //////        as a list of chunks.
  //////        
  //////        
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  class Pool {
    typedef size_t size_type;

    const int esize;
    const int page_size;

    struct Link {Link* next;};

    Link *head;
    Chunk* chunks;
    static arena::Arena<mem_space::shared> mem_arena;
    arena::locks lock;		// array of chunk locks. each chunk gets a lock
    int semid;

    int expansion_factor;
    
    int find(int num_of_elements);
    Chunk* find_chunk(int start_block);
    Chunk* find_chunk(void *p,int *start_block);
    void mark(int start_element,int num_of_elements);
    void clear(int start_element,int num_of_elements);
    void *allocate(int start_element,int num_of_elements);
    void recycle(void* p,int num_of_elements);

    void grow();
    Chunk* get_last_chunk();	// find and return a pointer to the list's last chunk
  public:
    Pool(unsigned int sz);
    ~Pool();
    // copy constructor
    Pool(const Pool& t);
    // assignment operator
    Pool& operator=(const Pool& t);
    void* alloc(size_type num_of_blocks);
    void free(void* p,size_type num_of_blocks);
  };  // class Pool

  inline void *Pool::alloc(size_type n) {
  // void *Pool::alloc(size_type n) {
    if (chunks==0) grow();
    // see if there is enough contigous memory avail to satisfy the request for n
    // find first block of size n
    // See if the space is avail right here at the list head

    // determines which chunk has num_of_elements free blocks which can be
    // allocated to the requestor.
    Chunk* this_chunk=chunks;
    int start_block=-1;
    while ((this_chunk)&&(start_block==-1)) {
      // lock the chunk
      lock.lock(this_chunk->get_semnum());
      start_block=this_chunk->find(n);
      if (start_block==-1) {
	if (this_chunk->get_next()==0) {
	  grow();
	} 
	// release the current block
	lock.unlock(this_chunk->get_semnum());
	this_chunk=this_chunk->get_next();
	// lock the new block
	lock.lock(this_chunk->get_semnum());
      }
      
    }
    this_chunk->mark(start_block,n);
    void* p=reinterpret_cast<void*>(this_chunk->allocate(start_block,n));
    lock.unlock(this_chunk->get_semnum());
    return p;
  }
  
  inline void Pool::free(void *b,size_type n) {
  // void Pool::free(void *b,size_type n) {
    // Chunk* this_chunk = find_chunk(b);
    int start_block;
    Chunk* this_chunk=find_chunk(b,&start_block);
    // release the current block if it exists
    if (this_chunk!=0) {
      lock.lock(this_chunk->get_semnum());
      this_chunk->free(b,n);
      // lock the block
      lock.unlock(this_chunk->get_semnum());
    }
    //    Link *p = static_cast<Link *>(b);
    //    p->next = head;		// put b back as first element
    //    head = p;
  }


  /////////////////////////////////////////////////////////////////////////////
  //////                           Class Pool_alloc
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        This class provide the generic allocator interface to the 
  //////        outside world.
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  template<class T>class Pool_alloc {
  private:
    static Pool mem;		// pool of elements of sizeof(T)
  public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    
    typedef T* pointer;
    typedef const T* const_pointer;
    
    typedef T& reference;
    typedef const T& const_reference;
    
    pointer address(reference r) const { return &r; }
    const_pointer address(const_reference r) const {return &r;}
    
    Pool_alloc() throw();
    template<class U> Pool_alloc(const Pool_alloc<U>& t) throw() { };
    ~Pool_alloc() throw() {};
    
    pointer allocate(size_type n, const void* hint=0);	// space for n Ts
    void deallocate(pointer p,size_type n); // deallocate n Ts, don't destroy
    
    void construct(pointer p, const T& val) { new(p) T(val); } // initialize *p by val
    void destroy(pointer p) { p->~T(); } // destroy *p but don't deallocate
    
    size_type max_size() const throw();
    
    template<class U>
    // in effect: typedef Pool_alloc<U> other
    struct rebind { typedef Pool_alloc<U> other; }; 
  };
  
  template<class T>bool operator==(const Pool_alloc<T>&, const Pool_alloc<T>&) throw();
  template<class T>bool operator!=(const Pool_alloc<T>&, const Pool_alloc<T>&) throw();
  
  template<class T>Pool Pool_alloc<T>::mem(sizeof(T));
  
  template<class T>Pool_alloc<T>::Pool_alloc() throw() { }

  template<class T>
    T* Pool_alloc<T>::allocate(size_type n, const void* = 0) {
    // if (n==1) return static_cast<pointer>(mem.alloc());
#ifdef SHARED_MEMORY_MESG
    std::cerr << "allocate: " << n << " objects ********\n";
#endif 
    return static_cast<pointer>(mem.alloc(n));
    // ...
  }
  
  template<class T>
    void Pool_alloc<T>::deallocate(pointer p, size_type n) {
    
    if (n!=0) {
      mem.free(p,n);
      return;
    }
    
  }
  
}; // namespace allocator_ns

#endif // ALLOCATOR_H
 
