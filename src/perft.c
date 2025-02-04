#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include "board.h"
#include "movegeneration.h"
#include "fen.h"

static char *FENs[6] = { DEFAULT_FEN, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 
                         "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                         "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};
static uint64_t NumberOfMoves[6][32] = {0};
static uint64_t NumberOfEnPassant[6][32] = {0};
static uint64_t NumberOfMovesExpected[6][32] = {{20, 400, 8902, 197281, 4865609, 119060324, 3195901860},
                                                {48, 2039, 97862, 4085603, 193690690, 8031647685},
                                                {14, 191, 2812, 43238, 674624, 11030083, 178633661},
                                                {6, 264,  9467, 422333, 15833292, 706045033},
                                                {44, 1486, 62379, 2103487, 89941194},
                                                {46, 2079, 89890, 3894594, 164075551, 6923051137}};
static int depth[6] = {6, 5, 7, 6, 5, 5};

FILE *cmdFile;
FILE *numbersFile;
char *cmdFilename = "./stockfish/cmd.txt";

void CountMoves(Board *board, int depth, int maxDepth, int test);
void FindIncorrectPositions(Board *board, int depth, int failDepth);

int main ()
{
    CalculateMoveData();
    Board board = {0};

    // cmdFile = fopen("./stockfish/cmd.txt\x00", "w");
    // numbersFile = fopen("./stockfish/num.txt\x00", "w");

    for(int test = 3; test < 6; test++)
    {
        printf("=== test %i ===\n", test+1);
        InitBoard(&board, FENs[test]);
        CountMoves(&board, 0, depth[test], test);
        for(int i = 0; i < 32; i++)
        {
            if(NumberOfMoves[test][i] == 0) break;
            uint64_t number = NumberOfMoves[test][i];
            uint64_t expected = NumberOfMovesExpected[test][i];
            bool isCorrect = number == expected;
            printf("moves at depth %i: %li, expected: %li, is correct: %s\n", i+1, number, expected, (isCorrect) ? "true" : "false");
            // if(!isCorrect)
            // {
                // FindIncorrectPositions(&board, 0, i);
                // break;
            // }
        }
    }

    // fclose(cmdFile);
    // fclose(numbersFile);
    return 0;
}

void FindIncorrectPositions(Board *board, int depth, int failDepth)
{
    MoveList *list = GenerateMoves(board);

    for(int i = 0; i < list->count; i++)
    {
        Move move = list->moves[i];
        Board b = *board;

        if (depth == failDepth)
        {
            char *fen = CreateFen(&b);
            fprintf(cmdFile, "position fen %s\ngo perft 1\n", fen);
            fprintf(numbersFile, "Nodes searched: %i\n", list->count);
            free(fen);
            continue;
        }

        MakeMove(&b, move);
        FindIncorrectPositions(&b, depth+1, failDepth);
    }
}

void CountMoves(Board *board, int depth, int maxDepth, int test)
{
    MoveList *list = GenerateMoves(board);
    NumberOfMoves[test][depth] += list->count;

    if(depth == maxDepth-1 || list->moves == 0) 
    {
        free(list);
        return;
    }

    for(int i = 0; i < list->count; i++)
    {
        Move move = list->moves[i];
        if(FLAG(move) == ENPASSANT) NumberOfEnPassant[test][depth]++;
        Board b = *board;
        MakeMove(&b, move);
        CountMoves(&b, depth+1, maxDepth, test);
    }

    free(list);
}