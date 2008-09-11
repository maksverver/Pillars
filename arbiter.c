#define _POSIX_C_SOURCE 1
#include "Board.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

static int boardnum;
static const char *command1;
static const char *command2;
static const char *logfile1 = "player1.log";
static const char *logfile2 = "player2.log";

static Board board;
static int turn;
static int joker[2], bonus[2];
static int winner, score[2];
static FILE *fpin[2], *fpout[2];
static const char *player1_error = NULL, *player1_line = NULL;
static const char *player2_error = NULL, *player2_line = NULL;

static void create_process( const char *command, const char *logfile,
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

static void initialize()
{
    char buf[3];
    Point p;
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, NULL);

    board_construct(&board, boardnum - 1);
    turn = 0;
    joker[0] = joker[1] = -1;
    bonus[0] = bonus[1] = 0;
    winner = -1;
    score[0] = score[1] = 0;

    create_process(command1, logfile1, &fpin[0], &fpout[0]);
    create_process(command2, logfile2, &fpin[1], &fpout[1]);

    for (p.r = 0; p.r < 10; ++p.r)
    {
        for (p.c = 0; p.c < 10; ++p.c)
        {
            if (board[p.r][p.c] == 0) continue;
            point_encode(&p, buf);
            fprintf(fpout[0], "%s\n", buf);
            fprintf(fpout[1], "%s\n", buf);
        }
    }
    fflush(fpout[1]);
    fprintf(fpout[0], "Start\n");
    fflush(fpout[0]);
}

static void print_cdata(const char *msg)
{
    while (*msg)
    {
        int i = *(unsigned char*)msg++;
        if (i < 32 || i >= 127)
        {
            printf("&#%d;", i);
        }
        else
        {
            switch (i)
            {
            case '<':  printf("&lt;"); break;
            case '>':  printf("&gt;"); break;
            case '&':  printf("&amp;"); break;
            case '"':  printf("&quot;"); break;
            case '\'': printf("&apos;"); break;
            default:   putchar(i);
            }
        }
    }
}

static void player_error(const char *msg, const char *line)
{
    *((turn%2) ? &player2_error : &player1_error) = msg;
    *((turn%2) ? &player2_line  : &player1_line) = line;
    winner = (turn+1)%2;
    score[turn%2] = 0;
    score[winner] = 18 + (joker[winner] == -1 ? board_empty_area(&board)/10
                                              : bonus[winner]);
}

static bool process_move()
{
    int p, len;
    char line[80], buf[5];
    Rect move;

    p = turn%2;

    /* Read a line of input */
    if (fgets(line, sizeof(line), fpin[p]) == NULL)
    {
        player_error("unexpected EOF", NULL);
        return false;
    }
    len = strlen(line);
    if (len == 0 || line[len - 1] != '\n')
    {
        player_error("missing newline character", line);
        return false;
    }
    line[len - 1] = '\0';

    if (line[0] == '!')
    {
        if (joker[p] != -1)
        {
            player_error("joker no longer available", line);
            return false;
        }
        joker[p] = turn;
    }

    if (!rect_decode(&move, line + (line[0] == '!')))
    {
        player_error("invalid rectangle", line);
        return false;
    }

    if (!board_is_valid_move(&board, &move))
    {
        player_error("invalid rectangle", line);
        return false;
    }

    /* Perform move */
    if (joker[p] == turn) bonus[p] = board_empty_area(&board)/10;
    board_fill(&board, &move, turn + 1);
    turn += 1;

    /* Notify opponent */
    rect_encode(&move, buf);
    fprintf(fpout[turn%2], "%s\n", buf);
    fflush(fpout[turn%2]);

    return true;
}

static void print_failure(int id, const char *error, const char *line)
{
    printf("<failure player=\"%d\">\n", id);
    printf("<error>");
    print_cdata(error);
    printf("</error>\n");
    if (line != NULL)
    {
        printf("<line>");
        print_cdata(line);
        printf("</line>\n");
    }
    printf("</failure>\n");
}

static void print_pillars()
{
    Point p;
    char buf[3];

    printf("<pillars perm=\"%d\">\n", boardnum);
    for (p.r = 0; p.r < 10; ++p.r)
    {
        for (p.c = 0; p.c < 10; ++p.c)
        {
            if (board[p.r][p.c] == -1)
            {
                point_encode(&p, buf);
                printf("<point>%s</point>\n", buf);
            }
        }
    }
    printf("</pillars>\n");
}

static void print_moves()
{
    int n;
    Rect move;
    char buf[5];

    printf("<moves count=\"%d\">\n", turn);
    for (n = 0; n < turn; ++n)
    {
        board_get_move(&board, &move, n);
        rect_encode(&move, buf);
        printf( "<rect>%s%s</rect>\n",
                (joker[0] == n || joker[1] == n) ? "!" : "", buf );
    }
    printf("</moves>\n");
}

static void print_status()
{
    char buf[201];

    board_encode_full(&board, buf);

    printf("<?xml version=\"1.0\"?>\n");
    printf("<game>\n");
    printf("<board>%s</board>\n", buf);
    print_pillars();
    print_moves();
    printf("<result winner=\"%d\" score1=\"%d\" score2=\"%d\">"
           "player %d won! Score: %d-%d.</result>\n",
           winner + 1, score[0], score[1],
           winner + 1, score[0], score[1] );
    if (player1_error != NULL) print_failure(1, player1_error, player1_line);
    if (player2_error != NULL) print_failure(2, player2_error, player2_line);
    printf("</game>\n");
}

int main(int argc, char *argv[])
{
    if (argc == 3 || argc == 4)
    {
        command1 = argv[1];
        command2 = argv[2];
        boardnum = argc == 4 ? atoi(argv[3]) : 0;
    }
    else
    if (argc == 5 || argc == 6)
    {
        command1 = argv[1];
        logfile1 = argv[2];
        command2 = argv[3];
        logfile2 = argv[4];
        boardnum = argc == 6 ? atoi(argv[5]) : 0;
    }
    else
    {
        printf("usage: arbiter <command1> <command2> [<board>]\n"
               "       arbiter <command1> <logfile1> <command2> <logfile2> [<board>]\n"
               "       (board must be between 1 and 3628800, inclusive)\n");
        return 1;
    }
    if (boardnum < 1 || boardnum >= NUM_BOARDS)
    {
        /* Generate random board number */
        unsigned rng = (unsigned)getpid() ^ (unsigned)time(NULL);
        rng = rng*1103515245 + 12345;
        boardnum = rng/65536%32768;
        rng = rng*1103515245 + 12345;
        boardnum = 32768*boardnum + rng/65536%32768;
        boardnum = 1 + boardnum%NUM_BOARDS;
    }

    initialize();
    while (process_move())
    {
        if (board_empty_area(&board) == 0)
        {
            winner = turn%2;
            score[0] = (winner == 0 ? 18 + bonus[0] : 9 - bonus[0]);
            score[1] = (winner == 1 ? 18 + bonus[1] : 9 - bonus[1]);
            break;
        }
    }
    fprintf(fpout[0], "Quit\n");
    fprintf(fpout[1], "Quit\n");
    fflush(fpout[0]);
    fflush(fpout[1]);
    print_status();

    return 0;
}
