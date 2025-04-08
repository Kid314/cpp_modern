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
#include<signal.h>

void hander(int signo)
{
    if(signo==SIGINT)
    {
        std::cout<<"oooooo\n";
    }
}
int main(int argc, char const *argv[])
{
    if(signal(SIGINT,hander)==SIG_ERR)
    //SIG_IGN   SIG_DFL
    {
        perror("SIGNAL error");
        return -1;
    }

    while(1)
    {
        std::cout<<"hello\n";
        sleep(1);
    }
    return 0;
}
