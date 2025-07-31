# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -O3 -march=native -flto
TARGETS = small-pSquare_tau_0 small-pSquare_tau_1 small-pSquare_tau_2

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)