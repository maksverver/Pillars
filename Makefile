CFLAGS=-g -ansi -Wall -Wextra -O0 -m64
LDFLAGS=-g -m64
LDLIBS=-lm
OBJS=Analysis.o Board.o HashTable.o nvalue_work.o
ANALYZE_OBJS=analyze.o $(OBJS)
ARBITER_OBJS=arbiter.o Board.o 
BENCHMARK_SRCS=benchmark.c HashTable.c Board.c Analysis.c nvalue_work.c

PLAYER_SRCS=Analysis.c Board.c Debug.c HashTable.c nvalue_work.c main.c
PLAYER_OBJS=Analysis.o Board.o Debug.o HashTable.o nvalue_work.o main.o

EXECUTABLES=analyze arbiter benchmark player

all: $(EXECUTABLES) player.c

analyze: $(ANALYZE_OBJS)
	$(CC) $(LDFLAGS) -o analyze $(ANALYZE_OBJS) $(LDLIBS)

arbiter: $(ARBITER_OBJS)
	$(CC) $(LDFLAGS) -o arbiter $(ARBITER_OBJS) $(LDLIBS)

runguard: runguard.c
	# Can be used in conjunction with the arbiter
	$(CC) -o $@ -Wall -O2 -m32 $^

benchmark: $(BENCHMARK_SRCS)
	$(CC) $(LDFLAGS) $(CFLAGS) -DANALYSIS_MAX_SIZE=25 -o benchmark $(BENCHMARK_SRCS) $(LDLIBS)

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
