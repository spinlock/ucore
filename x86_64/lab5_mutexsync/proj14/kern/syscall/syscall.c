#include <proc.h>
#include <syscall.h>
#include <trap.h>
#include <stdio.h>
#include <pmm.h>
#include <clock.h>
#include <error.h>
#include <assert.h>

static uint64_t
sys_exit(uint64_t arg[]) {
    int error_code = (int)arg[0];
    return do_exit(error_code);
}

static uint64_t
sys_fork(uint64_t arg[]) {
    struct trapframe *tf = current->tf;
    uintptr_t stack = tf->tf_rsp;
    return do_fork(0, stack, tf);
}

static uint64_t
sys_wait(uint64_t arg[]) {
    int pid = (int)arg[0];
    int *store = (int *)arg[1];
    return do_wait(pid, store);
}

static uint64_t
sys_exec(uint64_t arg[]) {
    const char *name = (const char *)arg[0];
    size_t len = (size_t)arg[1];
    unsigned char *binary = (unsigned char *)arg[2];
    size_t size = (size_t)arg[3];
    return do_execve(name, len, binary, size);
}

static uint64_t
sys_clone(uint64_t arg[]) {
    struct trapframe *tf = current->tf;
    uint32_t clone_flags = (uint32_t)arg[0];
    uintptr_t stack = (uintptr_t)arg[1];
    if (stack == 0) {
        stack = tf->tf_rsp;
    }
    return do_fork(clone_flags, stack, tf);
}

static uint64_t
sys_exit_thread(uint64_t arg[]) {
    int error_code = (int)arg[0];
    return do_exit_thread(error_code);
}

static uint64_t
sys_yield(uint64_t arg[]) {
    return do_yield();
}

static uint64_t
sys_sleep(uint64_t arg[]) {
    unsigned int time = (unsigned int)arg[0];
    return do_sleep(time);
}

static uint64_t
sys_kill(uint64_t arg[]) {
    int pid = (int)arg[0];
    return do_kill(pid, -E_KILLED);
}

static uint64_t
sys_gettime(uint64_t arg[]) {
    return ticks;
}

static uint64_t
sys_getpid(uint64_t arg[]) {
    return current->pid;
}

static uint64_t
sys_brk(uint64_t arg[]) {
    uintptr_t *brk_store = (uintptr_t *)arg[0];
    return do_brk(brk_store);
}

static uint64_t
sys_mmap(uint64_t arg[]) {
    uintptr_t *addr_store = (uintptr_t *)arg[0];
    size_t len = (size_t)arg[1];
    uint32_t mmap_flags = (uint32_t)arg[2];
    return do_mmap(addr_store, len, mmap_flags);
}

static uint64_t
sys_munmap(uint64_t arg[]) {
    uintptr_t addr = (uintptr_t)arg[0];
    size_t len = (size_t)arg[1];
    return do_munmap(addr, len);
}

static uint64_t
sys_shmem(uint64_t arg[]) {
    uintptr_t *addr_store = (uintptr_t *)arg[0];
    size_t len = (size_t)arg[1];
    uint32_t mmap_flags = (uint32_t)arg[2];
    return do_shmem(addr_store, len, mmap_flags);
}

static uint64_t
sys_putc(uint64_t arg[]) {
    int c = (int)arg[0];
    cputchar(c);
    return 0;
}

static uint64_t
sys_pgdir(uint64_t arg[]) {
    print_pgdir();
    return 0;
}

static uint64_t (*syscalls[])(uint64_t arg[]) = {
    [SYS_exit]              sys_exit,
    [SYS_fork]              sys_fork,
    [SYS_wait]              sys_wait,
    [SYS_exec]              sys_exec,
    [SYS_clone]             sys_clone,
    [SYS_exit_thread]       sys_exit_thread,
    [SYS_yield]             sys_yield,
    [SYS_kill]              sys_kill,
    [SYS_sleep]             sys_sleep,
    [SYS_gettime]           sys_gettime,
    [SYS_getpid]            sys_getpid,
    [SYS_brk]               sys_brk,
    [SYS_mmap]              sys_mmap,
    [SYS_munmap]            sys_munmap,
    [SYS_shmem]             sys_shmem,
    [SYS_putc]              sys_putc,
    [SYS_pgdir]             sys_pgdir,
};

#define NUM_SYSCALLS        ((sizeof(syscalls)) / (sizeof(syscalls[0])))

void
syscall(void) {
    struct trapframe *tf = current->tf;
    uint64_t arg[6];
    int num = tf->tf_regs.reg_rax;
    if (num >= 0 && num < NUM_SYSCALLS) {
        if (syscalls[num] != NULL) {
            arg[0] = tf->tf_regs.reg_rdi;
            arg[1] = tf->tf_regs.reg_rsi;
            arg[2] = tf->tf_regs.reg_rdx;
            arg[3] = tf->tf_regs.reg_rcx;
            arg[4] = tf->tf_regs.reg_r8;
            arg[5] = tf->tf_regs.reg_r9;
            tf->tf_regs.reg_rax = syscalls[num](arg);
            return ;
        }
    }
    print_trapframe(tf);
    panic("undefined syscall %d, pid = %d, name = %s.\n",
            num, current->pid, current->name);
}

