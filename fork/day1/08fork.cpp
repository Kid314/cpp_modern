#include<iostream>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<string>

int get_file_size(const char* srcfile,const char* destfile)
{
    int srcfd,destfd;
    if((srcfd=open(srcfile,O_RDONLY))==-1)
    {
        perror("open srcfile error");
        return -1;
    }
    if((destfd=open(destfile,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
    {
        perror("open destfile error");
        return -1;
    }

    int len=lseek(srcfd,0,SEEK_END);
    close(srcfd);
    close(destfd);
    return len;

}

int copyfile(const char* srcfile,const char* destfile,int start,int len)
{
    int srcfd,destfd;
    if((srcfd=open(srcfile,O_RDONLY))==-1)
    {
        perror("open srcfile error");
        return -1;
    }
    if((destfd=open(destfile,O_WRONLY))==-1)
    {
        perror("open destfile error");
        return -1;
    }

    lseek(srcfd,start,SEEK_SET);
    lseek(destfd,start,SEEK_SET);

    char buf[128]="";
    int sum=0;
    while(1)
    {
        int res=read(srcfd,buf,sizeof(buf));
        sum+=res;
        if(res==0||sum>=len)
        {
            write(destfd,buf,res-(sum-len));
            break;
        }
        write(destfd,buf,res);
    }
    return 0;
}


int main(int argc, char const *argv[])
{
    if(argc!=3)
    {
        printf("inputfiles error\n");
        printf("usage:./a.out\n");
        return -1;
    }

    int len =get_file_size(argv[1],argv[2]);

    pid_t pid=fork();
    if(pid>0)
    {
        copyfile(argv[1],argv[2],0,len/2);

        wait(NULL);
    }
    else if(pid==0)
    {
        copyfile(argv[1],argv[2],len/2,len-len/2);
        exit(EXIT_SUCCESS);
    }
    else
    {
        perror("fork error");
        return -1;
    }


    return 0;
}
