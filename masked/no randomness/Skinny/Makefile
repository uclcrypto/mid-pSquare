# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -O3 -march=native -flto
TARGETS = Skinny-128-128 Skinny-128-256 Skinny-128-384

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)