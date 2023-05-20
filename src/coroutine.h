#if !defined(__COROUTINE_H__)
#define __COROUTINE_H__
#include <stdlib.h>
#include <ucontext.h>
// 协程回调函数类型
typedef void (*func_t)(void*);

// 核心数据结构： 调度器
typedef struct schedule_t {
    /* data */
} schedule_t;

// 核心数据结构： 协程
typedef struct coroutine_t {
    /* data */
    ucontext_t ctx;  // 协程上下文
    func_t func;     // 回调函数
    void* arg;       // 回调函数参数
    int status;      // 运行状态
} coroutine_t;

// 协程的创建
int coroutine_create();

// 获取调度器
schedule_t* get_sched();
// 调度器运行
void schedule_run();

#endif  // __COROUTINE_H__
