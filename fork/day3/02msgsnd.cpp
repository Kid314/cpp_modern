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
        printf("请输入消息类型:");
        scanf("%ld",&buf.mtype);
        getchar();
        printf("请输入消息正文:");
        fgets(buf.mtext,MSGSZ,stdin);
        buf.mtext[strlen(buf.mtext)-1]='\0';
        msgsnd(msqid,&buf,MSGSZ,0);
        printf("success\n");
        if(strcmp(buf.mtext,"quit")==0)
        {
            break;
        }
    }
    


    return 0;
}
