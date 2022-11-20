SHELL := /bin/sh

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

CPYTHON_ARCHIVE_URL := https://www.python.org/ftp/python/3.11.0/Python-3.11.0.tgz
CPYTHON_ARCHIVE_MD5 := c5f77f1ea256dc5bdb0897eeb4d35bb0

ifeq ($(shell uname),Darwin)
	MD5 := md5 -r
else
	MD5 := md5sum
endif

Python.tgz:
	curl -L -o $@ "$(CPYTHON_ARCHIVE_URL)"

py_src: Python.tgz
	[ "$$($(MD5) $< | tr -d ' ')" = "$(CPYTHON_ARCHIVE_MD5)$<" ] || exit 1
	mkdir -p $@
	tar --directory $@ -zx --strip-components 1 -f $<

python: py_src
	cd $< && ./configure --prefix $(abspath ./$@) --enable-optimizations
	cd $< && make -j && make -j test && make install

CLANG := clang
CFLAGS := \
	-std=c17 \
	-O3 \
	-Wall \
	-Werror \
	-fsanitize=undefined

$(OUT_FILES): $(OUT)/%: src/%.c | $(OUT)
	$(CLANG) \
		$(CFLAGS) \
		$(shell ./python/bin/python3-config --cflags --embed) \
		$^ \
		-o $@ \
		$(shell ./python/bin/python3-config --ldflags --embed)
