#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<iostream>
#include<string.h>
#include<string>
int main(int argc, char const *argv[])
{
    int fd=-1;
    if((fd=open("./cc.bmp",O_RDWR))==-1)
    {
        perror("open file");
    }
    //printf("%ld\n",lseek(fd,0,SEEK_END));
    int picnum=0;
    lseek(fd,2,SEEK_SET);
    read(fd,&picnum,4);
    //std::cout<<picnum<<"\n";
    std::string str_picnum=std::to_string(picnum);
    // std::cout<<sizeof(str_picnum)<<std::endl;
    // std::cout<<str_picnum.size()<<std::endl;
    write(1,str_picnum.c_str(),str_picnum.size());

    lseek(fd,54,SEEK_SET);

    unsigned char color[3]={0,255,0};
    for(int i=0;i<100;i++)
    {
        for(int j=0;j<500;j++)
        {
            write(fd,color,sizeof(color));
        }
    }
    return 0;
}
