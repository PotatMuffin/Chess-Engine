#ifndef PLAYER
#define PLAYER
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "input.h"
#include "move.h"
#include "negamax.h"

typedef enum {
    HUMAN,
    RANDOM,
    NEGA
} Player;

Move GetMove(Player player, Board *board, int offsetX, int offsetY)
{
    if(player == RANDOM)
    {
        MoveList *list = GenerateMoves(board);
        if (list->count == 0) return nullMove;
        srand(time(NULL));
        int r = rand()%list->count;
        Move move = list->moves[r];
        free(list);
        return move;
    }
    else if(player == NEGA)
    {
        if(!searching && bestMove) {
            Move move = bestMove;
            bestMove = nullMove;
            return move;
        }
        return nullMove;
    }
    else if(player == HUMAN)
    {
        return HandleInput(board, offsetX, offsetY);
    }
}

void StartTurn(Player player, Board *board, TranspositionTable *table)
{
    if(player == NEGA)
    {
        pthread_t thread_id;
        pthread_t abort_id;
        struct ThinkArgs *args = malloc(sizeof(struct ThinkArgs));
        args->board = board;
        args->table = table;
        args->depth = 100;
        pthread_create(&thread_id, NULL, (void * (*) (void*))Think, args);
        pthread_create(&abort_id, NULL, (void * (*) (void*))ScheduleAbort, (void *)1);
    }
}

#endif