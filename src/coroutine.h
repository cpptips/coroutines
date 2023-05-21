#if !defined(__COROUTINE_H__)
#define __COROUTINE_H__
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <ucontext.h>

#include "queue.h"
// 协程回调函数类型
typedef void (*func_t)(void*);

// 核心数据结构： 调度器
typedef struct schedule_t {
    /* data */
    ucontext_t ctx;
    void* stack;
    size_t stack_size;
    int page_size;

    // epoll
    int poller_fd;

    int corouties;
} schedule_t;

// 核心数据结构： 协程
typedef struct coroutine_t {
    /* data */
    ucontext_t ctx;  // 协程上下文
    func_t func;     // 回调函数
    void* arg;       // 回调函数参数
    int status;      // 运行状态

    u_int64_t id;
    int fd;
    int events;

    void* stack;
    size_t stack_size;
    schedule_t* sched;

} coroutine_t;

// 协程的创建
int coroutine_create(coroutine_t** co, func_t func, void* arg);

// 获取调度器
schedule_t* get_sched();
// 创建调度器
int schedule_create(int stack_size);
// 调度器运行
void schedule_run();

#endif  // __COROUTINE_H__
