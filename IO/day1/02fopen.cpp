#include<stdio.h>  //标准缓冲输入输出

int main(int args,const char* argv[])
{
    FILE *fp=NULL;
    fp=fopen("./file.txt","w+");
    if(fp==NULL)
    {
        printf("hello error\n");
        return -1;
    }
    printf("success\n");
    fclose(fp);
    return 0;
}
