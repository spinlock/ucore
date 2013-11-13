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
grade_backtrace2(long arg0, long arg1, long arg2, long arg3) {
    mon_backtrace(0, NULL, NULL);
}

void __noinline
grade_backtrace1(long arg0, long arg1) {
    grade_backtrace2(arg0, (long)&arg0, arg1, (long)&arg1);
}

void __noinline
grade_backtrace0(long arg0, long arg1, long arg2) {
    grade_backtrace1(arg0, arg2);
}

void
grade_backtrace(void) {
#ifdef DEBUG_GRADE
    grade_backtrace0(0, (long)kern_init, 0xffff0000);
#endif
}

