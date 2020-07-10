CC=gcc
CFLAGS=-I. -lncurses
DEPS=util/drawing.h util/box_drawing.h
OBJ=util/drawing.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm **/*.o
