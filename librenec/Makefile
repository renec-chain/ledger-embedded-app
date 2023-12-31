mode = debug
target := $(shell uname -s)
variant = $(target)_$(mode)
o = target/$(variant)

test_files := $(wildcard *_test.c)
test_exes = $(patsubst %.c,$o/%,$(test_files))
test_oks = $(addsuffix .ok,$(test_exes))

all: $(test_oks) $(test_exes) $o/librenec.a

CFLAGS += -Werror -Wall -Wextra -pedantic -Wshadow -Wcast-qual -Wcast-align -Wno-unused-parameter
CFLAGS += -fPIC
CFLAGS += -Iinclude
CFLAGS += $($(mode)_CFLAGS)

ifeq ($(COVERAGE),1)
	CFLAGS += --coverage
endif

debug_CFLAGS = -g -fsanitize=address -fsanitize=undefined
release_CFLAGS = -O2

librenec_source_files = $(filter-out %_test.c,$(wildcard *.c))
librenec_object_files = $(patsubst %.c,$o/%.o,$(librenec_source_files))
librenec_depend_files = $(patsubst %.c,$o/%.d,$(librenec_source_files))

-include $(librenec_depend_files)

$o/%.o: %.c
	@echo "==> Compile $<"
	@mkdir -p $(@D)
	$(CC) -MMD -c $(CFLAGS) $< -o $@

#
# unit tests
#
# Note: this executes on the host, not via QEMU
$o/%_test.ok: $o/%_test
	@echo "==> Run test $<"
	@$<
	@touch $@

$o/%_test: $o/%_test.o $o/librenec.a
	@echo "==> Link test $@"
	$(CC) $(CFLAGS) -o $@ $^

#
# librenec
#
$o/librenec.a: $(librenec_object_files)
	@echo "==> Create static library $@"
	ar rcs $@ $^

#
# clean
#
.PHONY: clean
clean:
	@echo "==> Clean build directory"
	rm -rf target
