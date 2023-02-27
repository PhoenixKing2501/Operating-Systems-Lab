// file: allocator.cc
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

#include <allocator.h>

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


  // constructor
  Chunk::Chunk(const int& elem_size,const int& total_memory_size)
    // elem_size is in bytes
    // total_memory_size is pages of memory
    // total_memory_size is all the memory allocated.  Parts of this
    // memory need to be used for the Chunk and Bit Vector Classes.
    : esize(elem_size),
    page_size(PAGE_SIZE),
    mem_size(((esize*8)*(total_memory_size-sizeof(Chunk)-1))/(1+(esize*8))),
    nelem((mem_size)/elem_size),
    bit_vec_sz(total_memory_size-sizeof(Chunk)-mem_size),
    bit_vec(nelem,reinterpret_cast<unsigned char*>(this)+sizeof(Chunk)) {
    first_block_num=0;
    prev=0;
    next=0;
    mem=reinterpret_cast<unsigned char*>(this)+sizeof(Chunk)+bit_vec_sz;
    for (unsigned char *p = mem; p<(&mem[(nelem-1)*esize]); p+=esize) {
      reinterpret_cast<Link *>(p)->next = reinterpret_cast<Link *>(p+esize);
#ifdef ALLOCATOR_DEBUG
      // std::ios_base::fmtflags old_options = std::cout.flags();
      ios::ios_base::fmtflags old_options = cout.flags();
      // std::cout.flags(std::ios_base::hex);
      cout.setf(ios::ios_base::hex,ios::ios_base::basefield);
      std::cout << "p: 0x" << reinterpret_cast<Link *>(p) << "\tp->next: 0x";
      std::cout << reinterpret_cast<Link *>(p)->next << std::endl;
      cout.setf(old_options);
#endif
    }
    unsigned char* temp=&mem[(nelem-1)*esize];
    reinterpret_cast<Link *>(temp)->next = 0;
    head = reinterpret_cast<Link *>(mem);
    
  };

  // copy constructor
  Chunk::Chunk(const Chunk& t)
    : esize(t.esize),
    page_size(PAGE_SIZE),
    mem_size(t.mem_size),
    nelem(t.nelem),
    bit_vec_sz(t.bit_vec_sz),
    bit_vec(t.bit_vec) {

    semid=t.semid;
    semnum=t.semnum;
    
    first_block_num=t.first_block_num;
    prev=t.prev;
    next=t.next;
    mem=reinterpret_cast<unsigned char*>(this)+sizeof(Chunk)+bit_vec_sz;
    for (int i=0; i<mem_size; i++) {
      mem[i] = t.mem[i];
    }
    head = reinterpret_cast<Link *>(mem);

  };
  
  // assignment operator
  Chunk& Chunk::operator=(const Chunk& t) {
    if (this != &t) {		// avoid self assignment: t=t
      semid=t.semid;
      semnum=t.semnum;
      
      first_block_num=t.first_block_num;
      prev=t.prev;
      next=t.next;
      mem=reinterpret_cast<unsigned char*>(this)+sizeof(Chunk)+bit_vec_sz;
      for (int i=0; i<mem_size; i++) {
	mem[i] = t.mem[i];
      }
      bit_vec=t.bit_vec;
      head = reinterpret_cast<Link *>(mem);
    }
    return *this;
  };

  
  /////////////////////////////////////////////////////////////////////////////
  //////                 Chunk::find(int num_of_elements)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        num_of_elements - memory for num_of_elements
  //////        head - memory address of first free block
  //////        
  //////        find free memory space for num_of_elements.  Returns the
  //////        start_element number of the requested space.
  //////        
  /////////////////////////////////////////////////////////////////////////////


  int Chunk::find(int num_of_elements) {

    // Query this chunk to see if num_of_elements memory is avail.  If so
    // return its start_block number, otherwise return -1;

    // first find starting point.  This function may return a -1 which will
    // be sent to the higher level indicating that the next chunk should be
    // searched (there is no more suitable space in the current chunk).
    int start_block;
    start_block=bit_vec.find(num_of_elements);
    // Convert the start_block addr which is the block addr local to this
    // block, into a global block number by adding in the global address
    // of the first_block_num of this chunk
    if (start_block!=-1)
      start_block+=first_block_num;
    return start_block;
  };  // Chunk::find()


  /////////////////////////////////////////////////////////////////////////////
  //////                  Chunk::pointer_to_block(void *p)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        p - address of a block inside this chunk
  //////        
  //////        find and return the local (within the chunk) start block 
  //////        number for the given address in p.  Used to free blocks which
  //////        are in use.  So the returned block number is an offset from 0.
  //////        
  /////////////////////////////////////////////////////////////////////////////


  int Chunk::pointer_to_block(unsigned char* p) {
    // Calculate and return the global block(element) number of the block
    // residing at p.
    unsigned char* lower_bound=mem;
    unsigned char* upper_bound=mem+nelem*esize;
    int start_block=-1;
    if ((lower_bound<=p)&&(p<upper_bound)) {
      start_block=(static_cast<int>(p-lower_bound))/esize;
    } else {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << " pointer_to_block(): ";
      std::cerr << "requested pointer outside of chunk.\n";
    }
    return start_block;
  };




  /////////////////////////////////////////////////////////////////////////////
  //////             Chunk::block_to_pointer(int start_block)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        p - address of a block inside this chunk
  //////        
  //////        find and return the local start block number for the given
  //////        address in p.  Used to free blocks which are in use.
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////


  unsigned char* Chunk::block_to_pointer(int start_block) {
    // Calculate and return the global block(element) number of the block
    // residing at p.
    unsigned char* p;
    if ((0<=start_block)&&(start_block<(nelem))) {
      // start_block-=first_block_num;
      p = mem+(start_block*esize);
    } else {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << " block_to_pointer(): ";
      std::cerr << "requested block outside of chunk.\n";
    }
    return p;
  };

  /////////////////////////////////////////////////////////////////////////////
  //////    Chunk::splice_memory_ptrs(int start_element,int num_of_elements)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block as a local
  //////                        coordinate, within this block start at 0.
  //////        num_of_elements - memory for num_of_elements
  //////        
  //////        A freed block is being reintegrated back into the chunk.  This
  //////        routine adjusts pointers so that the previous free block now
  //////        points to start_element, and the last block of this group now
  //////        points to whatever the previous free block pointed to.
  //////        
  /////////////////////////////////////////////////////////////////////////////


  void Chunk::splice_memory_ptrs(int start_element,int num_of_elements) {
    // adjust the pointers to reintegrate this segment of memory.
    // int local_start_element=start_element-first_block_num;
    int prev_block=bit_vec.find_prev_free_block(start_element);
    unsigned char* start_element_ptr=block_to_pointer(start_element);
    unsigned char* last=start_element_ptr+((num_of_elements-1)*esize);
    if (prev_block!=-1) {
      Link* prev_block_ptr=reinterpret_cast<Link*>(block_to_pointer(prev_block));
      reinterpret_cast<Link*>(last)->next = prev_block_ptr->next;
      prev_block_ptr->next = reinterpret_cast<Link*>(start_element_ptr);
    } else {
      // There is no previous block, use the head pointer
      reinterpret_cast<Link*>(last)->next = head;
      head = reinterpret_cast<Link*>(start_element_ptr);
    }
    // now just set the newly freed block elements to point to each other
    // one in turn.

    // Now walk through freed block and point each element to the next except
    // the last block
    for (unsigned char* i=start_element_ptr; i<(start_element_ptr+((num_of_elements-1)*esize));
	 i+=esize)
      reinterpret_cast<Link*>(i)->next = reinterpret_cast<Link*>(i+esize);
    return;
  };


  /////////////////////////////////////////////////////////////////////////////
  //////                          Chunk::mark() 
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory for num_of_elements
  //////        head - memory address of first free block
  //////        
  //////        mark as used in the bit vector
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Chunk::mark(int global_start_element,int num_of_elements) {
    if ((first_block_num<=global_start_element)&&
	(global_start_element<(first_block_num+nelem))) {
      int local_start_element=global_start_element-first_block_num;
      bit_vec.mark(local_start_element,num_of_elements);
    } else {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << " mark(): ";
      std::cerr << "requested start_element outside of chunk.\n";
    }
    return;
  };  // Chunk::mark() 


  /////////////////////////////////////////////////////////////////////////////
  //////                          Chunk::clear() 
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory for num_of_elements
  //////        
  //////        clear set blocks as unused in the bit vector
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Chunk::clear(int global_start_element,int num_of_elements) {
    if ((first_block_num<=global_start_element)&&
	(global_start_element<(first_block_num+nelem))) {
      int local_start_element=global_start_element-first_block_num;
      bit_vec.clear(local_start_element,num_of_elements);
    } else {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << " mark(): ";
      std::cerr << "requested start_element outside of chunk.\n";
    }
    return;
  };  // Chunk::clear() 


  /////////////////////////////////////////////////////////////////////////////
  //////                           Chunk::allocate()
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory for num_of_elements
  //////        head - memory address of first free block
  //////        
  //////        allocate adjusts pointers in the memory chunk and returns
  //////        a pointer to the start of the allocated memory
  //////        
  /////////////////////////////////////////////////////////////////////////////

  unsigned char* Chunk::allocate(int global_start_element,int num_of_elements) {
    // assumes that:
    //    1. global_start_element has already been located by
    //       find() as a free block,
    //    2. That the memory at global_start_element has been marked as
    //       allocated by mark() and is now ready to be returned
    int local_start_element=global_start_element-first_block_num;
    unsigned char* p = block_to_pointer(local_start_element);
    Link* last=reinterpret_cast<Link*>(p+((num_of_elements-1)*esize));
    // update the individual block pointers for the allocation
    if (head==reinterpret_cast<Link*>(p)) {
      // The start_element is the first free block in the chunk.
      // set head pointer to free block after this block
      head=reinterpret_cast<Link*>(last)->next;
    } else {
      int prev_free_block=bit_vec.find_prev_free_block(local_start_element);
      if (prev_free_block!=-1) {
	// There is a previous block, align the pointers
	unsigned char* prev_ptr=block_to_pointer(prev_free_block);
	reinterpret_cast<Link*>(prev_ptr)->next = reinterpret_cast<Link*>(last)->next;
      } 
    }
    return p;
  };  // Chunk::allocate()


  /////////////////////////////////////////////////////////////////////////////
  //////                           Chunk::free()
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        num_of_elements - memory for num_of_elements
  //////        head - memory address of first free block
  //////        
  //////        free adjusts pointers in the memory chunk 
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Chunk::free(void *p,int num_of_elements) {
    if ((mem<=reinterpret_cast<unsigned char*>(p))&&
	(reinterpret_cast<unsigned char*>(p)<(mem+(esize*nelem)))) {
      int start_block=pointer_to_block(reinterpret_cast<unsigned char*>(p));
      bit_vec.clear(start_block,num_of_elements);
      splice_memory_ptrs(start_block,num_of_elements);
    } else {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << "free(): ";
      std::cerr << "pointer p not in this chunk.\n";
      exit(1);
    }
  };  // Chunk::free()


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

  arena::Arena<mem_space::shared> Pool::mem_arena;
  
  // constructor
  Pool::Pool(unsigned int sz)
    : esize (sz),
    page_size(PAGE_SIZE) {
    chunks=0;
    head=0;
    expansion_factor=1;
  };  // Pool::Pool()

  // destructor
  Pool::~Pool() {			// free all chunks
    Chunk *n = chunks;
    while(n) {
#ifdef SHARED_MEMORY_MESG
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << "~Pool(): ";
      std::cerr << "destroying chunk.\n";
#endif 
      Chunk *p =n;
      n = reinterpret_cast<Chunk *>(n->get_next());
      // call the distructor
      p->~Chunk();
//        union semun semctl_arg;
//        for(int i=0; i<lock.get_num_in_array(); i++) {
//  	semctl(semid,i,IPC_RMID,semctl_arg);
//        }
    }
  }

  // copy constructor
  Pool::Pool(const Pool& t)
    : esize (t.esize),
    page_size(PAGE_SIZE) {
    expansion_factor=1;
    Chunk *target_chunk = t.chunks;
    while (target_chunk) {
      void* buff = mem_arena.alloc(10*expansion_factor);	// 10 pages
      // Chunk *n = new (buff) Chunk(*target_chunk);
      Chunk *n = new (buff) Chunk(*target_chunk);
      if (expansion_factor==1) {
	chunks=n;
	chunks->set_next(0);
	chunks->set_prev(0);
	chunks->set_semnum(0);
      } else {
	Chunk* last_chunk=get_last_chunk();
	last_chunk->set_next(n);
	n->set_prev(last_chunk);
	// set the new block's first global start block number
	n->set_first_block_num(last_chunk->get_first_block_num()+
			       last_chunk->get_nelem());
	int this_semnum=n->get_prev()->get_semnum()+1;
	n->set_semnum(this_semnum);
	if (lock.get_max_semnum()<this_semnum)
	  lock.set_max_semnum(this_semnum);
      }
      n->set_semid(semid);
      target_chunk = target_chunk->get_next();
      expansion_factor*=2;
    }
    chunks=t.chunks;
    head=t.head;
    // supply the semid
  };
  
  // assignment operator
  Pool& Pool::operator=(const Pool& t) {
    if (this != &t) {		// avoid self assignment: t=t

      // first destroy the existing chunk list
      Chunk *n = chunks;
      while(n) {
	std::cerr << __FILE__ << ':' << __LINE__ << ':' << "Pool::operator=(): ";
	std::cerr << "destroying chunk.\n";
	Chunk *p =n;
	n = reinterpret_cast<Chunk *>(n->get_next());
	// call the distructor
	p->~Chunk();
//  	union semun semctl_arg;
//  	for(int i=0; i<lock.get_num_in_array(); i++) {
//  	  semctl(semid,i,IPC_RMID,semctl_arg);
//  	}
      }
      // now copy and create the Chunk list
      expansion_factor=1;
      Chunk *target_chunk = t.chunks;
      while (target_chunk) {
	void* buff = mem_arena.alloc(10*expansion_factor);	// 10 pages
	Chunk *n = new(buff)Chunk(*target_chunk);
	if (expansion_factor==1) {
	  chunks=n;
	  chunks->set_next(0);
	  chunks->set_prev(0);
	  chunks->set_semnum(0);
	} else {
	  Chunk* last_chunk=get_last_chunk();
	  last_chunk->set_next(n);
	  n->set_prev(last_chunk);
	  // set the new block's first global start block number
	  n->set_first_block_num(last_chunk->get_first_block_num()+
				 last_chunk->get_nelem());
	  n->set_semnum(n->get_prev()->get_semnum()+1);
	}
	target_chunk = target_chunk->get_next();
	expansion_factor*=2;
      }
      chunks=t.chunks;
      head=t.head;
      expansion_factor=t.expansion_factor;
      // supply the semid
      n->set_semid(semid);
    }
    return *this;
  };
  
  /////////////////////////////////////////////////////////////////////////////
  //////                      Pool::find(int num_of_elements)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory to hold num_of_elements
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  int Pool::find(int num_of_elements) {
    // Search through the list of chunks to find the requested num_of_elements.

    Chunk* current_chunk = chunks;
    int start_element=-1;
    int block_offset=0;
    while ((current_chunk!=0)&&(start_element==-1)) {
      start_element=current_chunk->find(num_of_elements);
      if (start_element!=-1)
	block_offset+=current_chunk->get_nelem();
      else
	start_element+=block_offset;
    }
    return start_element;
  };


  /////////////////////////////////////////////////////////////////////////////
  //////               Pool::find_chunk(int start_block)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  Chunk* Pool::find_chunk(int start_block) {
    Chunk* current_chunk = chunks;
    int lower_bound=0;
    int upper_bound=current_chunk->get_nelem();
    while ((!(lower_bound<=start_block))&&(!(start_block<upper_bound))) {
      lower_bound+=current_chunk->get_nelem();
      current_chunk=current_chunk->get_next();
      if (current_chunk!=0)
	upper_bound+=current_chunk->get_nelem();
      else
	break;
    }
    return current_chunk;
  };

  /////////////////////////////////////////////////////////////////////////////
  //////               Pool::find_chunk(void *p,int *start_block)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        p - only real input parameter, address of block for
  //////            which to find the Chunk pointer and start_block
  //////            number.
  //////        start_element - find the first element of block at p
  //////        
  //////        Returns a pointer to the Chunk which contains the address p.
  //////        Also returns the start_block number of the element at p.
  //////        
  /////////////////////////////////////////////////////////////////////////////

  Chunk* Pool::find_chunk(void *p,int *start_block) {
    // first find the correct block
    // then set the value of the start_block.
    // finally return the chunk pointer.
    Chunk* current_chunk = chunks;
    if (chunks!=0) {
      // When the program is closing out, the destructors may eliminate
      // shared memory, so make sure that it exists by checking the value
      // of the chunks list != 0
      unsigned char* lower_bound=current_chunk->get_mem();
      unsigned char* upper_bound=lower_bound+
	(current_chunk->get_esize()*current_chunk->get_nelem());
      // run through the blocks until the correct one is found.
      while (!((lower_bound<=p)&&(p<upper_bound))) {
	if (current_chunk->get_next()!=0) {
	  current_chunk=current_chunk->get_next(); 
	  lower_bound=current_chunk->get_mem();
	  upper_bound=lower_bound+
	    (current_chunk->get_esize()*current_chunk->get_nelem());
	}
	else {
	  current_chunk=0;
	  *start_block=-1;
	  std::cerr << __FILE__ << ':' << __LINE__ << ':' << " find_chunk(): ";
	  std::cerr << "requested chunk lookup not found.\n";
	  break;
	}
      }
      *start_block=current_chunk->get_first_block_num()+
	current_chunk->pointer_to_block(reinterpret_cast<unsigned char*>(p));
    }  // if (chunks!=0)
    return current_chunk;
  };

  
  /////////////////////////////////////////////////////////////////////////////
  //////               Pool::mark(int start_element,int num_of_elements)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory to hold num_of_elements
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Pool::mark(int start_element,int num_of_elements) {
    Chunk* current_chunk = find_chunk(start_element);
    current_chunk->mark(start_element,num_of_elements);
    return;
  };


  /////////////////////////////////////////////////////////////////////////////
  //////          Pool::clear(int start_element,int num_of_elements);
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory to hold num_of_elements
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Pool::clear(int start_element,int num_of_elements) {
    Chunk* current_chunk = find_chunk(start_element);
    current_chunk->clear(start_element,num_of_elements);
    return;

  };


  /////////////////////////////////////////////////////////////////////////////
  //////         Pool::allocate(int start_element,int num_of_elements)
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory to hold num_of_elements
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void *Pool::allocate(int start_element,int num_of_elements) {
    Chunk* current_chunk = find_chunk(start_element);
    return current_chunk->allocate(start_element,num_of_elements);
  };


  /////////////////////////////////////////////////////////////////////////////
  //////                           
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        start_element - first element of found block
  //////        num_of_elements - memory to hold num_of_elements
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  void Pool::recycle(void* p,int num_of_elements)  {
    int start_element;
    Chunk* current_chunk = find_chunk(p,&start_element);
    return current_chunk->free(p,num_of_elements);
  };


  /////////////////////////////////////////////////////////////////////////////
  //////                           Pool::get_last_chunk()
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        get_last_chunk() finds and returns a pointer to the last chunk
  //////        in the chunks list.
  //////        
  /////////////////////////////////////////////////////////////////////////////

  Chunk* Pool::get_last_chunk() {
    Chunk* chunk_ptr=chunks;
    Chunk* prev_chunk=chunk_ptr;
    while (chunk_ptr) {
      prev_chunk=chunk_ptr;
      chunk_ptr=chunk_ptr->get_next();
    }
    return prev_chunk;
  };

  /////////////////////////////////////////////////////////////////////////////
  //////                           Pool::grow()
  /////////////////////////////////////////////////////////////////////////////
  //////        
  //////        Increase the current available memory in the Pool
  //////        
  //////        
  /////////////////////////////////////////////////////////////////////////////

  // allocate new 'chunk' organize it as a linked list of
  // elements of size esize.
  void Pool::grow() {
    void* buff = mem_arena.alloc(10*expansion_factor);	// 10 pages
    Chunk *n = new(buff)Chunk(esize,10*expansion_factor*page_size);
    // stuff the new block on the end, not the beginning, gives a sense
    // of ordered block numbers and reuse.
    if (chunks==0) {
      chunks=n;
      chunks->set_next(0);
      chunks->set_prev(0);
      chunks->set_semnum(0);
      lock.set_max_semnum(1);
    } else {
      Chunk* last_chunk=get_last_chunk();
      last_chunk->set_next(n);
      n->set_prev(last_chunk);
      // set the new block's first global start block number
      n->set_first_block_num(last_chunk->get_first_block_num()+
			     last_chunk->get_nelem());
      int this_semnum=n->get_prev()->get_semnum()+1;
      n->set_semnum(this_semnum);
      if (lock.get_max_semnum()<=this_semnum)
	lock.set_max_semnum(this_semnum+1);
    }
    expansion_factor*=2;
    // supply the semid
    n->set_semid(semid);
  }

}  // namespace allocator_ns
