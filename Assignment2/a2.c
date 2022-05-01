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
int sem64;
pthread_t threadsP6[10];
void P(int semid, int semnum)
{
    struct sembuf op = {semnum, -1, 0};

    semop(semid, &op, 1);
}
void V(int semid, int semnum)
{
    struct sembuf op = {semnum, +1, 0};
    semop(semid, &op, 1);
}
void *th_func(void *context)
{

    int nr = (int)(size_t)context;
    if (nr == 4)
    {

        P(sem64, 0);
        info(BEGIN, 6, nr);
        info(END, 6, nr);
        V(sem64, 1);
    }
    else if (nr == 2)
    {
        info(BEGIN, 6, nr);
    }
    else
    {
        info(BEGIN, 6, nr);
    }
    if (nr == 5)
    {
        V(sem64, 0);
        P(sem64, 1);
        info(END, 6, nr);
    }
    else if (nr != 4)
    {

        info(END, 6, nr);
    }

    return NULL;
}
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
int main(int argc, char **argv)
{
    init();
    info(BEGIN, 1, 0);
    sem64 = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    semctl(sem64, 0, SETVAL, 0);
    semctl(sem64, 1, SETVAL, 0);
    if (sem64 < 0)
    {
        perror("Error creating the semaphore set for P6");
        exit(2);
    }
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
        pid[4] = fork(); // p4 starts
        if (pid[4] == 0)
        {
            info(BEGIN, 4, 0);
            p4procs(5); // p5 starts
            pid[6] = fork();
            if (pid[6] == 0)
            {
                info(BEGIN, 6, 0);
                for (int i = 1; i < 6; i++)
                {

                    pthread_create(&threadsP6[i], NULL, th_func, (void *)(size_t)i);
                }

                for (int i = 1; i < 6; i++)
                {
                    pthread_join(threadsP6[i], NULL);
                }
                info(END, 6, 0);
                exit(0);
            }
            p4procs(7); // p7 starts
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
    info(END, 1, 0);
    return 0;
}