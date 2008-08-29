CFLAGS=-g -ansi -Wall -Wextra -O2 -m32 -DLOCAL
LDFLAGS=-g -m32
LDLIBS=-lm
OBJS=Analysis.o Board.o HashTable.o nvalue_new_work.o
ANALYZE_OBJS=analyze.o $(OBJS)
ARBITER_OBJS=arbiter.o Board.o 
BENCHMARK_OBJS=benchmark.o $(OBJS)

PLAYER_SRCS=Analysis.c Board.c Debug.c HashTable.c nvalue_new_work.c main.c
PLAYER_OBJS=Analysis.o Board.o Debug.o HashTable.o nvalue_new_work.o main.o

EXECUTABLES=analyze arbiter benchmark player

all: $(EXECUTABLES) player.c

analyze: $(ANALYZE_OBJS)
	$(CC) $(LDFLAGS) -o analyze $(ANALYZE_OBJS) $(LDLIBS)

arbiter: $(ARBITER_OBJS)
	$(CC) $(LDFLAGS) -o arbiter $(ARBITER_OBJS) $(LDLIBS)

benchmark: $(BENCHMARK_OBJS)
	$(CC) $(LDFLAGS) -o benchmark $(BENCHMARK_OBJS) $(LDLIBS)

player.c: $(PLAYER_SRCS) Analysis.h Board.h
	./compile.pl $(PLAYER_SRCS) > player.c

#player: player.c
#	$(CC) $(CFLAGS) $(LDFLAGS) -o player player.c $(LDLIBS)

player: $(PLAYER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o player $(PLAYER_OBJS) $(LDLIBS)

clean:
	rm -f *.o
	
distclean: clean
	rm -f $(EXECUTABLES) player.c

.PHONY: all clean distclean
