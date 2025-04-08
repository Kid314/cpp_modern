#include<myhead.h>
#include<thread>

void task()
{
    printf("hello\n");
    while(1);
}
int main(int argc, char const *argv[])
{
    std::thread th(task);
    th.join();
    while(1);
    return 0;
}
