#include<myhead.h>
sem_t sem;

void* task1(void* arg)
{
    int num=5;
    while(num--)
    {
        sleep(1);
        printf("+1\n");
        sem_post(&sem);
    }
    pthread_exit(NULL);
}
void* task2(void* arg)
{
    int num=5;
    while(num--)
    {
        sem_wait(&sem);
        printf("-1\n");
    }
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{
    sem_init(&sem,0,0);
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

    sem_destroy(&sem);

    return 0;
}
