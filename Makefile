SHELL  := /bin/sh
OUT    := out
CC     := clang
CFLAGS := \
	-std=c17 \
	-O3 \
	-Wall \
	-Werror \
	-fsanitize=undefined

ifeq ($(shell command -v python3.12),)
	ifeq ($(shell command -v python3.11),)
		ifeq ($(shell command -v python3.10),)
			PYTHON := python3
		else
			PYTHON := python3.10
		endif
	else
		PYTHON := python3.11
	endif
else
	PYTHON := python3.12
endif

$(addprefix $(OUT)/,py_aoc): $(OUT)/%: src/%.c $(wildcard include/*.h) $(OUT)
	$(CC) \
		$(CFLAGS) \
		$(shell $(PYTHON)-config --cflags --embed) \
		-I./include \
		$< \
		-o $@ \
		$(shell $(PYTHON)-config --ldflags --embed)

$(OUT):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rv $(OUT)

SOLUTIONS     := $(basename $(notdir $(wildcard include/d??.h)))
RUN_SOLUTIONS := $(addprefix run_,$(SOLUTIONS))

.PHONY: $(RUN_SOLUTIONS)
$(RUN_SOLUTIONS): $(OUT)/py_aoc
	@$< -c "from aoc_solve import y2022; print(y2022(int('$(patsubst run_d%,%,$@)')))"
