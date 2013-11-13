#ifndef __KERN_MM_MEMLAYOUT_H__
#define __KERN_MM_MEMLAYOUT_H__

/* This file contains the definitions for memory management in our OS. */

/* global segment number */
#define SEG_KTEXT   1
#define SEG_KDATA   2
#define SEG_UTEXT   3
#define SEG_UDATA   4
#define SEG_TSS     5

/* global descrptor numbers */
#define GD_KTEXT    ((SEG_KTEXT) << 4)      // kernel text
#define GD_KDATA    ((SEG_KDATA) << 4)      // kernel data
#define GD_UTEXT    ((SEG_UTEXT) << 4)      // user text
#define GD_UDATA    ((SEG_UDATA) << 4)      // user data
#define GD_TSS      ((SEG_TSS) << 4)        // task segment selector

#define DPL_KERNEL  (0)
#define DPL_USER    (3)

#define KERNEL_CS   ((GD_KTEXT) | DPL_KERNEL)
#define KERNEL_DS   ((GD_KDATA) | DPL_KERNEL)
#define USER_CS     ((GD_UTEXT) | DPL_USER)
#define USER_DS     ((GD_UDATA) | DPL_USER)

/* *
 * Virtual memory map:                                          Permissions
 *                                                              kernel/user
 *
 *     4G x 4G -------------> +---------------------------------+
 *                            |                                 |
 *                            |         Empty Memory (*)        |
 *                            |                                 |
 *                            +---------------------------------+ 0xFFFFA00000000000
 *                            |   Cur. Page Table (Kern, RW)    | RW/-- PUSIZE
 *     VPT -----------------> +---------------------------------+ 0xFFFF9F8000000000
 *                            |        Invalid Memory (*)       | --/--
 *     KERNTOP -------------> +---------------------------------+ 0xFFFF900000000000
 *                            |                                 |
 *                            |    Remapped Physical Memory     | RW/-- KMEMSIZE
 *                            |                                 |
 *     KERNBASE ------------> +---------------------------------+ 0xFFFF800000000000
 *                            |                                 |
 *                            |                                 |
 *                            |                                 |
 *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.
 *
 * */

/* All physical memory mapped at this address */
#define KERNBASE            0xFFFF800000000000
#define KMEMSIZE            0x0000100000000000          // the maximum amount of physical memory
#define KERNTOP             (KERNBASE + KMEMSIZE)

/* *
 * Virtual page table. Entry PGX[VPT] in the PGD (Page Global Directory) contains
 * a pointer to the page directory itself, thereby turning the PGD into a page
 * table, which maps all the PUEs (Page Upper Directory Table Entry) containing
 * the page mappings for the entire virtual address space into that region starting at VPT.
 * */
#define VPT                 0xFFFF9F8000000000

#define KSTACKPAGE          4                           // # of pages in kernel stack
#define KSTACKSIZE          (KSTACKPAGE * PGSIZE)       // sizeof kernel stack

#endif /* !__KERN_MM_MEMLAYOUT_H__ */

