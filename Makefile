# Makefile

# Should be either debug|release
TARGET  ?= debug

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -fPIC
LDFLAGS ?=
LIBS    ?= -lcrypto

ifeq ($(TARGET), debug)
	CFLAGS += -g -DDEBUG
else ifeq ($(TARGET), release)
	CFLAGS += -O2
endif

INCDIR  := include
SRCDIR  := src
TOOLDIR := tools
TESTDIR := tests
BUILDDIR:= build


INCLUDES := -I$(INCDIR)

SRC := \
  $(SRCDIR)/htx.c \
  $(SRCDIR)/frame.c \
  $(SRCDIR)/varint.c \
  $(SRCDIR)/aead_openssl.c \
	$(SRCDIR)/X25519_utils.c

OBJS := $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

LIB := $(BUILDDIR)/libhtx.a

TEST := $(BUILDDIR)/test_main
TOOL := $(BUILDDIR)/htx-dump

.PHONY: all clean test

all: $(LIB) $(TEST) $(TOOL)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)

$(TEST): $(TESTDIR)/test_main.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIB) $(LIBS)

$(TOOL): $(TOOLDIR)/htx-dump.c $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIB) $(LIBS)

test: $(TEST)
	$(TEST)

clean:
	rm -rf $(BUILDDIR)
