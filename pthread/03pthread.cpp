#include<myhead.h>
pthread_cond_t cond;
pthread_mutex_t mutex;
void* task1(void* arg)
{
    // int num=3;
    // while(num--)
    // {
    //     sleep(1);
    //     printf("+1,pthread=%#lx\n",pthread_self());
    //     pthread_cond_signal(&cond);
    // }
    sleep(3);
    pthread_cond_broadcast(&cond);
    pthread_exit(NULL);
}
void* task2(void* arg)
{
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond,&mutex);
    printf("-1pthread=%#lx\n",pthread_self());
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_t tid1,tid2,tid3,tid4;
    if(pthread_create(&tid1,NULL,task1,NULL)!=0)
    {
        printf("tid1 create error\n");
    }
    if(pthread_create(&tid2,NULL,task2,NULL)!=0)
    {
        printf("tid2 create error\n");
    }
    if(pthread_create(&tid3,NULL,task2,NULL)!=0)
    {
        printf("tid3 create error\n");
    }
    if(pthread_create(&tid4,NULL,task2,NULL)!=0)
    {
        printf("tid4 create error\n");
    }
    printf("tid1=%#lx,tid2=%#lx,tid3=%#lx,tid4=%#lx\n",tid1,tid2,tid3,tid4);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    pthread_join(tid4,NULL);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}
