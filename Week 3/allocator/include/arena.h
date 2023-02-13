// file: arena.h
// author: Marc Bumble
// May 22, 2000
// Page memory source for shared memory 
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

#ifndef ARENA_H
#define ARENA_H

extern "C" {
#include <unistd.h>		// for fork(),getpid(),getppid()
#include <fcntl.h>
#include <sys/types.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/sem.h>            // for semaphores
#include <sys/ipc.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/shm.h>		// for shared memory shmget(),shmat(),ftok()
  // #include <errno.h>		// for shared memory shmget(),shmat(),ftok()
  // extern int errno;
}

#include <math.h>
#include <bitset>

#include <global_defs.h>
#include <alloc_sem.h>
#include <bit_vector.h>
#include <shared_memory.h>

namespace arena {

//  #define PAGE_SIZE (8*1024)
//  #define NUM_OF_PAGES (1000)
//  #define ALLOCATOR_PAGE_SIZE (100*8*1024)
//  #define MAX_NUM_SHARED_SEGMENTS (128)
//  #define MAX_SEGMENT_SIZE (32768) // kbytes

    ////////////////////////////////////////////////////////////////////////////////
    //////                             Segment
    ////////////////////////////////////////////////////////////////////////////////
    //////   
    //////    Segment is the overhead class associated with each of the shared 
    //////    memory segments.  It resides at the start of each memory segment
    //////    and is used to administrate the segments.
    //////    
    //////    The bit vector in segment tracks page allocations, so each bit
    //////    represents one page.
    //////    
    //////    
    ////////////////////////////////////////////////////////////////////////////////

    class Segment {
      // This data must be visible to all shared segments
      // it is stored at the beginning of the shared memory segment
      
      enum {num_of_pages=1000,
	    bit_vec_sz=num_of_pages/8 + 1,
	    page_size=(8*1024),
	    max_num_shared_segments=128,
	    max_segment_size=32768
      };
      size_t segment_size;	// in bytes
      unsigned char* head;	// points to first free page
      // int expansion_factor;	// segment size increase over previous
      unsigned char* mem;	// points to the first memory page.
      Segment *prev;		// previous shared segment
      Segment *next;		// next shared segment
      bit_vec_space::bit_vector bit_vec;	// page free/allocated list
      int pages_in_segment;	// memory pages in segment (segment sizes may vary)
      int first_page_num;	// first page number in segment (segment sizes may vary)
      //  	unsigned char *bit_vec;	// points to free/allocated bit vector for pages
      //  	int bit_vec_length;
    public:
      // constructor
      Segment(int num_pages,Segment* prev_segment);
      // destructor
      ~Segment() {};
      // copy constructor
      Segment(const Segment& t);
      // assignment operator
      Segment& operator=(const Segment& t);
      int get_bit_vec_sz() {return bit_vec.get_size();}
      int get_segment_sz() {return sizeof(Segment)+bit_vec_sz+
			      page_size*pages_in_segment;}
      int get_pages_in_segment() {return pages_in_segment;}
      int get_first_page_num() {return first_page_num;}
      Segment *get_next() {return next;}
      int find(int num_of_pages);
      void mark_pages(int start_page, int num_of_pages);
      void clear_pages(int start_page, int num_of_pages);
      void* allocate(int start_page, int num_of_pages);
    };  // class Segment



    ////////////////////////////////////////////////////////////////////////////////
    //////                             Arena
    ////////////////////////////////////////////////////////////////////////////////
    //////   
    //////    Creates large pages of raw memory which are distributed to allocators.
    //////    The allocators then manage, allocate, and recycle their assigned
    //////    memory blocks.  The Arena simply creates and distributes very large
    //////    chunks of memory to all of the allocators so that all allocators get
    //////    their memory from the same contigious locations.
    //////    
    //////    The Arena exists because the number of shared memory segments are
    //////    limited by the Operating System.  So instead of allowing each 
    //////    allocator to create, manage, etc. its own shared memory segment, one
    //////    very large memory segment requested by Arena is shared by all 
    //////    allocators.  The large memory segment can be appended by additional
    //////    
    //////    Unlike the higher level allocators, a shared memory segment must be
    //////    allocated immediately in the Arena constructor, as the Arena_Overhead
    //////    data will be instantiated with the shared memory itself.  This way
    //////    all process can access the shared overhead data and make changes to
    //////    within the Arena as necessary.
    //////    
    //////    Segments are layed out as follows:
    //////    1. First is the Segment structure
    //////    2. The Bit Vector which shows the allocated and free pages
    //////    3. At the next free Page Boundry, the First Page starts
    //////    
    //////    
    //////    
    //////    
    //////    
    //////    
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> class Arena {

      enum {num_of_pages=1000,
	    // bit_vec_sz=num_of_pages/8 + 1,
	    page_size=(8*1024),
	    max_num_shared_segments=128,
	    // max_segment_size=32768
      };
      ////////////////////////////////////////////////////////////////////////////
      //////        Arena Variables
      ////////////////////////////////////////////////////////////////////////////

      int shmid[max_num_shared_segments]; // shared memory id
      void *buffers[max_num_shared_segments];
      Segment *segment_list;	// pointer to first segment in chain
      int next_segment_number;

      ////////////////////////////////////////////////////////////////////////////
      //////        Arena Functions
      ////////////////////////////////////////////////////////////////////////////

      virtual void grow(int segment_num);
      virtual int find(int num_of_pages);
      virtual void mark_pages(int start_page, int num_of_pages);
      virtual void clear_pages(int start_page, int num_of_pages);
      virtual void* allocate(int start_page, int num_of_pages);
      virtual Segment* find_segment(int start_page);
      Memory_Type mem;
    public:
      virtual void* alloc(size_t num_of_pages);
      virtual void free(void* p,size_t n);
      Arena();
      virtual ~Arena();
      // copy constructor
      Arena(const Arena& t);
      // assignment operator
      Arena& operator=(const Arena& t);
    };  // class Arena
    
    template<class Memory_Type> void Arena<Memory_Type>::grow(int segment_num) {
      // Routines to allocate LARGE blocks of shared memory which can then be 
      // divvied up and shared between hungry allocators
      bool initialized;
      buffers[segment_num]=mem.allocate(&initialized);
      if (!initialized) {
	// initialize memory segment
	// place the segment at the beginning of the shared memory segment
	segment_list = new(buffers[segment_num])Segment(num_of_pages,0);
      }

    }  // grow()

    ////////////////////////////////////////////////////////////////////////////////
    //////                             Arena Constructor
    ////////////////////////////////////////////////////////////////////////////////
    //////     
    //////     The mem class is initialized with the number of pages in each segment
    //////     and the size of the overhead.  The overhead in a segment is composed
    //////     of the size of the Segment class and the size of the bit vector,
    //////     which is ((pages_in_segment)/8 +1).
    //////     
    ////////////////////////////////////////////////////////////////////////////////

    // Arena constructor
    template<class Memory_Type> Arena<Memory_Type>::Arena()
      : mem(num_of_pages,sizeof(Segment)+((num_of_pages)/8 +1)) {
      // see page 255 section 10.4.11
      segment_list = 0;
      next_segment_number=0;
      for (int i=0; i<max_num_shared_segments; i++) {
	shmid[i] = -1;
	buffers[i] = 0;
      }
      // grow(0);

    }  // Arena()
    
    ////////////////////////////////////////////////////////////////////////////////
    //////                             Arena Destructor
    ////////////////////////////////////////////////////////////////////////////////

    // Arena destructor
    template<class Memory_Type> Arena<Memory_Type>::~Arena() {
      // deallocate the shared memory segment
      Segment *this_segment = segment_list;
      while (this_segment) {
	Segment *victim = this_segment;
	this_segment = segment_list->get_next();
	victim->~Segment();
      }
//        for (int i=0; i<next_segment_number; i++)
//  	mem.free(shmid[i]);
      mem.free(0);
    }  // ~Arena()

    // copy constructor
    template<class Memory_Type> Arena<Memory_Type>::Arena(const Arena& t)
      : mem(num_of_pages,sizeof(Segment)+((num_of_pages)/8 +1)) {
      for(int i=0; i<max_num_shared_segments; i++) {
	shmid[i]=t.shmid[i];
	buffers[i]=t.buffers[i];
      }
      // normally, in a copy constructor, we would walk through all the
      // segments and make new unique copies of each segment which were
      // distinct from the originals; however, here the senario is that
      // we want all copies tied to a single unique instance of the segements.
      // so copying the pointers is okay.
      segment_list=t.segment_list;
      next_segment_number=t.next_segment_number;
    };
    
    // assignment operator
    template<class Memory_Type> Arena<Memory_Type>& Arena<Memory_Type>::operator=(const Arena& t) {
      if (this != &t) {		// avoid self assignment: t=t
	// here we only want one unique copy of the data, so all pointers should
	// point to the same data set.
	for(int i=0; i<max_num_shared_segments; i++) {
	  shmid[i]=t.shmid[i];
	  buffers[i]=t.buffers[i];
	}
	segment_list=t.segment_list;
	next_segment_number=t.next_segment_number;
      }
      return *this;
    };


    ////////////////////////////////////////////////////////////////////////////////
    //////                             find()
    ////////////////////////////////////////////////////////////////////////////////
    //////       
    //////       num_of_pages - number of contiguous free pages requested
    //////       
    //////       find() calls each segment in turn to search for the number
    //////       of free pages requested and returns the global page number
    //////       which starts the found block of requested pages.
    //////       
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> int Arena<Memory_Type>::find(int num_of_pages) {
      if (segment_list==0) grow(next_segment_number++);
      Segment *this_segment=segment_list;
      bool success=false;
      int start_page=-1;
      while ((this_segment!=0)&&(!success)) {
	// search the first segment for the requested memory space
	start_page=this_segment->find(num_of_pages);
	// if found, we are done
	if (start_page!=-1) 
	  success=true;
	else {
	  // otherwise, time to go to the next segment or grow more memory
	  if ((!success)&&(this_segment->get_next()!=0))
	    this_segment = this_segment->get_next();
	  else {
	    grow(next_segment_number++);
	    this_segment = this_segment->get_next();
	  }
	}
      }
      return start_page;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //////                             mark_pages()
    ////////////////////////////////////////////////////////////////////////////////
    //////       
    //////       start_page   - first of the requested pages
    //////       num_of_pages - number of contiguous free pages requested
    //////       
    //////       mark_pages() after the requested page has been found, mark_pages()
    //////       is called to take the pages out of the free space and list them
    //////       as unavailable.
    //////       
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> void Arena<Memory_Type>::mark_pages(int start_page,
								    int num_of_pages) {
      Segment* this_segment = find_segment(start_page);
      this_segment->mark_pages(start_page,num_of_pages);
      return;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //////                             clear_pages()
    ////////////////////////////////////////////////////////////////////////////////
    //////       
    //////       start_page   - first of the requested pages
    //////       num_of_pages - number of contiguous free pages requested
    //////       
    //////       mark_pages() after the requested page has been found, mark_pages()
    //////       is called to take the pages out of the free space and list them
    //////       as unavailable.
    //////       
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> void Arena<Memory_Type>::clear_pages(int start_page,
								     int num_of_pages) {
      Segment* this_segment = find_segment(start_page);
      this_segment->clear_pages(start_page,num_of_pages);
      return;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //////                             find_segment()
    ////////////////////////////////////////////////////////////////////////////////
    //////       
    //////       start_page   - first of the requested pages
    //////       
    //////       find_segment() given the start_block, find_segment() returns the
    //////       integer number corresponding to the correct page.  Returns 0 if
    //////       segment not found in list.
    //////       
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> Segment* Arena<Memory_Type>::find_segment(int start_page) {
      Segment* this_segment=segment_list;
      bool successful=false;
      do {
	if ((this_segment->get_first_page_num()<=start_page)&&
	    (start_page<(this_segment->get_first_page_num()+
			 this_segment->get_pages_in_segment()))) successful=true;
	else if (this_segment->get_next()!=0)
	  this_segment=this_segment->get_next();
	else {
	  grow(next_segment_number++);
	}
      } while (!successful);
      return this_segment;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //////                             allocate()
    ////////////////////////////////////////////////////////////////////////////////
    //////       
    //////       start_page   - first of the requested pages
    //////       num_of_pages - number of contiguous free pages requested
    //////       
    //////       allocate() returns a pointer to the selected pages
    //////       
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> void* Arena<Memory_Type>::allocate(int start_page,
								   int num_of_pages) {
      Segment* this_segment = find_segment(start_page);
      return this_segment->allocate(start_page,num_of_pages);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //////                             alloc()
    ////////////////////////////////////////////////////////////////////////////////
    //////     
    //////     alloc() works in three stages:
    //////        1. find the requested number of contiguous pages
    //////        2. marks those pages as allocated
    //////        3. returns a pointer to the allocated space and adjusts
    //////           the page pointers
    //////      
    ////////////////////////////////////////////////////////////////////////////////

    template<class Memory_Type> void* Arena<Memory_Type>::alloc(size_t num_of_pages) {
      // n here is the number of pages
      int start_page = -1;
      while (start_page==-1) {
	start_page = find(num_of_pages);	// find the n requested pages
	if (start_page==-1) grow(next_segment_number++);
      }
      mark_pages(start_page,num_of_pages);	// mark the found pages as assigned
      return allocate(start_page,num_of_pages); // finally allocate the needed pages
    }  // Arena::alloc()

    template<class Memory_Type> void Arena<Memory_Type>::free(void* p, size_t n) {

    }  // Arena::free()
    

    ////////////////////////////////////////////////////////////////////////////////
    //////                             Memory Locks
    ////////////////////////////////////////////////////////////////////////////////
    //////   
    //////    Locks are mutual exclusion locks used to protect process from writing  
    //////    simultaneously to shared memory.  Each allocator will get one 
    //////    semaphore set, and then with that semaphore set, each chunk will 
    //////    individual elements from that semaphore set.  So each chunk is has
    //////    access to the semid and the semnum.  A semid is common for all chunks
    //////    within a given allocator, and then each of the allocator's chunks will
    //////    have its own individual semnum which indicates which semaphore in the
    //////    semid array.
    //////    
    ////////////////////////////////////////////////////////////////////////////////

    class locks {
      enum{num_of_arrays=128,num_in_array=250};
      static int lock_counter;	// counts number of lock sets created
      static int lock_users;	// how many existing lock users
      static int semid[num_of_arrays];  // semid for each lock set
      static char key[num_of_arrays];   // key to each lock set
      // int next_sem_array;	// next semaphore array.
      struct sembuf sem_ops;
      int lock_array;
      int max_semnum;		// max semnum in use.
    public:
      locks();
      ~locks();
      // copy constructor
      locks(const locks& t);
      // assignment operator
      locks& operator=(const locks& t);
      int get_num_of_arrays() {return num_of_arrays;}
      int get_num_in_array() {return num_in_array;}
      void lock(int semnum);
      void unlock(int semnum);
      int get_max_semnum() {return max_semnum;}
      void set_max_semnum(int val) { max_semnum=val;}
    }; // class locks

}  // namespace arena

#endif // ARENA_H


