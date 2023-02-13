// file: arena.cc
// author: Marc Bumble
// June 1, 2000
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

#include <arena.h>

namespace arena {

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
  
  //constructor
  Segment::Segment(int num_pages,Segment* prev_segment)
    : bit_vec(num_of_pages,
	      reinterpret_cast<unsigned char*>(this)+sizeof(Segment)) {
    pages_in_segment=num_pages;
    prev=prev_segment;
    next=0;
    if (prev_segment==0)
      first_page_num=0;
    else {
      first_page_num=prev_segment->get_first_page_num() +
	prev_segment->get_pages_in_segment();
    }
    head=mem=reinterpret_cast<unsigned char*>(this)+sizeof(Segment)+bit_vec_sz;
  };  // Segment() constructor

  // copy constructor
  Segment::Segment(const Segment& t)
    : bit_vec(num_of_pages,
	      reinterpret_cast<unsigned char*>(this)+sizeof(Segment)) {
    pages_in_segment=t.pages_in_segment;
    prev=t.prev;		// why would you want to make a copy?
    next=t.next;		// why would you want to make a copy?
    first_page_num=t.first_page_num;
    // not sure if these next two memory pointer values hold
    mem=reinterpret_cast<unsigned char*>(this)+sizeof(Segment)+bit_vec_sz;
    head=(t.head-t.mem)+mem;
  };
  
  // assignment operator
  Segment& Segment::operator=(const Segment& t) {
    if (this != &t) {		// avoid self-assignment: t=t
      pages_in_segment=t.pages_in_segment;
      prev=t.prev;		// why would you want to make a copy?
      next=t.next;		// why would you want to make a copy?
      first_page_num=t.first_page_num;
      // not sure if these next two memory pointer values hold
      mem=reinterpret_cast<unsigned char*>(this)+sizeof(Segment)+bit_vec_sz;
      head=(t.head-t.mem)+mem;
    }
    return *this;
  };
  
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                             find()
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////       num_of_pages - number of contiguous free pages requested
  //////       
  //////       returns the start block as a global number, across all segments,
  //////       of the requested contiguous memory block.  Use the bit vectors to
  //////       search for the requested memory.
  //////       
  //////       
  ////////////////////////////////////////////////////////////////////////////////
  
  int Segment::find(int num_of_pages) {
    
    int local_start_block = bit_vec.find(num_of_pages);
    if (local_start_block==-1) {
      // couldn't find the free pages in this segment see if there is a
      // next segment
      if (next==0)
	return -1;
      local_start_block = next->find(num_of_pages);
      if (local_start_block==-1) {
	// if the requested page cannot be delivered recursively, there is
	// a serious problem.  Abort here.
	std::cerr << __FILE__ << ':' << __LINE__ << ':' << " find(): ";
	std::cerr << "requested memory page lookup failed to allocate blocks.\n";
      }
      // get the value up to the global page value.
      local_start_block += pages_in_segment;
    }
    return local_start_block + first_page_num;
  };
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                             mark_pages()
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////       start_page   - first of the requested pages
  //////       num_of_pages - number of contiguous free pages requested
  //////       
  //////       mark_pages() is used to set the bit in the segment bit vector 
  //////       to indicate that the num_of_pages starting with the start_page are
  //////       no longer free and have been assigned.
  //////       
  //////       
  ////////////////////////////////////////////////////////////////////////////////
  
  void Segment::mark_pages(int start_page, int num_of_pages) {
    // find the local_start_page within this segment, first is 0
    int local_start_page=start_page-first_page_num;
    bit_vec.mark(local_start_page,num_of_pages);
  };
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                             clear_pages()
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////       start_page   - first of the requested pages
  //////       num_of_pages - number of contiguous free pages requested
  //////       
  //////       clear_pages() used to clear the bit in the segment bit vector
  //////       corresponding to newly freed pages of memory which may now
  //////       be recycled.
  //////       
  //////       
  ////////////////////////////////////////////////////////////////////////////////
  
  void Segment::clear_pages(int start_page, int num_of_pages) {
    // find the local_start_page within this segment, first is 0
    int local_start_page=start_page-first_page_num;
    bit_vec.clear(local_start_page,num_of_pages);
  };
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                             allocate()
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////       start_page   - first of the requested pages
  //////       num_of_pages - number of contiguous free pages requested
  //////       
  //////       allocate() return a pointer to the num_of_pages starting at
  //////       start_page.
  //////       
  //////       
  ////////////////////////////////////////////////////////////////////////////////
  
  void* Segment::allocate(int start_page, int num_of_pages) {
    // find the local_start_page within this segment, first is 0
    int local_start_page=start_page-first_page_num;
    return (mem + (local_start_page*page_size));
  };
  
  
  
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
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                           locks::locks()  
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////    Constructor, initializes arrays.
  //////    
  ////////////////////////////////////////////////////////////////////////////////

  int locks::lock_counter=0;		// static member, initialized once
  int locks::lock_users=0;		// static member, initialized once
  int locks::semid[locks::num_of_arrays] = {0};
  char locks::key[num_of_arrays]={'0'};

  // constructor
  locks::locks() {
    if (lock_counter==0) {
      // initialize and allocate the static lock members
      // done once only.
//        for (int i=0; i<num_of_arrays; i++) {
//  	semid[i] = 0;
//  	key[i] = static_cast<char>(i);
//        }
    }  // if (lock_counter++==0)
    // increment the static lock counter
    lock_array=lock_users;	// set of semaphores this lock instantiation uses
    // set the semaphore key
    key_t mykey = ftok(".",key[lock_array]);
    semid[lock_array] = semget(mykey,num_in_array, IPC_CREAT|0666);
    if (semid[lock_array] < 0) {
      std::cerr << "File: " << __FILE__ << ':' << __LINE__ << ':'<< "locks::make_lock()";
      std::cerr << ": semaphore creation error: " << errno << std::endl;
    }
    for (int i=0; i<num_in_array; i++) {
      sem_ops.sem_num=i;
      sem_ops.sem_op=0;         // initialize all at 0==unlock, (1==lock)
      sem_ops.sem_flg=SEM_UNDO;
      semop(semid[lock_array],&sem_ops,SEM_UNDO);
    }
    max_semnum=0;
    lock_counter++;
    lock_users++;
  };


  // destructor
  locks::~locks() {
    if (--lock_users==0) {
      // clean up and remove the locks
      union semun semctl_arg;
      for (int j=0; j<lock_counter; j++) {
	for (int i=0; i<max_semnum; i++) {
	  int semctlresult = semctl(semid[j],i, IPC_RMID,semctl_arg);
	  if ((semctlresult!=0)&&(errno!=EINVAL)) {
	    std::cout << __FILE__ << ':' << __LINE__ << " locks::~locks(): ";
	    std::cout << " semid: " << semid[j] << "semctlresult: ";
	    std::cout << semctlresult << " errno: " << errno << std::endl;
	  }
#ifdef SHARED_MEMORY_MESG
	  std::cout << "semid: " << semid[j] << " shmctlresult: ";
	  std::cout << semctlresult << " errno: " << errno << std::endl;
#endif
	}
      }
    }
  };
  
  // copy constructor
  locks::locks(const locks& t) {
    sem_ops=t.sem_ops;
    // next_sem_array=t.next_sem_array;
  };

  
  // assignment operator
  locks& locks::operator=(const locks& t) {
    if (this != &t) {		// avoid self assignment: t=t
      sem_ops=t.sem_ops;
    }
    return *this;
  };

  
  ////////////////////////////////////////////////////////////////////////////////
  //////                           locks::make_lock() 
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////    Generate a set of semaphore locks
  //////    
  ////////////////////////////////////////////////////////////////////////////////
  
//    int locks::make_lock() {
//      // set the semaphore key
//      key_t mykey = ftok(".",key[lock_counter]);
//      semid[lock_counter] = semget(mykey,num_in_array, IPC_CREAT|0666);
//      if (semid[lock_counter]<0) {
//        std::cerr << "File: " << __FILE__ << ':' << __LINE__ << ':'<< "locks::make_lock()";
//        std::cerr << ": semaphore creation error: " << errno << std::endl;
//      }
//      for (int i=0; i<num_of_arrays; i++) {
//        sem_ops.sem_num=i;
//        sem_ops.sem_op=0;         // initialize all at 0==unlock, (1==lock)
//        sem_ops.sem_flg=SEM_UNDO;
//        semop(semid[lock_counter],&sem_ops,SEM_UNDO);
//      }
//      return semid[lock_counter];
//    };  // make_lock()
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                   locks::lock(int semid,int semnum)
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////    semid - the semaphore array id
  //////    semnum- the semaphore mutex lock with the array identified by semid
  //////    
  //////    lock get the requested lock.
  //////    
  ////////////////////////////////////////////////////////////////////////////////
  
  void locks::lock(int semnum) {
    struct sembuf semops;
    semops.sem_num=semnum;
    semops.sem_op=1;		// adds one to lock
    semops.sem_flg=SEM_UNDO;
    int result = semop(semid[lock_array],&semops,1);
    if ((result!=0)&&(errno!=EINVAL)) {
      std::cerr << __FILE__ << ':' << __LINE__ << ':'<< "locks::lock()";
      std::cerr << ": semaphore lock errno: " << errno << " result: " << result << std::endl;
    }	
  };
  
  ////////////////////////////////////////////////////////////////////////////////
  //////                 locks::unlock(int semid,int semnum)
  ////////////////////////////////////////////////////////////////////////////////
  //////   
  //////    semid - the semaphore array id
  //////    semnum- the semaphore mutex lock with the array identified by semid
  //////    
  //////    lock get the requested lock.
  //////    
  ////////////////////////////////////////////////////////////////////////////////
  
  void locks::unlock(int semnum) {
    struct sembuf semops;
    semops.sem_num=semnum;
    semops.sem_op=-1;		// subtracts one to unlock
    semops.sem_flg=SEM_UNDO;
    int result = semop(semid[lock_array],&semops,1);
    if ((result!=0)&&(errno!=EINVAL)) {
      std::cerr << __FILE__ << ':' << __LINE__ << ':'<< "locks::unlock()";
      std::cerr << ": semaphore unlock errno: " << errno << " result: " << result << std::endl;
    }	
  };
  
}  // namespace arena



