LAB = lab1_boot_interrupt \
	  lab2_memory \
	  lab3_process \
	  lab4_scheduling \
	  lab5_mutexsync \
	  lab6_filesystem

SUBDIRS = $(foreach d,$(LAB),$(wildcard $(d)/proj*))

AWK		:= awk
MAKE	:= make

MAKEOPTS = --quiet --no-print-directory

.DEFAULT_GOAL := all

.PHONY: all
all:
	@echo compile all:
	@(for p in $(SUBDIRS); do (echo "compile: $${p}"; $(MAKE) -C $${p} $(MAKEOPTS)); done)

.PHONY: clean
clean:
	@(for p in $(SUBDIRS); do (echo "clean: $${p}"; $(MAKE) -C $${p} clean $(MAKEOPTS)); done)

.PHONY: list
list:
	@(for p in $(SUBDIRS); do echo $${p}; done)

.PHONY: grade
grade:
	@(for p in $(SUBDIRS); do (										\
		echo "grade: $${p}" | $(AWK) '{printf "%-48s ", $$0}';		\
		$(MAKE) -C $${p} grade > /dev/null 2>&1;					\
		if [ $$? -ne 0 ]; then										\
			echo "----";											\
		else														\
			echo "PASS";											\
			$(MAKE) -C $${p} clean > /dev/null 2>&1;				\
		fi;															\
	); done)

.PHONY: help
help:
	@echo "supported command:"
	@echo "all:                build *all* projects"
	@echo "clean:              clean *all* projects"
	@echo "list:               print projects list"
	@echo "grade:              build and test *all* projects"
	@echo "compile and run:"
	@echo "   make p\$${num}      compile & run \$${num} project"
	@echo "   make p\$${num}nx    compile & run \$${num} project without X-window"
	@echo "   make p\$${num}d     compile & run \$${num} project in *gdb-debug* mode"
	@echo "   make p\$${num}dnx   compile & run \$${num} project in *gdb-debug* mode without X-window"
	@echo "   make p\$${num}k     compile & run \$${num} project using kgdb, only for proj4.2"
	@echo "   make p\$${num}knx   compile & run \$${num} project using kgdb and without X-window, only for proj4.2"
	@echo "   make p\$${num}g     compile & test \$${num} project"

define do_run
.PHONY: $(1)
$(1):
	@$(MAKE) -C $(2) --quiet $(3)
endef

run = $(eval $(call do_run,p$(1),$(2),$(3)))

# qemu-x:
# for example: make p5.2
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p))),$(p),qemu))

# qemu-nox:
# for example: make p5.2nx
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))nx,$(p),qemu-nox))

# qemu-debug:
# for example: make p5.2d
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))d,$(p),debug))

# qemu-debug-nox:
# for example: make p5.2dnx
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))dnx,$(p),debug-nox))

# qemu-kgdb:
# for example: make p4.2k
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))k,$(p),kgdb))

# qemu-kgdb:
# for example: make p4.2knx
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))knx,$(p),kgdb-nox))

# grade:
# for example: make p5.2g
$(foreach p,$(SUBDIRS),$(call run,$(patsubst proj%,%,$(notdir $(p)))g,$(p),grade))

