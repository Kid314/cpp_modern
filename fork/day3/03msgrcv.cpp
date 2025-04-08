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

struct msgBuf
{
    long mtype;
    char mtext[1024];
};
#define MSGSZ (sizeof(struct msgBuf)-sizeof(long))

int main(int argc, char const *argv[])
{
    key_t key=ftok("/",'k');

    if(key==-1)
    {
        perror("ftok error");
        return -1;
    }
    printf("key=%d\n",key);
    int msqid=-1;
    if((msqid=msgget(key,IPC_CREAT|0664))==-1)
    {
        perror("msgget error");
        return -1;
    }
    printf("msqid=%d\n",msqid);

    struct msgBuf buf;
    while(1)
    {
        bzero(&buf,sizeof(buf));

        msgrcv(msqid,&buf,MSGSZ,1,0);
        printf("%s\n",buf.mtext);
        if(strcmp(buf.mtext,"quit")==0)
        {
            break;
        }
    }
    if(msgctl(msqid,IPC_RMID,NULL)==-1)
    {
        perror("msgctl error");
    }



    return 0;
}
