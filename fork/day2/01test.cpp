#include<iostream>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<string>
int num=520;
int main(int argc, char const *argv[])
{
    pid_t pid=fork();
    if(pid>0)
    {
        num=1314;
        std::cout<<"num="<<num<<"\n";

        wait(NULL);
    }
    else if(pid==0)
    {
        sleep(3);
        std::cout<<"num="<<num<<"\n";
    }
    else
    {
        perror("fork error");
        return -1;
    }
    return 0;
}
