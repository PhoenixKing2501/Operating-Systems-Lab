// file: semaphore.h
// author: Marc Bumble
// November 15, 1999
// Contains classes related to semaphores 
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

extern "C" {
#include <unistd.h>		// for fork(),getpid(),getppid()
#include <sys/types.h>		// for shared memory shmget(),shmat(),ftok(),semop()
#include <sys/ipc.h>		// for shared memory shmget(),shmat(),ftok(),semop()
#include <sys/sem.h>		// for shared memory shmget(),shmat(),ftok(),semop()
  // #include <errno.h>		// for shared memory shmget(),shmat(),ftok()
  // extern int errno;
}
//    union semun {
//      int		val;
//      struct semid_ds	*buf;
//      ushort		*array;
//    } semctl_arg;

extern "C" int semop(int semid, struct sembuf *sops, unsigned nsops);
extern "C" int semget(key_t key, int nsems, int semflg);
extern "C" int semctl(int  semid, int semnum, int cmd, ...);

#define BIGCOUNT (100)

#ifndef TRAFIX_SEMAPHORE_H
#define TRAFIX_SEMAPHORE_H

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
  int val;                    /* value for SETVAL */
  struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array for GETALL, SETALL */
  struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

namespace Semaphore {

  void sem_op(int id, int value);
  void sem_signal(int id);
  void sem_wait(int id);
  void sem_close(int id);
  void sem_rm(int id);
  int sem_open(key_t	key);
  int sem_create(key_t key, int initval);
  
};  // namespace Semaphore 

#endif
