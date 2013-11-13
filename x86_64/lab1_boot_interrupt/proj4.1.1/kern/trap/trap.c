#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <mmu.h>
#include <memlayout.h>
#include <console.h>
#include <trap.h>
#include <clock.h>
#include <assert.h>

#define TICK_NUM 100

static void print_ticks() {
    cprintf("%d ticks\n",TICK_NUM);
#ifdef DEBUG_GRADE
    cprintf("End of Test.\n");
    panic("EOT: kernel seems ok.");
#endif
}

/* *
 * Interrupt descriptor table:
 *
 * Must be built at run time because shifted function addresses can't
 * be represented in relocation records.
 * */
static struct gatedesc idt[256] = {{0}};

static struct pseudodesc idt_pd = {
    sizeof(idt) - 1, (uintptr_t)idt
};

/* idt_init - initialize IDT to each of the entry points in kern/trap/vectors.S */
void
idt_init(void) {
    extern uintptr_t __vectors[];
    int i;
    for (i = 0; i < sizeof(idt) / sizeof(struct gatedesc); i ++) {
        SETGATE(idt[i], 0, GD_KTEXT, __vectors[i], DPL_KERNEL);
    }

    // set for switch from user to kernel
    SETGATE(idt[T_SWITCH_TOK], 0, GD_KTEXT, __vectors[T_SWITCH_TOK], DPL_USER);

    // load the IDT
    lidt(&idt_pd);
}

static const char *
trapname(int trapno) {
    static const char * const excnames[] = {
        "Divide error",
        "Debug",
        "Non-Maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "BOUND Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection",
        "Page Fault",
        "(unknown trap)",
        "x87 FPU Floating-Point Error",
        "Alignment Check",
        "Machine-Check",
        "SIMD Floating-Point Exception"
    };

    if (trapno < sizeof(excnames)/sizeof(const char * const)) {
        return excnames[trapno];
    }
    if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16) {
        return "Hardware Interrupt";
    }
    return "(unknown trap)";
}

/* trap_in_kernel - test if trap happened in kernel */
bool
trap_in_kernel(struct trapframe *tf) {
    return (tf->tf_cs == (uint16_t)KERNEL_CS);
}

static const char *IA32flags[] = {
    "CF", NULL, "PF", NULL, "AF", NULL, "ZF", "SF",
    "TF", "IF", "DF", "OF", NULL, NULL, "NT", NULL,
    "RF", "VM", "AC", "VIF", "VIP", "ID", NULL, NULL,
};

void
print_trapframe(struct trapframe *tf) {
    cprintf("trapframe at %p\n", tf);
    print_regs(&tf->tf_regs);
    cprintf("  ds   0x------------%04x\n", tf->tf_ds);
    cprintf("  es   0x------------%04x\n", tf->tf_es);
    cprintf("  fs   0x------------%04x\n", tf->tf_fs);
    cprintf("  gs   0x------------%04x\n", tf->tf_gs);
    cprintf("  trap 0x--------%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
    cprintf("  err  0x%016llx\n", tf->tf_err);
    cprintf("  rip  0x%016llx\n", tf->tf_rip);
    cprintf("  cs   0x------------%04x\n", tf->tf_cs);
    cprintf("  flag 0x%016llx\n", tf->tf_rflags);
    cprintf("  rsp  0x%016llx\n", tf->tf_rsp);
    cprintf("  ss   0x------------%04x\n", tf->tf_ss);

    int i, j;
    for (i = 0, j = 1; i < sizeof(IA32flags) / sizeof(IA32flags[0]); i ++, j <<= 1) {
        if ((tf->tf_rflags & j) && IA32flags[i] != NULL) {
            cprintf("%s,", IA32flags[i]);
        }
    }
    cprintf("IOPL=%d\n", (tf->tf_rflags & FL_IOPL_MASK) >> 12);
}

void
print_regs(struct pushregs *regs) {
    cprintf("  rdi  0x%016llx\n", regs->reg_rdi);
    cprintf("  rsi  0x%016llx\n", regs->reg_rsi);
    cprintf("  rdx  0x%016llx\n", regs->reg_rdx);
    cprintf("  rcx  0x%016llx\n", regs->reg_rcx);
    cprintf("  rax  0x%016llx\n", regs->reg_rax);
    cprintf("  r8   0x%016llx\n", regs->reg_r8);
    cprintf("  r9   0x%016llx\n", regs->reg_r9);
    cprintf("  r10  0x%016llx\n", regs->reg_r10);
    cprintf("  r11  0x%016llx\n", regs->reg_r11);
    cprintf("  rbx  0x%016llx\n", regs->reg_rbx);
    cprintf("  rbp  0x%016llx\n", regs->reg_rbp);
    cprintf("  r12  0x%016llx\n", regs->reg_r12);
    cprintf("  r13  0x%016llx\n", regs->reg_r13);
    cprintf("  r14  0x%016llx\n", regs->reg_r14);
    cprintf("  r15  0x%016llx\n", regs->reg_r15);
}

/* trap_dispatch - dispatch based on what type of trap occurred */
static void
trap_dispatch(struct trapframe *tf) {
    char c;

    switch (tf->tf_trapno) {
    case IRQ_OFFSET + IRQ_TIMER:
        ticks ++;
        if (ticks % TICK_NUM == 0) {
            print_ticks();
        }
        break;
    case IRQ_OFFSET + IRQ_COM1:
        c = cons_getc();
        cprintf("serial [%03d] %c\n", c, c);
        break;
    case IRQ_OFFSET + IRQ_KBD:
        c = cons_getc();
        cprintf("kbd [%03d] %c\n", c, c);
        break;
    case T_SWITCH_TOU:
        if (tf->tf_cs != USER_CS) {
            tf->tf_cs = USER_CS;
            tf->tf_ds = tf->tf_es = USER_DS;
            tf->tf_ss = USER_DS;
            // set eflags, make sure ucore can use io under user mode.
            // if CPL > IOPL, then cpu will generate a general protection.
            tf->tf_rflags |= FL_IOPL_MASK;
        }
        break;
    case T_SWITCH_TOK:
        if (tf->tf_cs != KERNEL_CS) {
            tf->tf_cs = KERNEL_CS;
            tf->tf_ds = tf->tf_es = KERNEL_DS;
            tf->tf_ss = KERNEL_DS;
            tf->tf_rflags &= ~FL_IOPL_MASK;
        }
        break;
    default:
        // in kernel, it must be a mistake
        if ((tf->tf_cs & 3) == 0) {
            print_trapframe(tf);
            panic("unexpected trap in kernel.\n");
        }
    }
}

/* *
 * trap - handles or dispatches an exception/interrupt. if and when trap() returns,
 * the code in kern/trap/trapentry.S restores the old CPU state saved in the
 * trapframe and then uses the iret instruction to return from the exception.
 * */
void
trap(struct trapframe *tf) {
    trap_dispatch(tf);
}

