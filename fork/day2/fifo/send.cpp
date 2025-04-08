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
    int sfd=-1;
    if((sfd=open("./myfifo",O_WRONLY))==-1)
    {
        perror("open error");
        return -1;
    }

    char wbuf[128]="";
    while(1)
    {
        std::cout<<"请输入>>>>";
        fgets(wbuf,sizeof(wbuf),stdin);
        wbuf[strlen(wbuf)-1]=0;

        write(sfd,wbuf,strlen(wbuf));

        if(strcmp(wbuf,"quit")==0)
        {
            break;
        }
    }
    close(sfd);
    return 0;
}
