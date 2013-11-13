#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>

int __noreturn
kern_init(void) {
    extern char edata[], end[];
    memset(edata, 0, end - edata);

    cons_init();                // init the console

    const char *message = "(THU.CST) os is loading ...";
    cprintf("[%d]%s\n", strlen(message), message);

    while (1);
}

