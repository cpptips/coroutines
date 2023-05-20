#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 协程上下文
// 一共是九个寄存器
struct stack_ctx {
    unsigned long rax;
    unsigned long rbx;
    unsigned long rcx;
    unsigned long rdx;
    unsigned long rdi;
    unsigned long rsi;
    unsigned long rsp;
    unsigned long rbp;
    unsigned long rip;
};
/* 0. 常见的寄存器
寄存器      | 16位   |   32位    | 64位 |
累加add     | ax     |   eax     | rax
基址base    | bx     |   ebx     | rbx
计数count   | cx     |   ecx     | rcx
数据data    | dx     |   edx     | rdx
目的变址寄存器| di     |   edi     | rdi
堆栈基指针  | bp     |   ebp     | rbp  | 存放栈底指针
源变址寄存器| si     |   esi     | rsi  |
堆栈顶指针   | sp     |   esp     | rsp | 存放栈顶指针
指令        | ip     |   eip     | rip | 存放下一条指令
而这些就是协程运行的上下文。

ps：32位e可以理解位extend之类的意思，64位猜测应该是register。
参考:
https://softwareengineering.stackexchange.com/questions/127668/what-does-the-r-in-x64-register-names-stand-for
*/
// 1. mov指令
// movb 操作8位寄存器
// movw 操作16位寄存器
// movl 操作32位寄存器
// movq 操作64位寄存器，这里我们使用的是64位寄存器，q Quad word，4的意思
void set_jmp(struct stack_ctx* ctx) {
    //%rdi 用作函数第一个参数，即ctx
    //所以这个函数的作用就是，把寄存器组中的值保存到ctx指向的内存中
    asm volatile(
        "movq %%rax,0(%%rdi)\n\t"
        "movq %%rbx,8(%%rdi)\n\t"
        "movq %%rcx,16(%%rdi)\n\t"
        "movq %%rdx,24(%%rdi)\n\t"
        "movq %%rdi,32(%%rdi)\n\t"
        "movq %%rsi,40(%%rdi)\n\t"
        "movq %%rbp,%%rbx\n\t"
        "add $16,%%rbx\n\t"
        "movq %%rbx,48(%%rdi)\n\t"
        "movq 0(%%rbp),%%rbx\n\t"
        "movq %%rbx,56(%%rdi)\n\t"
        "movq 8(%%rbp),%%rbx\n\t"
        "movq %%rbx,64(%%rdi)\n\t"
        :
        :);
}
// 为什么要用两个%，不是一个%?
// 内联汇编分两种，一种使用是基本格式
// 另一种使用带有C/C++表达式格式，在其最后会有冒号(:)存在
// 我们在这里使用的是后者
// https:// blog.csdn.net/weixin_36071439/article/details/111967993
// https://zhuanlan.zhihu.com/p/25133012
void long_jmp(struct stack_ctx* ctx) {
    //%rdi 用作函数第一个参数，即ctx
    //所以这个函数的作用就是私用ctx所指向的内存数据恢复寄存器组
    asm volatile(
        "movq 0(%%rdi), %%rax\n\t"
        "movq 16(%%rdi), %%rcx\n\t"
        "movq 24(%%rdi), %%rdx\n\t"
        "movq 48(%%rdi), %%rsp\n\t"
        "movq 56(%%rdi), %%rbp\n\t"
        "movq 64(%%rdi), %%rbx\n\t"
        "pushq %%rbx\n\t"
        "movq 8(%%rdi), %%rbx\n\t"
        "movq 32(%%rdi), %%rdi\n\t"
        "movq 40(%%rdi), %%rsi\n\t"
        "ret\n\t"
        :
        :);
}

struct stack_ctx ctx = {0};
jmp_buf env;

void loop() {
    printf("---%s---\n", __func__);
    long_jmp(&ctx);
}
int g_count = 0;
int main(int argc, char const* argv[]) {
    set_jmp(env);
    if (++g_count > 3) {
        return 0;
    }
    loop();
    return 0;
}
