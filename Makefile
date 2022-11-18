SHELL := /bin/sh

CPYTHON_ARCHIVE_URL := https://www.python.org/ftp/python/3.11.0/Python-3.11.0.tgz
CPYTHON_ARCHIVE_MD5 := c5f77f1ea256dc5bdb0897eeb4d35bb0


Python.tgz:
	curl -L -o Python.tgz "$(CPYTHON_ARCHIVE_URL)"

py_src: Python.tgz
	[[ "$$(md5 Python.tgz)" == "MD5 (Python.tgz) = $(CPYTHON_ARCHIVE_MD5)" ]] || exit 1
	mkdir -p $@
	tar --directory py_src -zx --strip-components 1 -f Python.tgz

python: py_src
	cd py_src && ./configure --prefix $(abspath python) --enable-optimizations
	cd py_src && make -j && make -j test && make install


CLANG := clang
CFLAGS := \
	-x c \
	-std=c17 \
	-Wall \
	-Werror \
	-fsanitize=address \
	-fsanitize=undefined

OUT ?= out
SRC_FILES := $(wildcard src/??.c)
BIN_FILES := $(notdir $(basename $(SRC_FILES)))
OUT_FILES := $(addprefix $(OUT)/,$(BIN_FILES))

.PHONY: all
all: $(OUT_FILES)

.PHONY: clean
clean:
	rm -rv $(OUT)

$(OUT):
	mkdir -p $@

$(OUT_FILES): $(OUT)/%: src/%.c | $(OUT) python
	$(CLANG) \
		$(CFLAGS) \
		$(shell ./python/bin/python3-config --cflags) \
		$(shell ./python/bin/python3-config --ldflags --embed) \
		-o $@ \
		$^
