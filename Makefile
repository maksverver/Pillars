CFLAGS=-O2 -g -ansi -Wall -Wextra -Werror
LDFLAGS=-g
LDLIBS=-lm
OBJS=test.o Board.o Analysis.o

all: test

test: $(OBJS)
	$(CC) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)

clean:
	rm -f $(OBJS)
	
distclean: clean
	rm -f test

.PHONY: all clean distclean
