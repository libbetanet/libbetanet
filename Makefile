# Makefile
CC      ?= cc
CFLAGS  ?= -O2 -g -std=c11 -Wall -Wextra -Werror -fPIC
LDFLAGS ?=
LIBS    ?= -lcrypto

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
