#include<myhead.h>
int main(int argc, char const *argv[])
{
    const char* ip ="172.20.10.8";
    unsigned int ip_net=inet_addr(ip);
    printf("%#x\n",ip_net);
    return 0;
}
