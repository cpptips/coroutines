#include "coroutine.h"

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
pthread_key_t global_sched_key;
static pthread_once_t sched_key_once = PTHREAD_ONCE_INIT;

static void sched_key_destructor(void *data) { free(data); }
static void coroutine_sched_key_creator() {
    assert(pthread_key_create(&global_sched_key, sched_key_destructor) == 0);
    // 线程局部数据存储
    assert(pthread_setspecific(global_sched_key, NULL) == 0);
}

// 协程的创建
int coroutine_create(coroutine_t **new_co, func_t func, void *arg) {
    // 在多线程环境中只执行一次
    assert(pthread_once(&sched_key_once, coroutine_sched_key_creator) == 0);

    schedule_t *sched = get_sched();
    if (sched == NULL) {
        schedule_create(0);  // 创建一个调度器
        sched = get_sched();
        if (sched == NULL) {
            printf("failed get sched!\n");
            return -1;
        }
    }

    // 协程申请内存
    coroutine_t *co = calloc(1, sizeof(coroutine_t));
    co->stack = NULL;
    co->stack_size = 0;
    co->sched = sched;
    co->status = 0;  // ready
    co->func = func;
    co->id = sched->corouties++;
    co->fd = -1;
    co->events = 0;

    *new_co = co;
    return 0;
}

// 调度器运行
void schedule_run() {
    // 获取调度器
    schedule_t *sched = get_sched();
    if (sched == NULL) return;

    while (1) {
        // 休眠队列

        // 就绪队列

        // 等待队列
    }
}

schedule_t *get_sched() { return pthread_getspecific(global_sched_key); }

int schedule_create(int stack_size) {
    schedule_t *sched =
        (schedule_t *)calloc(1, sizeof(schedule_t));  // 调度器分配内存
    assert(pthread_setspecific(global_sched_key, sched) == 0);

    // epoll来管理事件
    sched->poller_fd = epoll_create(1024);
    // 专门用于事件通知的文件描述符 eventfd, 将其挂在epoll_fd上,
    // todo: 这个是做什么用的呢？可能是计数使用
    if (!sched->eventfd) {
        sched->eventfd = eventfd(0, EFD_NONBLOCK);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sched->eventfd;
    int ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_ADD, sched->eventfd, &ev);

    // 注册触发器
    // 栈大小，默认64K
    sched->stack_size = stack_size ? stack_size : (64 * 1024);
    sched->page_size = getpagesize();
    posix_memalign(&sched->stack, sched->page_size,
                   sched->stack_size);  // 页内存申请
    return 0;
}