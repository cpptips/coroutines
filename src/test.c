#include <stdio.h>

#include "coroutine.h"

void test_func(void *arg) { printf("hello world\n"); }
int main(int argc, char const *argv[]) {
    // 创建一个协程
    coroutine_t *co = NULL;
    coroutine_create(&co, test_func, NULL);
    // 调度器开始运行
    schedule_run();
    return 0;
}
