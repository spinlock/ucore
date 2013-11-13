#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <monitor.h>
#include <assert.h>

static __noinline void grade_backtrace(void);

int __noreturn
kern_init(void) {
    extern char edata[], end[];
    memset(edata, 0, end - edata);

    cons_init();                // init the console

    const char *message = "(THU.CST) os is loading ...";
    cprintf("%s\n\n", message);

    // drop into kernel monitor
    grade_backtrace();
    while (1) {
        monitor(NULL);
    }
}

void __noinline
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) {
    mon_backtrace(0, NULL, NULL);
}

void __noinline
grade_backtrace1(int arg0, int arg1) {
    grade_backtrace2(arg0, (int)&arg0, arg1, (int)&arg1);
}

void __noinline
grade_backtrace0(int arg0, int arg1, int arg2) {
    grade_backtrace1(arg0, arg2);
}

void
grade_backtrace(void) {
#ifdef DEBUG_GRADE
    grade_backtrace0(0, (int)kern_init, 0xffff0000);
#endif
}

