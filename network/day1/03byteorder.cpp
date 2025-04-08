#include<myhead.h>

int main(int argc, char const *argv[])
{
    int num=0x12345678;

    int res=htonl(num);
    printf("res=%#x\n",res);

    return 0;
}
