CC := gcc -c
LD := gcc

RM := rm -f
MKDIR := mkdir -p -v

SRC := $(wildcard src/*.c)
OBJ := $(SRC:%=%.o)

CFLAGS := -Wall -Wextra
LDFLAGS := 

NAME=bin/idk

all: setup $(NAME)

$(NAME): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

setup:
	$(MKDIR) bin

clean:
	$(RM) $(OBJ) $(NAME)
