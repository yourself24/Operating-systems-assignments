#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <sys/stat.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
pid_t pid[10];
void p4procs(int i)
{
    pid[i] = fork();
    if (pid[i] == 0)
    {
        info(BEGIN, i, 0);
        info(END, i, 0);
        exit(0);
    }
}
int main(int argc,char **argv)
{
     init();
      info(BEGIN, 1, 0);
      pid[2] = fork();
      if (pid[2] == 0)
    {
        info(BEGIN, 2, 0);
        info(END, 2, 0);
        exit(0);
    }
     pid[3] = fork();
    if (pid[3] == 0)
    {
        info(BEGIN, 3, 0);
        pid[4] = fork(); //p4 starts
        if (pid[4] == 0)
        {
            info(BEGIN, 4, 0);
            p4procs(5); //p5 starts
            p4procs(6); //p6 starts
            p4procs(7); //p7 starts
            wait(NULL); // p5 ends
            wait(NULL); // p6 ends
            wait(NULL); // p7 ends
            info(END, 4, 0);
            exit(0);
        }
        wait(NULL); // p4 ends
        info(END, 3, 0);
        exit(0);
    }

    wait(NULL); // p2 ends
    wait(NULL); // p3 ends
    info(END,1,0);
    return 0;
}
