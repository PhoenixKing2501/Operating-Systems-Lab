// file: shared_memory.cc
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

#include <shared_memory.h>

namespace mem_space {

    ////////////////////////////////////////////////////////////////////////////////
    //////                             Shared Memory
    ////////////////////////////////////////////////////////////////////////////////
    //////   
    //////    Shared Memory is the class interface to the specific operating system.
    //////    This is class is user defined and allows the Arena to generate the 
    //////    generic segments of memory which it will provide to the allocator.  
    //////    
    //////    
    //////    
    ////////////////////////////////////////////////////////////////////////////////

  int shared::shmid[num_of_segments] = {0};
  unsigned char* shared::buffers[num_of_segments] = {0};
  char shared::key[num_of_segments] = {0};
  
    // constructor
    shared::shared(int pages,int overhead) {
//        for (int i=0; i<num_of_segments; i++) {
//  	shmid[i]=0;
//  	buffers[i]=0;
//  	key[i] = static_cast<char>(i);
//        }
      next_buffer=0;
      segment_size=pages*page_size + overhead;
    };

    // destructor
    shared::~shared() {
      struct shmid_ds temp;
      for (int i=0; i<num_of_segments; i++) {
	if (shmid[i]!=0) {
	  // deallocate the shared memory segment
	  int shmctlresult = shmctl(shmid[i], IPC_RMID,&temp);
	  if (shmctlresult != 0) {
	    std::cerr << "shmid: " << shmid[i] << " shmctlresult: ";
	    std::cerr << shmctlresult << " errno: " << errno << std::endl;
	  }
	}
      }
    }; 

    // copy constructor
    shared::shared(const shared& t) {
      for (int i=0; i<num_of_segments; i++) {
	shmid[i]=t.shmid[i];
	buffers[i]=t.buffers[i];
	key[i]=t.key[i];
      }
      next_buffer=t.next_buffer;
      segment_size=t.segment_size;
    }; // copy constructor

    // assignment operator
    shared& shared::operator=(const shared& t) {
      if (this != &t) {		// prevent self-assignment: t=t
	for (int i=0; i<num_of_segments; i++) {
	  shmid[i]=t.shmid[i];
	  buffers[i]=t.buffers[i];
	  key[i]=t.key[i];
	}
	next_buffer=t.next_buffer;
	segment_size=t.segment_size;
      }  // if (this != &t)
      return *this;
    };  // assignment operator

    void* shared::allocate(bool* initialized) {
      // returns a pointer to the newly allocated page, parameter indicates if
      // the page is newly allocated and therefore NOT initialized, or if the
      // page was already in use and simply attached.


      // set the segment key
      key_t mykey = ftok(".",key[next_buffer]);
      shmid[next_buffer] = shmget(mykey, segment_size, IPC_CREAT|0666);
      if (errno != 0) {
	std::clog << "File: " << __FILE__ << ':' << __LINE__ ;
	std::clog << ": shared memory get/attach errno: " << errno << std::endl;
      }
      struct shmid_ds temp_shinfo;
      shmctl(shmid[next_buffer], IPC_STAT, &temp_shinfo);
      if (temp_shinfo.shm_nattch == 0)
	(*initialized)=false;
      else
	(*initialized)=true;
      buffers[next_buffer] =
	static_cast<unsigned char*>(shmat(shmid[next_buffer], 0, 0));
#ifdef SHARED_MEMORY_MESG
      stl::cerr << __FILE__ << ':' << __LINE__ << ':' << "shared::allocate(): ";
      stl::cerr << "process " << getpid() << " attached to shmid: " << shmid[next_buffer];
      stl::cerr << stl::endl;
#endif 
      return buffers[next_buffer++];
    };

    void shared::free(int shmid_val) {
      struct shmid_ds temp;
      int shmctlresult = shmctl(shmid[shmid_val], IPC_RMID,&temp);
      if (shmctlresult != 0) {
	std::cout << "shmid: " << shmid[shmid_val] << " shmctlresult: ";
	std::cout << shmctlresult << " errno: " << errno << std::endl;
      }
    };

}  // namespace mem_space
