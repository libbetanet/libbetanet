# Makefile

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
