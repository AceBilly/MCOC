#include <stdio.h>
#include "coroutine.h"
#include <windows.h>
//int _add(int left,int right)
//{
//    __asm__(
//    "movl 4(%esp),%eax \n"
//    "movl 8(%esp),%ebx \n"
//    "addl %ebx,%eax \n"
//    "ret \n"
//    );
//};

int add(int left, int right);
__asm__(
    ".globl _add \n"
    ".text \n"
    "_add: \n"
    "movl 4(%esp),%eax \n"
    "movl 8(%esp),%edx \n"
    "addl %edx,%eax \n"
    "ret \n");

void run(void *arg, struct coroutine *co)
{

    printf("run1=%d\n", arg);
    co_yield(co);
}
void run2(void *arg, struct coroutine *co)
{
    char buf[1024];
    printf("run2=%d\n", arg);
    co_yield(co);
}

int main(void)
{
    int num1 = 3;
    int num2 = 5;

    struct scheduler *sche = co_scheduler_create(1);
    for (size_t i = 0; i < 1000; i++)
    {
        struct coroutine *co2 = co_create(run2, (void *)i, sche);
        struct coroutine *co1 = co_create(run, (void *)i, sche);
        co_resume(co2);
        co_resume(co1);
        //co_free(co1);
        //co_free(co2);
    }

    printf("finished\n");
    system("pause");
    return 0;
}
