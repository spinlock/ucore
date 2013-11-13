PROJ	:= 10.1
EMPTY	:=
SPACE	:= $(EMPTY) $(EMPTY)
SLASH	:= /

V       := @

# try to infer the correct GCCPREFX
ifndef GCCPREFIX
GCCPREFIX := $(shell if x86_64-ucore-elf-objdump -i 2>&1 | grep '^elf64-x86-64$$' >/dev/null 2>&1; \
	then echo 'x86_64-ucore-elf-'; \
	elif objdump -i 2>&1 | grep 'elf64-x86-64' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an x86_64-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with x86_64-ucore-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your x86_64-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'x86_64-ucore-elf-', set your GCCPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake GCCPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# try to infer the correct QEMU
ifndef QEMU
QEMU := $(shell if which qemu-system-x86_64 > /dev/null; \
	then echo 'qemu-system-x86_64'; exit; \
	else \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU (qemu-system-x86_64) executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# try to find corret gdb
ifndef GDB
GDB := $(shell if which x86_64-ucore-elf-gdb > /dev/null; \
	then echo 'x86_64-ucore-elf-gdb'; exit; \
	else \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working GDB (x86_64-ucore-elf-gdb) executable." 1>&2; \
	echo "*** Is the directory containing the gdb binary in your PATH" 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# eliminate default suffix rules
.SUFFIXES: .c .S .h

# delete target files if there is an error (or make is interrupted)
.DELETE_ON_ERROR:

# define compiler and flags

HOSTCC		:= gcc
HOSTCFLAGS	:= -g -Wall -O2

CC		:= $(GCCPREFIX)gcc
CFLAGS	:= -fno-builtin -Wall -ggdb -nostdinc $(DEFS)
CFLAGS	+= $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CC32	:= -m32
CC64	:= -m64 -mcmodel=large -fno-omit-frame-pointer -mno-red-zone -mno-sse -mno-sse2 -mno-sse3 -mno-mmx
CTYPE	:= c S

LD      := $(GCCPREFIX)ld
LDFLAGS	:= -nostdlib
LD32	:= -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)
LD64	:= -m $(shell $(LD) -V | grep elf_x86_64 2>/dev/null)

OBJCOPY := $(GCCPREFIX)objcopy
OBJDUMP := $(GCCPREFIX)objdump

COPY	:= cp
MKDIR   := mkdir -p
MV		:= mv
RM		:= rm -f
AWK		:= awk
SED		:= sed
SH		:= sh
TR		:= tr
TOUCH	:= touch -c

TAR		:= tar
ZIP		:= gzip

OBJDIR	:= obj
BINDIR	:= bin

ALLOBJS	:=
ALLDEPS	:=
TARGETS	:=

USER_PREFIX	:= __user_

include tools/function.mk

listf_cc = $(call listf,$(1),$(CTYPE))

# for cc
add_files_cc = $(call add_files,$(1),$(CC),$(CFLAGS) $(3),$(2),$(4))
create_target_cc = $(call create_target,$(1),$(2),$(3),$(CC),$(CFLAGS))

# for hostcc
add_files_host = $(call add_files,$(1),$(HOSTCC),$(HOSTCFLAGS),$(2),$(3))
create_target_host = $(call create_target,$(1),$(2),$(3),$(HOSTCC),$(HOSTCFLAGS))

cgtype = $(patsubst %.$(2),%.$(3),$(1))
objfile = $(call toobj,$(1))
asmfile = $(call cgtype,$(call toobj,$(1)),o,asm)
outfile = $(call cgtype,$(call toobj,$(1)),o,out)
symfile = $(call cgtype,$(call toobj,$(1)),o,sym)
filename = $(basename $(notdir $(1)))
ubinfile = $(call outfile,$(addprefix $(USER_PREFIX),$(call filename,$(1))))

# for match pattern
match = $(shell echo $(2) | $(AWK) '{for(i=1;i<=NF;i++){if(match("$(1)","^"$$(i)"$$")){exit 1;}}}'; echo $$?)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# include kernel/user

INCLUDE	+= libs/

CFLAGS	+= $(addprefix -I,$(INCLUDE))

LIBDIR	+= libs

$(call add_files_cc,$(call listf_cc,$(LIBDIR)),libs,$(CC64))

# -------------------------------------------------------------------
# user programs

UINCLUDE	+= user/include/ \
			   user/libs/

USRCDIR		+= user

ULIBDIR		+= user/libs

UCFLAGS		+= $(addprefix -I,$(UINCLUDE))
USER_BINS	:=

$(call add_files_cc,$(call listf_cc,$(ULIBDIR)),ulibs,$(UCFLAGS))
$(call add_files_cc,$(call listf_cc,$(USRCDIR)),uprog,$(UCFLAGS))

UOBJS	:= $(call read_packet,ulibs libs)

define uprog_ld
__user_bin__ := $$(call ubinfile,$(1))
USER_BINS += $$(__user_bin__)
$$(__user_bin__): tools/user.ld
$$(__user_bin__): $$(UOBJS)
$$(__user_bin__): $(1) | $$$$(dir $$$$@)
	$(V)$(LD) $(LDFLAGS) -T tools/user.ld -o $$@ $$(UOBJS) $(1)
	@$(OBJDUMP) -S $$@ > $$(call cgtype,$$<,o,asm)
	@$(OBJDUMP) -t $$@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$$$/d' > $$(call cgtype,$$<,o,sym)
endef

$(foreach p,$(call read_packet,uprog),$(eval $(call uprog_ld,$(p))))

# -------------------------------------------------------------------
# kernel

KINCLUDE	+= kern/debug/ \
			   kern/driver/ \
			   kern/mm/ \
			   kern/trap/ \
			   kern/libs/ \
			   kern/sync/ \
			   kern/fs/ \
			   kern/process/ \
			   kern/schedule/ \
			   kern/syscall

KSRCDIR		+= kern/init \
			   kern/libs \
			   kern/debug \
			   kern/driver \
			   kern/trap \
			   kern/mm \
			   kern/sync \
			   kern/fs \
			   kern/process \
			   kern/schedule \
			   kern/syscall

KCFLAGS		+= $(addprefix -I,$(KINCLUDE)) $(CC64)

$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))

KOBJS	= $(call read_packet,kernel libs)

# create kernel target
kernel	= $(call totarget,kernel)

$(kernel): tools/kernel.ld

$(kernel): $(KOBJS) $(USER_BINS)
	@echo + ld $@
	$(V)$(LD) $(LD64) $(LDFLAGS) -T tools/kernel.ld -z max-page-size=0x1000 -o $@ $(KOBJS) -b binary $(USER_BINS)
	@$(OBJDUMP) -S -D $@ > $(call asmfile,kernel)
	@$(OBJDUMP) -t $@ | $(SED) '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(call symfile,kernel)

$(call create_target,kernel)

# -------------------------------------------------------------------

# create bootblock
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) $(CC32) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,boot/bootasm.S) $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LD32) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S -D $(call objfile,bootblock) > $(call asmfile,bootblock)
	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

$(call create_target,bootblock)

# -------------------------------------------------------------------

# create 'sign' tools
$(call add_files_host,tools/sign.c,sign,sign)
$(call create_target_host,sign,sign)

# -------------------------------------------------------------------

# create ucore.img
UCOREIMG	:= $(call totarget,ucore.img)

$(UCOREIMG): $(kernel) $(bootblock)
	$(V)dd if=/dev/zero of=$@ count=10000
	$(V)dd if=$(bootblock) of=$@ conv=notrunc
	$(V)dd if=$(kernel) of=$@ seek=1 conv=notrunc

$(call create_target,ucore.img)

# -------------------------------------------------------------------

# create swap.img
SWAPIMG		:= $(call totarget,swap.img)

$(SWAPIMG):
	$(V)dd if=/dev/zero of=$@ bs=1M count=128

$(call create_target,swap.img)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

$(call finish_all)

IGNORE_ALLDEPS	= gdb \
				  clean \
				  distclean \
				  grade \
				  touch \
				  print-.+ \
				  run-.+ \
				  build-.+ \
				  handin

ifeq ($(call match,$(MAKECMDGOALS),$(IGNORE_ALLDEPS)),0)
-include $(ALLDEPS)
endif

# files for grade script

targets: $(TARGETS)

.DEFAULT_GOAL := targets

QEMUOPTS = -hda $(UCOREIMG) -drive file=$(SWAPIMG),media=disk,cache=writeback

.PHONY: qemu qemu-nox gdb debug debug-mon debug-nox
qemu: targets
	$(V)$(QEMU) -parallel stdio $(QEMUOPTS) -serial null

qemu-nox: targets
	$(V)$(QEMU) -serial mon:stdio $(QEMUOPTS) -nographic

gdb:
	$(V)$(GDB) -q -x tools/gdbinit

debug: targets
	$(V)$(QEMU) -S -s -parallel stdio $(QEMUOPTS) -serial null

debug-mon: targets
	$(V)$(QEMU) -S -s -monitor stdio $(QEMUOPTS) -parallel null -serial null

debug-nox: targets
	$(V)$(QEMU) -S -s -serial mon:stdio $(QEMUOPTS) -nographic

RUN_PREFIX	:= _binary_$(OBJDIR)_$(USER_PREFIX)
MAKEOPTS	:= --quiet --no-print-directory

run-%: build-%
	$(V)$(QEMU) -parallel stdio $(QEMUOPTS) -serial null

build-%: touch
	$(V)$(MAKE) $(MAKEOPTS) "DEFS+=-DTEST=$* -DTESTSTART=$(RUN_PREFIX)$*_out_start -DTESTSIZE=$(RUN_PREFIX)$*_out_size"

.PHONY: grade touch

GRADE_GDB_IN	:= .gdb.in
GRADE_QEMU_OUT	:= .qemu.out
HANDIN			:= proj$(PROJ)-handin.tar.gz

TOUCH_FILES		:= kern/process/proc.c

MAKEOPTS		:= --quiet --no-print-directory

grade:
	$(V)$(MAKE) $(MAKEOPTS) clean
	$(V)$(SH) tools/grade.sh

touch:
	$(V)$(foreach f,$(TOUCH_FILES),$(TOUCH) $(f))

print-%:
	@echo $($(shell echo $(patsubst print-%,%,$@) | $(TR) [a-z] [A-Z]))

.PHONY: clean distclean handin packall
clean:
	$(V)$(RM) $(GRADE_GDB_IN) $(GRADE_QEMU_OUT)
	$(V)$(RM) -r $(OBJDIR) $(BINDIR)

distclean: clean
	$(V)$(RM) $(HANDIN)

handin: distclean
	$(V)$(TAR) -cf - `find . -type f -o -type d | grep -v '^\.$$' | grep -v '/CVS/' \
					| grep -v '/\.git/' | grep -v '/\.svn/' | grep -v "$(HANDIN)"` \
					| $(ZIP) > $(HANDIN)

