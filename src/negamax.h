#ifndef NEGAMAX
#define NEGAMAX

#include "transpositiontable.h"
#include "movegeneration.h"
#include "eval.h"
#include "board.h"
#include "move.h"

static Move bestMove = nullMove;
static const int infinity = 9999999;
static const int negInfinity = -infinity;
static TranspositionTable *transpositionTable;
static int nodes = 0;
bool searching;
bool aborted;

struct ThinkArgs {
    Board *board;
    TranspositionTable *table;
    int depth;
    int seconds;
};

int Search(Board *board, int rootDistance, int depth, int alpha, int beta);
int IterativeSearch(Board *board, int maxDepth);
void Think(struct ThinkArgs *args);

void Think(struct ThinkArgs *args)
{
    searching = true;
    transpositionTable = args->table;
    Board *board = args->board;
    int depth = args->depth;
    bestMove = nullMove;
    free(args);

    nodes = 0;
    aborted = false;
    // int eval = Search(board,depth,negInfinity,infinity);
    IterativeSearch(board, depth);
    printf("nodes searched: %d\n", nodes);
    searching = false;
}

int Search(Board *board, int rootDistance, int depth, int alpha, int beta)
{
    if(aborted) return 0;
    nodes++;
    Transposition *tp = GetTranspositionEntry(transpositionTable, depth, rootDistance, board->zobristKey);
    if(tp != NULL)
    {
        if(tp->type == PV) return tp->value;
        else if(tp->type == CUT && tp->value >= beta) return tp->value;
        else if(tp->type == ALL && tp->value < alpha) return tp->value;
    }

    if(depth == 0) return Eval(board);

    Move preferredMove = nullMove;
    int bestEval = negInfinity;
    MoveList *list = GenerateMoves(board);
    if(list->count == 0)
    {
        free(list);
        if(checks > 0) return -(mateScore - rootDistance);
        else return 0;
    }
    SortMoveList(list, board, transpositionTable, depth);

    for(int i = 0; i < list->count; i++)
    {
        Board b = *board;
        Move move = list->moves[i];
        MakeMove(&b, move);
        int eval = -Search(&b, rootDistance+1, depth-1, -beta, -alpha);
        if (eval > bestEval) 
        {
            bestEval = eval;
            preferredMove = move;
            if (eval > alpha) alpha = eval;
        }
        if (eval >= beta) 
        {
            AddTransposition(transpositionTable, depth, rootDistance, board->zobristKey, move, beta, CUT);
            free(list);
            return bestEval; 
        }
    }

    if(!aborted && rootDistance == 0) 
    {
        bestMove = preferredMove;
        AddTransposition(transpositionTable, depth, rootDistance, board->zobristKey, bestMove, alpha, (bestEval <= alpha) ? ALL : PV);
    }

    free(list);
    return bestEval;
}

int IterativeSearch(Board *board, int maxDepth)
{
    for(int i = 1; i < maxDepth+1; i++)
    {
        int eval = Search(board, 0, i, negInfinity, infinity);
        if (aborted) break;
        printf("depth %d completed, score = %d\n", i, eval);
    }
}

void ScheduleAbort(uint64_t seconds)
{
    aborted = false;
    usleep(seconds*1000000);
    aborted = true;
}

#endif