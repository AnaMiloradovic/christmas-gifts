PROGRAM   = 1
CC        = gcc
CFLAGS    = -g -Wall -std=c99
LDFLAGS   = -lGL -lGLU -lglut -lm 

$(PROGRAM): main.o image.o
	$(CC) -o $(PROGRAM) main.o image.o $(LDFLAGS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)


