CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -O3 -Iinclude $(shell sdl2-config --cflags)
LDFLAGS ?= $(shell sdl2-config --libs) -lm

SRCDIR = src
INCDIR = include
OBJDIR = obj
BIN = matrix-sdl

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean run

all: $(BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJDIR) $(BIN)

run: $(BIN)
	./$(BIN)
