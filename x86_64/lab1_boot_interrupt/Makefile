LAB		:= 1
HANDIN	:= lab$(LAB)_handin.tar.gz

PROJS = $(foreach f,$(wildcard proj*),\
	$(shell if [ "x"$f != "x" ] && [ -f $f/Makefile ]; then echo $f; fi))

AWK		:= awk
TAR		:= tar
ZIP		:= gzip
MAKE	:= make
RM		:= rm -f

MAKEOPTS = --quiet --no-print-directory

.PHONY: all clean grade handin

all:
	@(for p in $(PROJS); do (echo "compile: $${p}"; $(MAKE) -C $${p} $(MAKEOPTS)); done)

clean:
	@(for p in $(PROJS); do (echo "clean: $${p}"; $(MAKE) -C $${p} clean $(MAKEOPTS)); done)

distclean:
	@(for p in $(PROJS); do (echo "distclean: $${p}"; $(MAKE) -C $${p} distclean $(MAKEOPTS)); done)
	@$(RM) $(HANDIN)

grade:
	@(for p in $(PROJS); do (										\
		echo "grade: $${p}" | $(AWK) '{printf "%-48s ", $$0}';		\
		$(MAKE) -C $${p} grade > /dev/null 2>&1;					\
		if [ $$? -ne 0 ]; then										\
			echo "----";											\
		else														\
			echo "PASS";											\
			$(MAKE) -C $${p} clean > /dev/null 2>&1;				\
		fi;															\
	); done)

handin: distclean
	@$(TAR) -cf - `find . -type f -o -type d | grep -v '^\.$$' | grep -v '/CVS/' \
					| grep -v '/\.git/' | grep -v '/\.svn/' | grep -v "$(HANDIN)"` \
					| $(ZIP) > $(HANDIN)

