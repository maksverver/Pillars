CFLAGS=-g -ansi -Wall -Wextra -O2 -m32 -march=pentium4
LDFLAGS=-g -m32
LDLIBS=-lm
OBJS=test.o Board.o Analysis.o nvalue_new_work.o

all: test

test: $(OBJS)
	$(CC) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)

clean:
	rm -f $(OBJS)
	
distclean: clean
	rm -f test

.PHONY: all clean distclean
