CFLAGS=-g -ansi -Wall -Wextra -O2 -m32 -I/usr/include/libxml2 -I.
LDFLAGS=-g -m32
LDLIBS=-lm -lxml2

SRC=Board.c Analysis.c Group.c
OBJ=Board.o Analysis.o Group.o

PLAYER_SRCS=Analysis.c Group.c Board.c Debug.c main.c

EXECUTABLES=analyze benchmark analyze-board arbiter player board-to-xml shapes

all: $(EXECUTABLES) player.c

analyze: $(OBJ) analyze.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

analyze-board: $(OBJ) analyze-board.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

arbiter: $(OBJ) arbiter.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

runguard: runguard.c
	# Can be used in conjunction with the arbiter
	$(CC) -o $@ -Wall -O2 -m32 $^

benchmark: $(SRC) benchmark.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DANALYSIS_MAX_SIZE=25 -fno-inline-functions -o $@ $^ $(LDLIBS)

player.c: $(PLAYER_SRCS) Analysis.h Board.h
	./compile.pl $(PLAYER_SRCS) > player.c

player: player.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o player player.c $(LDLIBS)

board-to-xml: Board.o tools/board-to-xml.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

shapes: ${OBJ} tools/shapes.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o
	
distclean: clean
	rm -f $(EXECUTABLES) player.c

.PHONY: all clean distclean
