#ifndef COROUTINE_H
#define COROUTINE_H

//128kb共享栈
#define SHARESTACKSIZE 128*1024
#define COSTACKSIZE 1024*2
#define READY 0
#define RUNNING 1
#define SUSPEND 2
//是否使用共享栈，1为使用 ，0为独立栈
static int isuShareStack=0;


struct co_cpu_ctx
{
    void *esp; //
	void *ebp;
	void *eip;
	void *edi;
	void *esi;
	void *ebx;
	void *r1;
	void *r2;
	void *r3;
	void *r4;
	void *r5;
};

struct scheduler
{
    struct co_cpu_ctx* main_ctx;
    char* share_stack;   
};

struct coroutine
{
    struct co_cpu_ctx *co_ctx;
    struct scheduler* sche;
    void (*func)(void *data,struct coroutine*co);
    void *data;
    char* stack;
    int stack_size;
    int status;
    
};

struct co_ctx_param
{
	const void *param1;
};

int co_ctx_make(struct coroutine*co);
void* co_switch(struct co_cpu_ctx *nw_ctx, struct co_cpu_ctx *cur_ctx);
void co_resume(struct coroutine* co);
void co_yield(struct coroutine* co);
void co_saveStack(struct coroutine *co);
struct coroutine *co_create(void (*func)(void *, struct coroutine *), void *data, struct scheduler *sche);
void co_free(struct coroutine *co);
static int co_main_func(struct coroutine *co);
struct scheduler *co_scheduler_create(int isUseShareStack);
int _switch(struct co_cpu_ctx *new_ctx,struct co_cpu_ctx *cur_ctx);


#endif
