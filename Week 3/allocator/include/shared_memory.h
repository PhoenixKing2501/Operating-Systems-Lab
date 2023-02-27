// file: shared_memory.h
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

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

extern "C" {
#include <unistd.h>		// for fork(),getpid(),getppid()
#include <fcntl.h>
#include <sys/types.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/ipc.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/shm.h>		// for shared memory shmget(),shmat(),ftok()
  // #include <errno.h>		// for shared memory shmget(),shmat(),ftok()
  // extern int errno;
}

#include <cerrno>
#include <cstdlib>

#include <global_defs.h>

#define PAGE_SIZE (8192)

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

    class shared {
      enum {num_of_segments=128,page_size=PAGE_SIZE};
      static int shmid[num_of_segments];
      static unsigned char* buffers[num_of_segments];
      static char key[num_of_segments];
      int next_buffer;
      int segment_size;
    public:
      // constructor
      shared(int pages,int overhead);
      // destructor
      ~shared();
      // copy constructor
      shared(const shared& t);
      // assignment operator
      shared& operator=(const shared& t);
      void* allocate(bool* initialized);
      void free(int shmid_val);
    };  // class shared


}  // namespace mem_space

#endif // SHARED_MEMORY_H

