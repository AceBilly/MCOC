#include <stdio.h>
#include "coroutine.h"
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

void run(void*arg)
{
    char buf[1024*100];
    printf("run1=%d\n",arg);
}

int main(void)
{
    int num1=3;
    struct scheduler* sche=co_scheduler_create();
    struct coroutine *co1=co_create(run,(void*)num1,sche);
    co_resume(co1);
    co_resume(co1);
    printf("finished\n");
    co_resume(co1);
    return 0;
}



