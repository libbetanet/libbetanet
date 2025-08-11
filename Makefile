# Should be either debug|release
TARGET  ?= debug

INCDIR  := $(CURDIR)/include
SRCDIR  := $(CURDIR)/src
TOOLDIR := $(CURDIR)/tools
TESTDIR := $(CURDIR)/tests
EXTDIR  := $(CURDIR)/ext
BUILDDIR:= $(CURDIR)/build

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -fPIC
LDFLAGS ?= -L$(EXTDIR)/install/lib
LIBS    ?= -lcrypto -lnoisekeys -lnoiseprotobufs -lnoiseprotocol

ifeq ($(TARGET), debug)
	CFLAGS += -g -DDEBUG
else ifeq ($(TARGET), release)
	CFLAGS += -O2
endif

INCLUDES := -I$(INCDIR) -I$(EXTDIR)/install/include

SRC := \
  $(SRCDIR)/htx.c \
  $(SRCDIR)/frame.c \
  $(SRCDIR)/varint.c \
  $(SRCDIR)/aead_openssl.c \

OBJS := $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

LIB := $(BUILDDIR)/libhtx.a

TEST := $(BUILDDIR)/test_main
TOOL := $(BUILDDIR)/htx-dump

.PHONY: ext full_clean all clean test

all: ext $(LIB) $(TEST) $(TOOL)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)

$(TEST): $(TESTDIR)/test_main.c $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -o $@ $(LIB) $(LIBS)

$(TOOL): $(TOOLDIR)/htx-dump.c $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -o $@ $(LIB) $(LIBS)

ext:
	$(MAKE) -C $(EXTDIR) libs

test: $(TEST)
	$(TEST)

clean:
	rm -rf $(BUILDDIR)


full_clean: clean
	$(MAKE) -C $(EXTDIR) clean

FUZZ_CC ?= clang
FUZZ_CFLAGS = -O1 -g -fsanitize=address,undefined -I$(INCDIR) -fno-sanitize=function,vptr
FUZZ_LDFLAGS = -fsanitize=address,undefined,fuzzer -fno-sanitize=function,vptr
FUZZ_LIBS = -lcrypto

FUZZ_SRCS := $(wildcard fuzz/*.c)

FUZZ_BINS := $(FUZZ_SRCS:fuzz/%.c=build/fuzz_%)

FUZZ_OBJS_DIR := $(BUILDDIR)/fuzz_objs
FUZZ_OBJS := $(SRC:$(SRCDIR)/%.c=$(FUZZ_OBJS_DIR)/%.o)

.PHONY: fuzz fuzz-smoke

fuzz: $(FUZZ_BINS)

$(FUZZ_OBJS_DIR)/fuzz_%.o: fuzz/%.c | $(FUZZ_OBJS_DIR)
	  @echo "  FUZZ CC   $<"
		@$(FUZZ_CC) $(FUZZ_CFLAGS) -c $< -o $@

build/fuzz_%: $(FUZZ_OBJS_DIR)/fuzz_%.o $(FUZZ_OBJS)
	  @echo "  FUZZ LD   $@"
	  @$(FUZZ_CC) $(FUZZ_CFLAGS) $^ -o $@ $(FUZZ_LDFLAGS) $(FUZZ_LIBS)

$(FUZZ_OBJS_DIR)/%.o: $(SRCDIR)/%.c | $(FUZZ_OBJS_DIR)
	  @echo "  FUZZ CC   $<"
	  @$(FUZZ_CC) $(FUZZ_CFLAGS) -c $< -o $@

$(FUZZ_OBJS_DIR):
	  @mkdir -p $(FUZZ_OBJS_DIR)

fuzz-smoke: fuzz 
	  @mkdir -p fuzz/corpora/varint fuzz/corpora/pack 
		ASAN_OPTIONS=detect_leaks=0 ./build/fuzz_varint -max_total_time=60 fuzz/corpora/varint || true 
		ASAN_OPTIONS=detect_leaks=0 ./build/fuzz_pack_roundtrip -max_total_time=60 fuzz/corpora/pack || true 
		ASAN_OPTIONS=detect_leaks=0 ./build/fuzz_mutation -max_total_time=60 fuzz/corpora/pack || true
