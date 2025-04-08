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
    
    char wbuf[128]="nihao a";
    char rbuf[128]="";

    write(fildes[1],wbuf,strlen(wbuf));
    read(fildes[0],rbuf,sizeof(rbuf));
    printf("%s\n",rbuf);
    close(fildes[0]);
    close(fildes[1]);
    
    
    return 0;
}