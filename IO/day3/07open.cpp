#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
int main(int argc, char const *argv[])
{
    int fd=-1;
    if((fd=open("./tt.txt",O_WRONLY|O_CREAT,0644))==-1)
    {
        perror("open error");
        return -1;
    }
    printf("open success fd=%d\n",fd);

    char wbuf[128] = "hello world";
    write(fd,wbuf,strlen(wbuf));
    close(fd);

    if((fd=open("./tt.txt",O_RDONLY))==-1)
    {
        perror("open error");
        return -1;
    }
    printf("open success fd=%d\n",fd);
    char rbuf[5]="";
    int res=read(fd,rbuf,sizeof(rbuf));

    write(1,rbuf,res);
    close(fd);

    return 0;
}
