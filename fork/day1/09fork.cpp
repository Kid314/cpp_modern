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
    pid_t pid =fork();
    if(pid>0)
    {
        pid_t pid_2=fork();
        if(pid_2>0)
        {
            std::cout<<"hello\n";
        }
        else if(pid_2==0)
        {

        }
        else
        {
            perror("fork error");
            return -1;
        }
    }
    else if(pid==0)
    {

    }
    else
    {
        perror("fork error");
        return -1;
    }
    return 0;
}
