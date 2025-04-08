#include<myhead.h>
#define SER_PORT 8888
#define SER_IP "172.31.180.118"

struct info
{
    int newfd;
    struct sockaddr_in cin;
};

void* deal(void* arg)
{
    int newfd=((struct info*)arg)->newfd;
    struct sockaddr_in cin=((struct info*)arg)->cin;
    char rbuf[128]="";
    while(1)
    {
        bzero(rbuf,sizeof(rbuf));

        int res=recv(newfd,rbuf,sizeof(rbuf),0);
        if(res==0)
        {
            printf("对端下线了\n");
            break;
        }
        printf("[%s:%d]:%s\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),rbuf);

        strcat(rbuf,"*_*");
        if(send(newfd,rbuf,strlen(rbuf),0)==-1)
        {
            perror("send error");
            return (void*)-1;
        }
        printf("发送成功\n");
    }
    close(newfd);
    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
        perror("socket error");
        return -1;
    }
    printf("socket success sfd=%d\n",sfd);

    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(SER_PORT);
    sin.sin_addr.s_addr=inet_addr(SER_IP);

    if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))==-1)
    {
        perror("bind error");
        return -1;
    }
    printf("bind success\n");

    if(listen(sfd,128)==-1)
    {
        perror("listen error");
        return -1;
    }
    printf("listen success\n");


    struct sockaddr_in cin;
    socklen_t socklen=sizeof(cin);
    while(1)
    {
        //accept会预分配
        int newfd=accept(sfd,(struct sockaddr*)&cin,&socklen);
        if(newfd==-1)
        {
            perror("accept success\n");
            return -1;
        }
        struct info buf={newfd,cin};
        printf("[%s:%d]:接受了新的连接,newfd=%d\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);

        //创建分支线程
        pthread_t tid=-1;
        if(pthread_create(&tid,NULL,deal,&buf)!=0)
        {
            printf("thread error\n");
            return -1;
        }
        pthread_detach(tid);
    }
    close(sfd);

    return 0;
}
