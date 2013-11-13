#include <defs.h>
#include <x86.h>
#include <mmu.h>
#include <memlayout.h>
#include <pmm.h>

/* *
 * Task State Segment:
 *
 * The TSS may reside anywhere in memory. A special segment register called
 * the Task Register (TR) holds a segment selector that points a valid TSS
 * segment descriptor which resides in the GDT. Therefore, to use a TSS
 * the following must be done in function gdt_init:
 *   - create a TSS descriptor entry in GDT
 *   - add enough information to the TSS in memory as needed
 *   - load the TR register with a segment selector for that segment
 *
 * There are several fileds in TSS for specifying the new stack pointer when a
 * privilege level change happens. But only the fields SS0 and RSP0 are useful
 * in our os kernel.
 *
 * The field SS0 contains the stack segment selector for CPL = 0, and the RSP0
 * contains the new RSP value for CPL = 0. When an interrupt happens in protected
 * mode, the x86-64 CPU will look in the TSS for SS0 and RSP0 and load their value
 * into SS and RSP respectively.
 * */
static struct taskstate ts = {0};

/* *
 * Global Descriptor Table:
 *
 * The kernel and user segments are identical (except for the DPL). To load
 * the %ss register, the CPL must equal the DPL. Thus, we must duplicate the
 * segments for the user and the kernel. Defined as follows:
 *   - 0x0 :  unused (always faults -- for trapping NULL far pointers)
 *   - 0x10:  kernel code segment
 *   - 0x20:  kernel data segment
 *   - 0x30:  user code segment
 *   - 0x40:  user data segment
 *   - 0x50:  defined for tss, initialized in gdt_init
 * */
static struct segdesc gdt[] = {
    SEG_NULL,
    [SEG_KTEXT] = SEG(STA_X | STA_R, DPL_KERNEL),
    [SEG_KDATA] = SEG(STA_W, DPL_KERNEL),
    [SEG_UTEXT] = SEG(STA_X | STA_R, DPL_USER),
    [SEG_UDATA] = SEG(STA_W, DPL_USER),
    [SEG_TSS]   = SEG_NULL,
};

static struct pseudodesc gdt_pd = {
    sizeof(gdt) - 1, (uintptr_t)gdt
};

/* *
 * lgdt - load the global descriptor table register and reset the
 * data/code segement registers for kernel.
 * */
static inline void
lgdt(struct pseudodesc *pd) {
    asm volatile ("lgdt (%0)" :: "r" (pd));
    asm volatile ("movw %%ax, %%fs" :: "a" (0));
    asm volatile ("movw %%ax, %%gs" :: "a" (0));
    asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
    asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
    // reload cs & ss
    asm volatile (
        "movq %%rsp, %%rax;"            // move %rsp to %rax
        "pushq %1;"                     // push %ss
        "pushq %%rax;"                  // push %rsp
        "pushfq;"                       // push %rflags
        "pushq %0;"                     // push %cs
        "call 1f;"                      // push %rip
        "jmp 2f;"
        "1: iretq; 2:"
        :: "i" (KERNEL_CS), "i" (KERNEL_DS));
}

/* temporary kernel stack */
uint8_t stack0[1024];

/* gdt_init - initialize the default GDT and TSS */
static void
gdt_init(void) {
    // Setup a TSS so that we can get the right stack when we trap from
    // user to the kernel. But not safe here, it's only a temporary value,
    // it will be set to KSTACKTOP in lab2.
    ts.ts_rsp0 = (uintptr_t)stack0 + sizeof(stack0);

    // initialize the TSS filed of the gdt
    gdt[SEG_TSS] = SEGTSS(STS_T32A, (uintptr_t)&ts, sizeof(ts), DPL_KERNEL);

    // reload all segment registers
    lgdt(&gdt_pd);

    // load the TSS
    ltr(GD_TSS);
}

/* pmm_init - initialize the physical memory management */
//pmm_init - setup a pmm to manage physical memory, build PDT&PT to setup paging mechanism 
//         - check the correctness of pmm & paging mechanism, print PDT&PT
void
pmm_init(void) {
    gdt_init();
}

