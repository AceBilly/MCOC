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

int add(int left,int right);
__asm__(
".globl _add \n"
".text \n"
"_add: \n"
"movl 4(%esp),%eax \n"
"movl 8(%esp),%edx \n"
"addl %edx,%eax \n"
"ret \n"
);

void run(void*arg,struct coroutine *co)
{

    
    for (int i = 0; i < 3; i++)
    {
        printf("run1=%d\n",arg);
    }
    
    
}
void run2(void* arg,struct coroutine*co)
{
    char buf[1024*100];
    for (int i = 0; i < 3; i++)
    {
        printf("run2=%d\n",arg);
    }
    co_yield(co);
    printf("run2=%d\n",arg);
}

int main(void)
{
    int num1=3;
    int num2=5;

    struct scheduler* sche=co_scheduler_create();
    struct coroutine *co2=co_create(run2,(void*)num2,sche);
    struct coroutine *co1=co_create(run,(void*)num1,sche);
    co_resume(co2);
    co_resume(co2);
    co_resume(co1);
    printf("finished\n");
    system("pause");
    return 0;
}



