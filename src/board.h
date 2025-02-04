#ifndef BOARD_IMPLEMENTATION
#define BOARD_IMPLEMENTATION
#include <stdint.h>
#include <stdbool.h>
#include "piece.h"

typedef uint64_t Bitboard;

typedef struct {
    char map[64];
    Bitboard pieceBoards[16];
    bool castleRights[4];
    int colourToMove;
    char enPassant;
} Board;

#include "move.h"
#include "fen.h"
void InitBoard(Board *board, char *fen)
{
    for(int i = 0; i < 64; i++) board->map[i] = NONE;
    for(int i = 0; i < 16; i++) board->pieceBoards[i] = 0ULL;
    for(int i = 0; i < 4;  i++) board->castleRights[i] = false;
    LoadFen(board, fen);
    

    for (int i = 0; i < 64; i++)
    {
        char piece = board->map[i];
        if (piece == NONE) continue;
        int index = piece-WHITE;
        board->pieceBoards[index] |= (Bitboard)1 << i;
        index = GetPieceColor(piece)-WHITE;
        board->pieceBoards[index] |= (Bitboard)1 << i;
    }
}

void MakeMove(Board *board, Move move) 
{
    board->enPassant = 0;
    char start = START(move);
    char target = TARGET(move);
    char flag = FLAG(move);
    char movedPiece = board->map[start];
    char capturedPiece = board->map[target];
    char pieceColour = board->colourToMove;
    char colourIndex = pieceColour>>4; 
    char enemyColour = pieceColour ^ COLORMASK;
    bool kingMoved = IsPieceType(movedPiece, KING);

    board->map[target] = movedPiece;
    board->map[start] = NONE;

    board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << start;
    board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
    board->pieceBoards[pieceColour-8] ^= (Bitboard)1 << start;
    board->pieceBoards[pieceColour-8] |= (Bitboard)1 << target;

    if (capturedPiece != NONE)
    {
        board->pieceBoards[capturedPiece-8] ^= (Bitboard)1 << target;
        board->pieceBoards[enemyColour-8] ^= (Bitboard)1 << target;
    }

    switch(flag)
    {
        case PROMOTEQUEEN:
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;
            movedPiece = pieceColour | QUEEN;
            board->map[target] = movedPiece;
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEKNIGHT:
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;
            movedPiece = pieceColour | KNIGHT;
            board->map[target] = movedPiece;
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEROOK:
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;
            movedPiece = pieceColour | ROOK;
            board->map[target] = movedPiece;
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEBISHOP:
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;
            movedPiece = pieceColour | BISHOP;
            board->map[target] = movedPiece;
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PAWN2FORDWARD:
            board->enPassant = (pieceColour == WHITE) ? target-8 : target+8;
            break;
        case ENPASSANT:
            char enPassant = (pieceColour == WHITE) ? target-8 : target+8;
            capturedPiece = PAWN | enemyColour;
            board->pieceBoards[capturedPiece-8] ^= (Bitboard)1 << enPassant;
            board->pieceBoards[enemyColour-8] ^= (Bitboard)1 << enPassant;
            board->map[enPassant] = NONE;
            break;
        case CASTLE:
            char rookPos;
            char rookTarget;
            char rook = ROOK | pieceColour;
            if (target == 58 || target == 2)
            {
                rookPos = target-2;
                rookTarget = target+1;
            }
            else if(target == 62 || target == 6)
            {
                rookPos = target+1;
                rookTarget = target-1;
            }
            board->pieceBoards[rook-8] ^= (Bitboard)1 << rookPos;
            board->pieceBoards[rook-8] |= (Bitboard)1 << rookTarget;
            board->pieceBoards[pieceColour-8] ^= (Bitboard)1 << rookPos;
            board->pieceBoards[pieceColour-8] |= (Bitboard)1 << rookTarget;
            board->map[rookTarget] = rook;
            board->map[rookPos] = NONE;
            break;
    }   

    board->castleRights[0] &= ~(target == 7 | start == 7);
    board->castleRights[1] &= ~(target == 0 | start == 0);
    board->castleRights[2] &= ~(target == 63 | start == 63);
    board->castleRights[3] &= ~(target == 56 | start == 56);
    board->castleRights[colourIndex<<1]   &= ~kingMoved;
    board->castleRights[(colourIndex<<1)+1] &= ~kingMoved;

    board->colourToMove = enemyColour;
}

#endif