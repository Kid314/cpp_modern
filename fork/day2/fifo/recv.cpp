#include<iostream>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<string>
#include<sys/stat.h>
int main(int argc, char const *argv[])
{
    int rfd=-1;
    if((rfd=open("./myfifo",O_RDONLY))==-1)
    {
        perror("open error");
        return -1;
    }

    char rbuf[128]="";
    while(1)
    {
        bzero(rbuf,sizeof(rbuf));
        read(rfd,rbuf,sizeof(rbuf));
        printf("%s\n",rbuf);
        if(strcmp(rbuf,"quit")==0)
        {
            break;
        }
    }
    close(rfd);
    return 0;
}
