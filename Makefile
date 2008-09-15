CFLAGS=-g -ansi -Wall -Wextra -O2 -m32 -I/usr/include/libxml2
LDFLAGS=-g -m32
LDLIBS=-lm -lxml2
ANALYZE_OBJS=Board.o Analysis.o Group.o
ARBITER_OBJS=arbiter.o Board.o 
BENCHMARK_SRCS=benchmark.c Board.c Analysis.c Group.c

PLAYER_SRCS=Analysis.c Group.c Board.c Debug.c main.c
PLAYER_OBJS=Analysis.o Group.o Board.o Debug.o main.o

EXECUTABLES=analyze analyze-board arbiter player board-to-xml shapes

all: $(EXECUTABLES) player.c

analyze: $(ANALYZE_OBJS) analyze.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

analyze-board: $(ANALYZE_OBJS) analyze-board.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

arbiter: $(ARBITER_OBJS)
	$(CC) $(LDFLAGS) -o arbiter $(ARBITER_OBJS) $(LDLIBS)

runguard: runguard.c
	# Can be used in conjunction with the arbiter
	$(CC) -o $@ -Wall -O2 -m32 $^

benchmark:
	$(CC) $(CFLAGS) $(LDFLAGS) -DANALYSIS_MAX_SIZE=25 -o benchmark $(BENCHMARK_SRCS) $(LDLIBS)

board-to-xml: Board.o tools/board-to-xml.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I. -o $@ $^

player.c: $(PLAYER_SRCS) Analysis.h Board.h
	./compile.pl $(PLAYER_SRCS) > player.c

#player: player.c
#	$(CC) $(CFLAGS) $(LDFLAGS) -o player player.c $(LDLIBS)

player: $(PLAYER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o player $(PLAYER_OBJS) $(LDLIBS)

shapes: Board.o Analysis.o Group.o shapes.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o
	
distclean: clean
	rm -f $(EXECUTABLES) player.c

.PHONY: all clean distclean
