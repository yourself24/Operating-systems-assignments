#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <dirent.h>
unsigned int nr = 33248;
int main()
{
    char req_name[100];
    unsigned char n;
    mkfifo("RESP_PIPE_33248", 0666);
    int fdread = open("REQ_PIPE_33248", O_RDONLY);
    int fdwrite = open("RESP_PIPE_33248", O_WRONLY);
    if (fdread == -1)
    {
        perror("error opening request pipe!");
        exit(1);
    }
    if (fdwrite == -1)
    {
        perror("error opening response pipe!");
        exit(1);
    }
    if (fdwrite > 0)
    {
        unsigned char len = strlen("CONNECT");
        write(fdwrite, &len, sizeof(unsigned char));
        write(fdwrite, "CONNECT", len);
        printf("SUCCESS\n");
    }
    while (1)
    {
        read(fdread, &n, sizeof(n));
        read(fdread, req_name, n);
        if (strncmp(req_name, "PING", 4) == 0)
        {
            unsigned char ping = strlen("PING");
            write(fdwrite, &ping, 1);
            write(fdwrite, "PING", ping);
            unsigned char pong = strlen("PONG");
            write(fdwrite, &pong, 1);
            write(fdwrite, "PONG", pong);
            write(fdwrite, &nr, sizeof(nr));
            exit(1);
        }
        

        else if (strncmp(req_name, "EXIT", 4) == 0)
        {
            printf("Hopa\n");
            close(fdread);
            close(fdwrite);
            exit(0);
        }
        else
        {
            exit(0);
        }
    }

    return 0;
}