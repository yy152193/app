#include <stdio.h>

static int test_watch_point = 100;

int func1(int arg)
{
    printf("this is func 1. arg:%d\n", arg);
    test_watch_point = 200;


    return arg;
}

int func2(int arg)
{
    printf("this is func2. arg:%d\n", arg);
    int ret = func1(arg+1);
    return ret;
}

int main(int argc, char *argv[])
{
    printf("this is main. argc:%d, argv[0]:%s\n", argc, argv[0]);
    
    int ret = func2(argc);
    printf("ret is :%d\n", ret);
    test_watch_point = 300;
    return 0;
}
