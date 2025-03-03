#ifndef MOVESORT
#define MOVESORT
#include "move.h"
#include "piece.h"
#include "movegeneration.h"
#include "transpositiontable.h"

static const int million = 1000000;
static const int tpMoveWeight = 8*million; 

void SortMoveList(MoveList *list, Board *board, TranspositionTable *table, int depth)
{
    int moveValues[256] = {0};
    Transposition *entry = GetTranspositionEntry(table, depth, 0, board->zobristKey);
    Move tpMove = nullMove;

    if(entry != NULL) tpMove = entry->move;

    for(int i = 0; i < list->count; i++)
    {
        Move move = list->moves[i];
        char start = START(move);
        char target = TARGET(move);

        char piece = board->map[start];
        char pieceType = GetPieceType(piece);
        char capturedPieceType = GetPieceType(board->map[target]);

        if (move == tpMove) moveValues[i] += tpMoveWeight;
        if(capturedPieceType)
        {
            int delta = values[capturedPieceType] - values[pieceType];
            bool canRecapture = (enemyAttacks & ((Bitboard)1 << target)) != 0;
            if(canRecapture)
            {
                moveValues[i] += ((delta >= 0) ? 1000000 : -1000000) + delta;
            }
            else moveValues[i] += 1000000 + delta;
        }
    }

    for(int i = 0; i < list->count; i++)
    {
        for(int j = 1; j < list->count; j++)
        {
            int swap = j - 1;
            Move tempMove = list->moves[j];
            int tempValue = moveValues[j];

            if(moveValues[swap] < moveValues[j])
            {
                moveValues[j] = moveValues[swap];
                moveValues[swap] = tempValue;
                list->moves[j] = list->moves[swap];
                list->moves[swap] = tempMove;
            }
        }
    }
}

#endif