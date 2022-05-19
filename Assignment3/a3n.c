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
unsigned char err = strlen("ERROR");
unsigned char succ = strlen("SUCCESS");

void wErr(int fd)
{
    write(fd, &err, 1);
    write(fd, "ERROR", err);
}
void wSucc(int fd)
{

    write(fd, &succ, 1);
    write(fd, "SUCCESS", succ);
}
int main()
{
    unsigned char n;
    char *map = NULL;
    int fdmem = 0;
    unsigned int sh_size = 0;
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
        // printf("SUCCESS\n");
    }
    
   
   
    while (1)
    { 
        
        read(fdread, &n, sizeof(n));
        char *req_name=(char*)malloc(n*sizeof(char));
        read(fdread, req_name, n);
        printf("Request is: %s \n",req_name);
        if (strncmp(req_name, "PING",strlen("PING")) == 0)
        {
            unsigned char ping = strlen("PING");
            write(fdwrite, &ping, 1);
            write(fdwrite, "PING", ping);
            unsigned char pong = strlen("PONG");
            write(fdwrite, &pong, 1);
            write(fdwrite, "PONG", pong);
            write(fdwrite, &nr, sizeof(nr));
            free(req_name);
            exit(1);
        }
        if (strncmp(req_name, "CREATE_SHM",strlen("CREATE_SHM")) == 0)
        {
            unsigned char req = strlen(req_name);

            if (read(fdread, &sh_size, sizeof(unsigned int)) < 0)
            {
                close(fdread);
                exit(1);
            }
            fdmem = shm_open("/tnYUYnaf", O_CREAT | O_RDWR, 0664);
            ftruncate(fdmem, sh_size);
            if (fdmem < 0)
            {
                exit(1);
            }
            map = (char *)mmap(0, sh_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdmem, 0);

            write(fdwrite, &req, 1);
            write(fdwrite, "CREATE_SHM", req);

            if (map == MAP_FAILED)
            {
              wErr(fdwrite);
                exit(1);
            }
            else
            {
                wSucc(fdwrite);
                exit(1);
            }
            free(req_name);
            exit(1);
        }
        if (strncmp(req_name, "WRITE_TO_SHM", strlen("WRITE_TO_SHM")) == 0)
        {
            printf("ACI II WRITE\n");
            unsigned char len = strlen("WRITE_TO_SHM");
            write(fdwrite, &len, 1);
            write(fdwrite, "WRITE_TO_SHM", len);

            unsigned int offset = 0;
            if (read(fdread, &offset, sizeof(unsigned int)) < 0)
            {
                close(fdread);
            }
            unsigned int value = 0;
            if (read(fdread, &value, sizeof(unsigned int)) < 0)
            {
                close(fdread);
            }

            if (offset >= 0 && offset <= 1735790 && offset + sizeof(value) <= 1735790)
            {
                if (memcpy(map + offset, &value, sizeof(value)) < 0)
                {
                  wErr(fdwrite);
                    exit(1);
                }
                else
                {
                  wSucc(fdwrite);
                }
            }
            else
            {
                wErr(fdwrite);
            }
            free(req_name);
            exit(1);
        }
        if (strncmp(req_name, "MAP_FILE", strlen("MAP_FILE")) == 0)
        {

            printf("ACI II MAP\n");
            unsigned char req2 = strlen("MAP_FILE");
            unsigned char map_size = 0;
            if (read(fdread, &map_size, sizeof(unsigned char)) < 0)
            {
                close(fdread);
            }
            char *sh_name = malloc(sizeof(char) * (map_size + 1));
            if (read(fdread, sh_name, map_size) < 0)
            {

                exit(1);
            }
            write(fdwrite, &req2, 1);
            write(fdwrite, "MAP_FILE", req2);
            int fd = -1;
            fd = open(sh_name, O_RDONLY);
            if (fd == -1)
            {
                wErr(fdwrite);
                exit(1);
            }

            map_size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);
            char *map2 = mmap(0, map_size, PROT_READ, MAP_SHARED, fd, 0);
            if (map2 == MAP_FAILED)
            {
                close(fd);
                wErr(fdwrite);
                exit(1);
            }
            else
            {
                wSucc(fdwrite);
                close(fd);
            }
            free(req_name);
            exit(1);
        }

        if (strncmp(req_name, "EXIT", 4) == 0)
        {
            printf("Hopa\n");
            close(fdread);
            close(fdwrite);
            unlink("RESP_PIPE_33248");
            free(req_name);
            exit(0);
        }
       else
        {
            exit(0);
        }
    }

    return 0;
}