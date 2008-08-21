#define _POSIX_C_SOURCE 1
#include "Board.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

Board board;
int turn;
int joker[2];

const char *command1;
const char *command2;
const char *logfile1 = "player1.log";
const char *logfile2 = "player2.log";

FILE *fpin[2], *fpout[2];

void create_process( const char *command, const char *logfile,
                     FILE **fpin, FILE **fpout )
{
    FILE *fperr;
    int fdin[2], fdout[2];
    int res;

    res = pipe(fdin);
    assert(res == 0);
    res = pipe(fdout);
    assert(res == 0);

    res = fork();
    assert(res >= 0);
    if (res == 0)
    {
        res = dup2(fdout[0], 0);
        assert(res >= 0);
        res = dup2(fdin[1], 1);
        assert(res >= 0);
        close(fdin[0]);
        close(fdin[1]);
        close(fdout[0]);
        close(fdout[1]);
        if (logfile != NULL)
        {
            fperr = fopen(logfile, "wt");
            assert(fperr != NULL);
            res = dup2(fileno(fperr), 2);
            fclose(fperr);
            assert(res >= 0);
        }
        system(command);
        exit(0);
    }
    else
    {
        *fpin  = fdopen(fdin[0], "rt");
        assert(*fpin != NULL);
        *fpout = fdopen(fdout[1], "wt");
        assert(*fpout != NULL);
        close(fdin[1]);
        close(fdout[0]);
    }
}

void initialize()
{
    board_construct(&board, 12345);
    turn = 0;
    joker[0] = joker[1] = -1;

    create_process(command1, logfile1, &fpin[0], &fpout[0]);
    create_process(command2, logfile2, &fpin[1], &fpout[2]);
}

bool process_move()
{
    return false;
}

void print_status()
{
    char buf[201];
    board_encode_full(&board, buf);
    printf("%s\n", buf);
    board_print(&board, stdout);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: arbiter <command1> <command2>\n");
        return 1;
    }
    command1 = argv[1];
    command2 = argv[2];

    initialize();
    while (process_move()) { };
    print_status();

    return 0;
}
