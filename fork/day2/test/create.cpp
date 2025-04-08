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
    if(mkfifo("./myfifo",0664)==-1)
    {
        perror("make error");
        return -1;
    }
    if(mkfifo("./myfifo2",0664)==-1)
    {
        perror("make error");
        return -1;
    }
    std::cout<<"success\n";
    return 0;
}
