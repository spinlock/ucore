#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>

static void switch_test(void);

int __noreturn
kern_init(void) {
    extern char edata[], end[];
    memset(edata, 0, end - edata);

    cons_init();                // init the console

    const char *message = "(THU.CST) os is loading ...";
    cprintf("%s\n\n", message);

    print_kerninfo();

    pmm_init();                 // init physical memory management

    pic_init();                 // init interrupt controller
    idt_init();                 // init interrupt descriptor table

    clock_init();               // init clock interrupt
    intr_enable();              // enable irq interrupt

    // user/kernel mode switch test
    switch_test();

    /* do nothing */
    while (1);
}

static void
print_cur_status(void) {
    static int round = 0;
    uint16_t reg[6];
    asm volatile (
            "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%fs, %3;"
            "mov %%gs, %4;"
            "mov %%ss, %5;"
            : "=m" (reg[0]), "=m" (reg[1]), "=m" (reg[2]),
              "=m" (reg[3]), "=m" (reg[4]), "=m" (reg[5]));
    cprintf("%d: @ring %d\n", round, reg[0] & 3);
    cprintf("%d: cs = %02x, ds = %02x, es = %02x\n",
            round, reg[0], reg[1], reg[2]);
    cprintf("%d: fs = %02x, gs = %02x, ss = %02x\n",
            round, reg[3], reg[4], reg[5]);
    round ++;
}

static void
switch_to_user(void) {
    asm volatile ("int %0\n" :: "i" (T_SWITCH_TOU));
}

static void
switch_to_kernel(void) {
    asm volatile ("int %0\n" :: "i" (T_SWITCH_TOK));
}

static void
switch_test(void) {
    print_cur_status();
    cprintf("+++ switch to  user  mode +++\n");
    switch_to_user();
    print_cur_status();
    cprintf("+++ switch to kernel mode +++\n");
    switch_to_kernel();
    print_cur_status();
}

