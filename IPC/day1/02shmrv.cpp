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
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#define PAGE_SIZE 4096
int main(int argc, char const *argv[])
{
    key_t key=ftok("/",'k');
    if(key==-1)
    {
        perror("ftok error");
        return -1;
    }
    printf("key=%#x\n",key);

    int shmid=-1;
    if((shmid=shmget(key,PAGE_SIZE,IPC_CREAT|0664))==-1)
    {
        perror("shmget error");
        return -1;
    }
    printf("shmid=%d\n",shmid);

    char* addr=(char*)shmat(shmid,NULL,0);
    if(addr==(void*)-1)
    {
        perror("shmat error");
        return -1;
    }
    printf("addr=%p\n",addr);

    while(1)
    {
        printf("读取到：%s\n",addr);
        if(strcmp(addr,"quit")==0)
        {
            break;
        }
    }
    if(shmdt(addr)==-1)
    {
        perror("取消映射\n");
    }

    if(shmctl(shmid,IPC_RMID,NULL)==-1)
    {
        perror("shmctl error");
        return -1;
    }
    return 0;
}
