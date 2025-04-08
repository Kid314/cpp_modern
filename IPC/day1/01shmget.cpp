#include<myhead.h>
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
        printf("请输入:");
        fgets(addr,PAGE_SIZE,stdin);
        addr[strlen(addr)-1]=0;
        if(strcmp(addr,"quit")==0)
        {
            break;
        }

    }
    sleep(5);
    if(shmdt(addr)==-1)
    {
        perror("取消映射\n");
    }

    return 0;
}
