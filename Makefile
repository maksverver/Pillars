CFLAGS=-g -ansi -Wall -Wextra -O2 -m32 -DLOCAL
LDFLAGS=-g -m32
LDLIBS=-lm
OBJS=Board.o Analysis.o nvalue_new_work.o
ANALYZE_OBJS=analyze.o $(OBJS)
ARBITER_OBJS=arbiter.o Board.o
PLAYER_SRCS=Analysis.c Board.c HashTable.c nvalue_new_work.c main.c

all: analyze arbiter player

analyze: $(ANALYZE_OBJS)
	$(CC) $(LDFLAGS) -o analyze $(ANALYZE_OBJS) $(LDLIBS)

arbiter: $(ARBITER_OBJS)
	$(CC) $(LDFLAGS) -o arbiter $(ARBITER_OBJS) $(LDLIBS)

player.c: $(PLAYER_SRCS) Analysis.h Board.h
	./compile.pl $(PLAYER_SRCS) > player.c

player: player.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o player player.c $(LDLIBS)

clean:
	rm -f $(OBJS) analyze.o player
	
distclean: clean
	rm -f analyze player

.PHONY: all clean distclean
