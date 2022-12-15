SHELL  := /bin/sh
OUT    := out
CC     := clang
CXX    := clang++
CFLAGS := \
	-std=c17 \
	-O3 \
	-Wall \
	-Werror \
	-fsanitize=address,undefined

.PHONY: py_aoc
py_aoc: $(OUT)/py_aoc

$(OUT)/py_aoc: $(OUT)/%: src/%.c include/common.h $(wildcard include/d??.h) $(OUT)
	$(CXX) \
		$(CFLAGS) \
		$(shell ./python/bin/python3-config --cflags --embed) \
		-I./include \
		$< \
		-o $@ \
		$(shell ./python/bin/python3-config --ldflags --embed)

.PHONY: py_aoc_debug
py_aoc_debug: $(OUT)/py_aoc_debug

$(OUT)/py_aoc_debug: src/py_aoc.c include/common.h $(wildcard include/d??.h) $(OUT)
	$(CXX) \
		$(CFLAGS) \
		$(shell ./python_debug/bin/python3-config --cflags --embed) \
		-I./include \
		$< \
		-o $@ \
		$(shell ./python_debug/bin/python3-config --ldflags --embed)

$(OUT):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rv $(OUT)


CPYTHON_ARCHIVE_URL := https://www.python.org/ftp/python/3.11.0/Python-3.11.0.tgz
CPYTHON_ARCHIVE_MD5 := c5f77f1ea256dc5bdb0897eeb4d35bb0

ifeq ($(shell uname),Darwin)
	MD5 := md5 -r
else
	MD5 := md5sum
endif

Python.tgz:
	curl -L -o $@ "$(CPYTHON_ARCHIVE_URL)"

py_src py_src_debug: Python.tgz
	[ "$$($(MD5) $< | tr -d ' ')" = "$(CPYTHON_ARCHIVE_MD5)$<" ] || exit 1
	mkdir -p $@
	tar --directory $@ -zx --strip-components 1 -f $<

python: py_src
	cd $< && CC=$(CC) CXX=$(CXX) ./configure \
		--prefix $(abspath ./$@) \
		--enable-optimizations
	cd $< && make -j && make -j testall && make install

python_debug: py_src_debug
	cd $< && CC=$(CC) CXX=$(CXX) ./configure \
		--prefix $(abspath ./$@) \
		--with-pydebug \
		--with-trace-refs \
		--with-assertions
	cd $< && make -j && make -j testall && make install
