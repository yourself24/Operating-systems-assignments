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
int tid;
int sem64;
int sem5;
int semT511;
int countth = 0;
int semP2;
int semT5112;
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
void *th_funcP2(void *context)
{
    int nr3 = (int)(size_t)context;

    if (nr3 == 2)
    {

        info(BEGIN, 2, nr3);

        info(END, 2, nr3);
        V(semP2, 0);
    }
    if(nr3 == 1)
    {
        P(semP2,1);
        info(BEGIN,2,nr3);
        info(END,2,nr3);
    }
    else if(nr3!=2 && nr3!=1)
    {
        info(BEGIN, 2, nr3);
        info(END, 2, nr3);
    }
    return NULL;
}
void *th_funcP6(void *context)
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
        P(semP2, 0);
        info(BEGIN, 6, nr);
        info(END,6,nr);
        V(semP2,1);
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
    else if (nr != 4 && nr!=2)
    { 

        info(END, 6, nr);
    }

    return NULL;
}
void *th_func44(void *context)
{
    int nr2 = (int)(size_t)context;
    P(sem5, 0);
    info(BEGIN, 5, nr2);
    countth++;
    info(END, 5, nr2);
    V(sem5, 0);
    countth--;

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
    pthread_t threadsp6[15];
    pthread_t threadsp5[45];
    pthread_t threadsp2[10];
    info(BEGIN, 1, 0);
    semP2 = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    if(semP2<0)
    {
        perror("Error creating the semaphore for P2!");
        exit(1);
    }
    semctl(semP2, 0, SETVAL, 0);
    semctl(semP2, 1, SETVAL, 0);
    pid[0] = fork();
    if (pid[0] == 0)
    {
        info(BEGIN, 2, 0);

        for (int i = 1; i < 7; i++)
        {

           if( pthread_create(&threadsp2[i], NULL, th_funcP2, (void *)(size_t)i)!=0)
           {
               perror("Error creating threads for P2!");
               exit(1);
           }
        }

        for (int i = 1; i < 7; i++)
        {
            pthread_join(threadsp2[i], NULL);
        }
        info(END, 2, 0);
        exit(0);
    }
    pid[1] = fork();
    if (pid[1] == 0)
    {
        info(BEGIN, 3, 0);
        pid[2] = fork();
        if (pid[2] == 0)
        {
            info(BEGIN, 4, 0);
            pid[5] = fork();
            if (pid[5] == 0)
            {

                sem5 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
                if (sem5 < 0)
                {
                    perror("Error creating the semaphore for P5!");
                    exit(0);
                }
                semctl(sem5, 0, SETVAL, 4);
                
                semT511 = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
                if (semT511 < 0)
                {
                    perror("Error creating the semaphore set for P5.11!");
                    exit(0);
                }
                semctl(semT511, 0, SETVAL, 0);
                info(BEGIN, 5, 0);
                for (int t = 1; t < 45; t++)
                {
                    if (pthread_create(&threadsp5[t], NULL, th_func44, (void *)(size_t)t) != 0)
                    {
                        perror("Error creating threads for P5!");
                        exit(1);
                    }
                }
                for (int t = 1; t < 45; t++)
                {
                    pthread_join(threadsp5[t], NULL);
                }
                info(END, 5, 0);
                exit(0);
            }
            pid[4] = fork();
            if (pid[4] == 0)
            {
                sem64 = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
                semctl(sem64, 0, SETVAL, 0);
                semctl(sem64, 1, SETVAL, 0);
                if (sem64 < 0)
                {
                    perror("Error creating the semaphore set for P6!");
                    exit(2);
                }
                info(BEGIN, 6, 0);
                for (int i = 1; i < 6; i++)
                {

                    if(pthread_create(&threadsp6[i], NULL, th_funcP6, (void *)(size_t)i)!=0)
                    {
                        perror("Error creating threads for P6!");
                        exit(1);
                    }
                }

                for (int i = 1; i < 6; i++)
                {
                    pthread_join(threadsp6[i], NULL);
                }

                info(END, 6, 0);
                exit(0);
            }
            p4procs(7);
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
