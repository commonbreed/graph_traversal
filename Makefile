SHELL:=/bin/bash
CC=gcc
CFLAGS=-I. -lncurses
DEPS=util/drawing.h util/box_drawing.h
OBJ=util/drawing.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
ifeq (, $(shell which gawk))
	$(error "No gawk in $(PATH), consider doing apt-get install gawk")
endif
ifeq (, $(shell dpkg -l | grep libncurses-dev))
	$(error "Package libncurses-dev doesn't appear to be installed")
endif
ifeq ("ben", "$(shell id -nu)")
	@echo "Imposter!"
endif
	$(CC) -o $@ $^ $(CFLAGS)

debug: CFLAGS+=-g
debug: main

debug2: CFLAGS+=-DTDEBUG
debug2: debug

.PHONY: clean

clean:
	rm **/*.o
