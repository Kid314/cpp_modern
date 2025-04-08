#include<myhead.h>

pthread_mutex_t mutex;

int num=520;
void* task1(void* arg)
{
    while(1)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        num-=10;
        printf("tid1:num=%d\n",num);
        pthread_mutex_unlock(&mutex);

    }
}
void* task2(void* arg)
{
    while(2)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        num-=20;
        printf("tid2:num=%d\n",num);
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex,NULL);
    pthread_t tid1,tid2;
    if(pthread_create(&tid1,NULL,task1,NULL)!=0)
    {
        printf("tid1 create error\n");
    }
    if(pthread_create(&tid2,NULL,task2,NULL)!=0)
    {
        printf("tid2 create error\n");
    }
    printf("tid1=%#lx,tid2=%#lx\n",tid1,tid2);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}
