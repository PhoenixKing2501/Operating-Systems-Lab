extern "C" {
#include <unistd.h>		// for fork(),getpid(),getppid()
#include <fcntl.h>
#include <sys/types.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/ipc.h>		// for shared memory shmget(),shmat(),ftok()
#include <sys/shm.h>		// for shared memory shmget(),shmat(),ftok()
#include <errno.h>		// for shared memory shmget(),shmat(),ftok()
extern int errno;
}

#include <vector>
#include <iostream>

#include <allocator.h>

int main(int argc, char **argv) {
  std::vector<int,allocator_ns::Pool_alloc<int> > w;
  std::vector<int,allocator_ns::Pool_alloc<int> > x;
  std::vector<int,allocator_ns::Pool_alloc<int> > y;
  std::vector<int,allocator_ns::Pool_alloc<int> > z;
  arena::Arena<mem_space::shared> Shared;

  for (int i=0; i<200000; i++) {
    std::cout << "index i: " << i << std::endl;
    w.push_back(i);
  }
  for (int i=2000; i>0; i--) {
    std::cout << "index i: " << i << std::endl;
    x.push_back(i);
  }
  for (int i=2000; i<4000; i++) {
    std::cout << "index i: " << i << std::endl;
    y.push_back(i);
  }
  for (int i=4000; i>2000; i--) {
    std::cout << "index i: " << i << std::endl;
    z.push_back(i);
  }
  for (std::vector<int,allocator_ns::Pool_alloc<int> >::iterator obj = w.begin();
       obj != w.end();
       obj++) {
    std::cout << "Number inserted: " << *obj << std::endl;
  }
  for (std::vector<int,allocator_ns::Pool_alloc<int> >::iterator obj = x.begin();
       obj != x.end();
       obj++) {
    std::cout << "Number inserted: " << *obj << std::endl;
  }
  for (std::vector<int,allocator_ns::Pool_alloc<int> >::iterator obj = y.begin();
       obj != y.end();
       obj++) {
    std::cout << "Number inserted: " << *obj << std::endl;
  }
  for (std::vector<int,allocator_ns::Pool_alloc<int> >::iterator obj = z.begin();
       obj != z.end();
       obj++) {
    std::cout << "Number inserted: " << *obj << std::endl;
  }
  for (std::vector<int,allocator_ns::Pool_alloc<int> >::iterator obj = w.begin();
       obj != w.end();
       obj++) {
    std::cout << "Number inserted: " << *obj << std::endl;
  }
}




