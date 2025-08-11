RM := rm -f
MKDIR := mkdir -p -v

SRC := $(wildcard src/*.c)
OBJ := $(SRC:%=%.o)

CFLAGS := -Wall -Wextra
LDFLAGS := 

NAME=bin/idk

all: setup $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.c.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

setup:
	$(MKDIR) bin

clean:
	$(RM) $(OBJ) $(NAME)
