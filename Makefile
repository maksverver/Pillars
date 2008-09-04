CFLAGS=-g -ansi -Wall -Wextra -O2 -m32
LDFLAGS=-g -m32
LDLIBS=-lm
ANALYZE_OBJS=Board.o Analysis.o analyze.o
ARBITER_OBJS=arbiter.o Board.o 
BENCHMARK_SRCS=benchmark.c Board.c Analysis.c

PLAYER_SRCS=Analysis.c Board.c Debug.c main.c
PLAYER_OBJS=Analysis.o Board.o Debug.o main.o

EXECUTABLES=analyze arbiter player

all: $(EXECUTABLES) player.c

analyze: $(ANALYZE_OBJS)
	$(CC) $(LDFLAGS) -o analyze $(ANALYZE_OBJS) $(LDLIBS)

arbiter: $(ARBITER_OBJS)
	$(CC) $(LDFLAGS) -o arbiter $(ARBITER_OBJS) $(LDLIBS)

runguard: runguard.c
	# Can be used in conjunction with the arbiter
	$(CC) -o $@ -Wall -O2 -m32 $^

benchmark:
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

.PHONY: all clean distclean benchmark
