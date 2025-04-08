#include<iostream>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<string>
int main(int argc, char const *argv[])
{
    int fildes[2];
    if(pipe(fildes)==-1)
    {
        perror("pipe error");
        return -1;
    }
    std::cout<<fildes[0]<<","<<fildes[1]<<"\n";
    pid_t pid=fork();
    if(pid>0)
    {
        close(fildes[0]);
        char wbuf[128]="hello world";

        write(fildes[1],wbuf,strlen(wbuf));
        close(fildes[1]);
        wait(NULL);
    }
    else if(pid==0)
    {
        close(fildes[1]);
        char rbuf[128]="";

        read(fildes[0],rbuf,sizeof(rbuf));
        printf("%s\n",rbuf);
        close(fildes[0]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        perror("fork error");
        return -1;
    }
    return 0;
}