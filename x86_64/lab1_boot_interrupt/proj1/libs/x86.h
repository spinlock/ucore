#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include <defs.h>

#define barrier() __asm__ __volatile__ ("" ::: "memory")

static __always_inline uint8_t
inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" : "=a" (data) : "d" (port) : "memory");
    return data;
}

static __always_inline void
outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" :: "a" (data), "d" (port) : "memory");
}

#endif /* !__LIBS_X86_H__ */

