#ifndef NEGAMAX_IMPLEMENTATION
#define NEGAMAX_IMPLEMENTATION

#include "movegeneration.h"
#include "eval.h"
#include "board.h"
#include "move.h"

static Move bestMove = nullMove;
static const int infinity = 999999999;
static const int negInfinity = -infinity;
bool searching;

struct ThinkArgs {
    Board *board;
    int depth;
};

int Search(Board *board, int depth);
void Think(struct ThinkArgs *args);

void Think(struct ThinkArgs *args)
{
    searching = true;
    Board *board = args->board;
    int depth = args->depth;
    bestMove = nullMove;
    int eval = Search(board,depth);
    searching = false;
}

int Search(Board *board, int depth)
{
    if(depth == 0) return Eval(board);

    Move preferredMove = nullMove;
    int bestEval = negInfinity;
    MoveList *list = GenerateMoves(board);

    for(int i = 0; i < list->count; i++)
    {
        Board b = *board;
        Move move = list->moves[i];
        MakeMove(&b, move);
        int eval = -Search(&b, depth-1);
        if (eval > bestEval) 
        {
            bestEval = eval;
            preferredMove = move;
        }
    }

    bestMove = preferredMove;
    return bestEval;
}

#endif