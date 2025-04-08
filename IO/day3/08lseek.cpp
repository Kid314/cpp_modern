#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
int main(int argc, char const *argv[])
{
    int fd=-1;
    if((fd=open("./tt.txt",O_RDWR|O_CREAT|O_TRUNC,0644))==-1)
    {
        perror("open error");
        return -1;
    }
    printf("open success fd=%d\n",fd);

    char wbuf[128] = "hello world";
    write(fd,wbuf,strlen(wbuf));

    //lseek(fd,6,SEEK_SET);
    lseek(fd,-5,SEEK_CUR);

    char rbuf[5]="";
    int res=read(fd,rbuf,sizeof(rbuf));

    write(1,rbuf,res);
    close(fd);

    return 0;
}
