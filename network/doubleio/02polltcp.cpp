#include<myhead.h>
#define SER_PORT 8888
#define SER_IP "172.31.180.118"
#define CLI_PORT 9999
#define CLI_IP  "172.31.180.118"
int main(int argc, char const *argv[])
{
    int cfd=socket(AF_INET,SOCK_STREAM,0);
    if(cfd==-1)
    {
        perror("socket error");
        return -1;
    }
    printf("socket success cfd=%d\n",cfd);

    // struct sockaddr_in cin;
    // cin.sin_family=AF_INET;
    // cin.sin_port=htons(CLI_PORT);
    // cin.sin_addr.s_addr=(inet_addr(CLI_IP));
    // if(bind(cfd,(struct sockaddr*)&cin,sizeof(cin))==-1)
    // {
    //     perror("bind error");
    //     return -1;
    // }
    // printf("bind success\n");

    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(SER_PORT);
    sin.sin_addr.s_addr=(inet_addr(SER_IP));

    if(connect(cfd,(struct sockaddr*)&sin,sizeof(sin))==-1)
    {
        perror("connect error");
        return -1;
    }

    struct pollfd pfds[2];

    pfds[0].fd=0;
    pfds[0].events=POLLIN;

    pfds[1].fd=cfd;
    pfds[1].events=POLLIN;



    char wbuf[128]="";
    while(1)
    {
        int res=poll(pfds,2,-1);

        if(res==-1)
        {
            perror("poll error");
            return -1;
        }



        if(pfds[0].revents==POLLIN)
        {
            bzero(wbuf,sizeof(wbuf));

            fgets(wbuf,sizeof(wbuf),stdin);
            wbuf[strlen(wbuf)-1]=0;

            if(send(cfd,wbuf,sizeof(wbuf),0)==-1)
            {
                perror("send error");
                return -1;
            }
        }
        if(pfds[1].revents==POLLIN)
        {
            if(recv(cfd,wbuf,sizeof(wbuf),0)==0)
            {
                printf("对端下线\n");
                break;
            }
            printf("recv:%s\n",wbuf);
        }
        
    }
    close(cfd);
    return 0;
}
