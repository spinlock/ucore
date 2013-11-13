.text
.globl kernel_thread_entry
kernel_thread_entry:        # void kernel_thread(void)

    movq %rdi, %rax         # mov fn to rax
    movq %rsi, %rdi         # prepare arg
    callq *%rax             # call fn

    movq %rax, %rdi         # save the return value of fn(arg)
    call do_exit            # call do_exit to terminate current thread

