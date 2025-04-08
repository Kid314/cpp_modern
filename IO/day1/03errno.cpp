#include<stdio.h>  //标准缓冲输入输出
#include<errno.h>
#include<string.h>

int main(int args,const char* argv[])
{
    FILE *fp=NULL;
    fp=fopen("./file.txt","r");
    if(fp==NULL)
    {
        //printf("hello error: %d,%s\n",errno,strerror(errno));
        perror("fopen error");
        return -1;
    }
    printf("success\n");
    fclose(fp);
    return 0;
}
