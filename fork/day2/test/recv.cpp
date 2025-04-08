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
    pid_t pid=fork();
    if(pid>0)
    {
        int rfd=-1;
        if((rfd=open("./myfifo",O_RDONLY))==-1)
        {
            perror("open error");
            return -1;
        }

        char rbuf[128]="";
        while(1)
        {
            bzero(rbuf,sizeof(rbuf));
            read(rfd,rbuf,sizeof(rbuf));
            printf("%s\n",rbuf);
            if(strcmp(rbuf,"quit")==0)
            {
                break;
            }
        }
        close(rfd);


        
        wait(NULL);
    }
    else if(pid==0)
    {
        int sfd=-1;
        if((sfd=open("./myfifo2",O_WRONLY))==-1)
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


        exit(EXIT_SUCCESS);
    }
    else
    {
        perror("fork error");
        return -1;
    }
    
    return 0;
}
