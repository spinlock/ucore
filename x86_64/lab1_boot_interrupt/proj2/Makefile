PROJ	:= 2
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
	echo "*** Error: Couldn't find an x86_64-ucore-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with x86_64-ucore-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your x86_64-ucore-elf toolchain is installed with a command" 1>&2; \
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
CFLAGS	:= -fno-builtin -Wall -ggdb -nostdinc
CFLAGS	+= $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
CC32	:= -m32
CC64	:= -m64
CTYPE	:= c S

LD      := $(GCCPREFIX)ld
LDFLAGS	:=
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

# for match pattern
match = $(shell echo $(2) | $(AWK) '{for(i=1;i<=NF;i++){if(match("$(1)","^"$$(i)"$$")){exit 1;}}}'; echo $$?)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

INCLUDE	+= libs/

CFLAGS	+= $(addprefix -I,$(INCLUDE))

# -------------------------------------------------------------------

# create bootblock
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) $(CC32) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,boot/bootasm.S) $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LD32) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
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

$(UCOREIMG): $(call totarget,bootblock)
	$(V)dd if=/dev/zero of=$@ count=10000
	$(V)dd if=$< of=$@ conv=notrunc

$(call create_target,ucore.img)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

$(call finish_all)

IGNORE_ALLDEPS	= gdb \
				  clean \
				  distclean \
				  print-.+ \
				  handin

ifeq ($(call match,$(MAKECMDGOALS),$(IGNORE_ALLDEPS)),0)
-include $(ALLDEPS)
endif

targets: $(TARGETS)

.DEFAULT_GOAL := targets

QEMUOPTS = -hda $(UCOREIMG)

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

HANDIN			:= proj$(PROJ)-handin.tar.gz

print-%:
	@echo $($(shell echo $(patsubst print-%,%,$@) | $(TR) [a-z] [A-Z]))

.PHONY: clean distclean handin
clean:
	$(V)$(RM) -r $(OBJDIR) $(BINDIR)

distclean: clean
	$(V)$(RM) $(HANDIN)

handin: distclean
	$(V)$(TAR) -cf - `find . -type f -o -type d | grep -v '^\.$$' | grep -v '/CVS/' \
					| grep -v '/\.git/' | grep -v '/\.svn/' | grep -v "$(HANDIN)"` \
					| $(ZIP) > $(HANDIN)

