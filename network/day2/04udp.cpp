#include<myhead.h>
#define SER_PORT 8888
#define SER_IP "172.31.180.118"
#define CLI_PORT 9999
#define CLI_IP  "172.31.180.118"
int main(int argc, char const *argv[])
{
    int sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)
    {
        perror("socket error");
        return -1;
    }
    printf("socket syccess sfd=%d\n",sfd);

    // struct sockaddr_in sin;
    // sin.sin_family=AF_INET;
    // sin.sin_port=htons(CLI_PORT);
    // sin.sin_addr.s_addr=inet_addr(CLI_IP);

    // if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))==-1)
    // {
    //     perror("bind error");
    //     return -1;
    // }
    // printf("bind sucess\n");

    char rbuf[128]="";
    struct sockaddr_in cin;
    cin.sin_family=AF_INET;
    cin.sin_port=htons(SER_PORT);
    cin.sin_addr.s_addr=inet_addr(SER_IP);

    while(1)
    {
        bzero(rbuf,sizeof(rbuf));
        fgets(rbuf,sizeof(rbuf),stdin);
        rbuf[strlen(rbuf)-1]=0;
        sendto(sfd,rbuf,strlen(rbuf),0,(struct sockaddr*)&cin,sizeof(cin));
        printf("send success\n");

        recvfrom(sfd,rbuf,sizeof(rbuf),0,NULL,NULL);
        printf("%s\n",rbuf);
    }
    close(sfd);
    return 0;
}