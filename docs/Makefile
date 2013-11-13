.DEFAULT_GOAL = main.pdf

RM		:= rm -rf
MKDIR	:= mkdir -p
TEX		:= xelatex

OUTPUT	:= output

$(OUTPUT):
	@$(MKDIR) $@ $@/parts $@/others

main.pdf: main.tex | $(OUTPUT)
#	@$(TEX) $<
#	@$(TEX) $<
	@$(TEX) -output-directory $(OUTPUT) $<
	@$(TEX) -output-directory $(OUTPUT) $<

.PHONY: clean

clean:
	@$(RM) $(OUTPUT)
