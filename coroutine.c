#include "coroutine.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void *co_switch(struct co_cpu_ctx *nw_ctx, struct co_cpu_ctx *cur_ctx)
{
    __asm__(
        "movl 8(%esp), %edx      # fs->%edx         \n"
        "movl %esp, 0(%edx)      # save esp         \n"
        "movl %ebp, 4(%edx)      # save ebp         \n"
        "movl (%esp), %eax       # save eip         \n"
        "movl %eax, 8(%edx)                         \n"
        "movl %ebx, 12(%edx)     # save ebx,esi,edi \n"
        "movl %esi, 16(%edx)                        \n"
        "movl %edi, 20(%edx)                        \n"
        "movl 4(%esp), %edx      # ts->%edx         \n"
        "movl 20(%edx), %edi     # restore ebx,esi,edi      \n"
        "movl 16(%edx), %esi                                \n"
        "movl 12(%edx), %ebx                                \n"
        "movl 0(%edx), %esp      # restore esp              \n"
        "movl 4(%edx), %ebp      # restore ebp              \n"
        "movl 8(%edx), %eax      # restore eip              \n"
        "movl %eax, (%esp)                                  \n"
        "ret                                                \n");
}

struct scheduler *co_scheduler_create()
{
    struct scheduler *sche = (struct scheduler *)malloc(sizeof(struct scheduler));
    sche->main_ctx = (struct co_cpu_ctx *)malloc(sizeof(struct co_cpu_ctx));
    return sche;
}

struct coroutine *co_create(void (*func)(void *), void *data, struct scheduler *sche)
{
    struct coroutine *co = (struct coroutine *)malloc(sizeof(struct coroutine));
    co->func = func;
    co->data = data;
    co->sche = sche;
    co->status = READY;

    co->co_ctx = (struct co_cpu_ctx *)malloc(sizeof(struct co_cpu_ctx));
    co->stack_size = COSTACKSIZE;
    co->real_stack_size = COSTACKSIZE;
    co->stack = (char *)malloc(COSTACKSIZE);

    return co;
}

void co_free(struct coroutine *co)
{
    if (co == 0)
    {
        return;
    }
    free(co->stack);
    free(co->co_ctx);
}

//初始化coroutine
void co_ctx_make(struct coroutine *co)
{
    //移动到栈顶
    char *sp = (co->stack + co->stack_size) - sizeof(struct coroutine *);
    //对齐字节
    sp = (char *)((unsigned long)sp & -16L);
    /*
    下面这段代码，相当于下两行代码（struct ……）
    //将sp化成二级指针，每一个[]都指向一个一级指针地址
    char** temp=(char**)sp;
    //将前四位赋值
    temp[0]=co;
    */

    struct co_ctx_param *pa = (struct co_ctx_param *)sp;
    pa->param1 = (void *)co;

    //将寄存器信息初始化为0
    memset(co->co_ctx, 0, sizeof(struct co_cpu_ctx));

    //保留函数返回地址的空间
    //还原成正常函数调用的栈空间
    //ESP指针sp向下偏移2,因为除了ebp还有一个返回地址(eip)
    //该语句非常重要，一旦esp设置错误将导致参数传递失败
    co->co_ctx->esp = (char *)(sp) - sizeof(void *) * 2;

    //设置协程主函数到eip
    co->co_ctx->eip = (void *)co_main_func;
}

void co_resume(struct coroutine *co)
{
    if (co->status == READY)
    {
        co_ctx_make(co);
    }
    co_restoreStack(co);
    void *temp = co_switch(co->co_ctx, co->sche->main_ctx);

    printf("resume=%p\n", temp);
}

void co_yield(struct coroutine *co)
{
    co_saveStack(co, co->sche->stack + SHARESTACKSIZE);
    co_switch(co->sche->main_ctx, co->co_ctx);
}

void co_saveStack(struct coroutine *co, char *top)
{

    // int temp1=100;
    // int temo2=122;
    // int temp3=10022;
    char temp = 0;
    co->real_stack_size = (char *)co->co_ctx->esp - &temp;
    if (co->real_stack_size > co->stack_size)
    {
        free(co->stack);
        co->stack_size = co->real_stack_size;
        co->stack = (char *)malloc(co->real_stack_size);
    }
    // printf("%p\n", &temp);
    // printf("%p\n", co->sche->stack);
    // printf("%p\n", co->sche->stack + SHARESTACKSIZE);
    // printf("%p\n", co->co_ctx->esp);
    // printf("%p\n", co->sche->stack + SHARESTACKSIZE - (char *)co->co_ctx->esp);
    // printf("%d\n", (char *)co->co_ctx->esp - &temp);
    memcpy(co->stack, &temp, co->real_stack_size);
}

void co_restoreStack(struct coroutine *co)
{

    memcpy(co->sche->stack + SHARESTACKSIZE - co->stack_size, co->stack, co->stack_size);
}

static int co_main_func(void *arg1)
{
    for (;;)
    {
        struct coroutine *co = (struct coroutine *)arg1;
        int temp = 21024;
        int temp2 = 1111;
        printf("%d\n", temp);
        printf("%d\n", temp2);
        printf("args=%d\n", co->data);
        co->status = RUNNING;
        printf("%d\n", temp);
        printf("%d\n", temp2);
        co->func(co->data);
        co->status = SUSPEND;
        co_yield(co);
    }
}
