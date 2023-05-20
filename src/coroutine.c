#include "coroutine.h"
// 协程的创建
int coroutine_create() { return 0; }

// 调度器运行
void schedule_run() {
    // 获取调度器
    schedule_t *sched = get_sched();
    if (sched == NULL) return;
}

schedule_t *get_sched() {}