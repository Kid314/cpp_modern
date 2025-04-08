#include<stdio.h>  //标准缓冲输入输出
#include<string.h>
#include<iostream>

class stu
{
public:
    char name[20];
    int age;
    double score;
};

int main(int args,const char* argv[])
{
    FILE *fp=NULL;
    fp=fopen("./file.txt","w+");
    if(fp==NULL)
    {
        //printf("hello error: %d,%s\n",errno,strerror(errno));
        perror("fopen error");
        return -1;
    }
    printf("success\n");
    stu s[3]={{"张三",17,98},\
             {"李四",18,98},\
             {"kid",19,100},};

    fwrite(s,sizeof(stu),3,fp);

    printf("大小:%ld\n",ftell(fp));

    //fseek(fp,sizeof(stu),SEEK_SET);
    fseek(fp,-2*sizeof(stu),SEEK_CUR);

    stu temp;
    fread(&temp,sizeof(stu),1,fp);
    printf("name:%s,age:%d,score:%.2lf\n",temp.name,temp.age,temp.score);
    // fread(&temp,sizeof(stu),1,fp);
    // printf("name:%s,age:%d,score:%.2lf\n",temp.name,temp.age,temp.score);
    // fread(&temp,sizeof(stu),1,fp);
    // printf("name:%s,age:%d,score:%.2lf\n",temp.name,temp.age,temp.score);




    fclose(fp);

    return 0;
}