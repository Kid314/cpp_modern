#include<iostream>
#include<stdio.h>

int main(int argc,const char* argv[])
{
    printf("1:%d\n",stdin->_fileno);
    printf("2:%d\n",stdout->_fileno);
    printf("3:%d\n",stderr->_fileno);
    return 0;
}