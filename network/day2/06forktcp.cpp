#include<myhead.h>
#define SER_PORT 8888
#define SER_IP "172.31.180.118"

void handler(int signo)
{
    if(signo==SIGCHLD)
    {
        while(waitpid(-1,NULL,WNOHANG)>0);
    }
}
int main(int argc, char const *argv[])
{
    if(signal(SIGCHLD,handler)==SIG_ERR)
    {
        perror("signal error");
        return -1;
    }
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
        int newfd=accept(sfd,(struct sockaddr*)&cin,&socklen);
        if(newfd==-1)
        {
            perror("accept success\n");
            return -1;
        }
        printf("[%s:%d]:接受了新的连接\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port));

        pid_t pid=fork();
        if(pid>0)
        {
            //父进程
            close(newfd);
        }
        else if(pid==0)
        {
            close(sfd);
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
                    return -1;
                }
                printf("发送成功\n");
            }
            close(newfd);
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("fork error");
            return -1;
        }
    }
    close(sfd);

    return 0;
}
