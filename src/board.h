#ifndef BOARD
#define BOARD
#include <stdint.h>
#include <stdbool.h>
#include "piece.h"
#include "piecesquaretable.h"
#include "zobrist.h"

typedef uint64_t Bitboard;

typedef struct {
    char map[64];
    Bitboard pieceBoards[16];
    uint64_t zobristKey;
    bool castleRights[4];
    int colourToMove;
    char enPassant;
} Board;

Zobrist zobrist = {0};

#include "move.h"
#include "fen.h"
void InitBoard(Board *board, char *fen)
{
    for(int i = 0; i < 64; i++) board->map[i] = NONE;
    for(int i = 0; i < 16; i++) board->pieceBoards[i] = 0ULL;
    for(int i = 0; i < 4;  i++) board->castleRights[i] = false;
    board->zobristKey = 0;
    LoadFen(board, fen);
    
    for (int i = 0; i < 64; i++)
    {
        char piece = board->map[i];
        char pieceType = GetPieceType(piece);
        char pieceColour = GetPieceColour(piece);
        if (piece == NONE) continue;
        int index = piece-WHITE;
        board->pieceBoards[index] |= (Bitboard)1 << i;
        board->zobristKey ^= zobrist.pieces[i][index];

        index = pieceColour-WHITE;
        board->pieceBoards[index] |= (Bitboard)1 << i;
    }

    for(int i = 0; i < 4;  i++) if (board->castleRights[i]) board->zobristKey ^= zobrist.castleRights[i];
    if(board->enPassant) board->zobristKey ^= zobrist.enPassant[board->enPassant%8];
    if(board->colourToMove == BLACK) board->zobristKey ^= zobrist.blackToMove;
}

void MakeMove(Board *board, Move move) 
{
    if(board->enPassant) board->zobristKey ^= zobrist.enPassant[board->enPassant%8];
    board->enPassant = 0;
    char pieceColour = board->colourToMove;
    bool isWhite = pieceColour == WHITE;
    int perspective = (isWhite) ? 1 : -1;

    char start = START(move);
    char target = TARGET(move);
    char flag = FLAG(move);

    char movedPiece = board->map[start];
    char pieceType = GetPieceType(movedPiece);
    char capturedPiece = board->map[target];
    char capturedPieceType = GetPieceType(capturedPiece);
    char colourIndex = pieceColour>>4; 
    char enemyColour = pieceColour ^ COLORMASK;
    bool kingMoved = IsPieceType(movedPiece, KING);

    board->map[target] = movedPiece;
    board->map[start] = NONE;

    board->zobristKey ^= zobrist.pieces[start][movedPiece-WHITE];
    board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];

    board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << start;
    board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
    board->pieceBoards[pieceColour-8] ^= (Bitboard)1 << start;
    board->pieceBoards[pieceColour-8] |= (Bitboard)1 << target;

    if (capturedPiece != NONE)
    {
        board->zobristKey ^= zobrist.pieces[target][capturedPiece-WHITE];
        board->pieceBoards[capturedPiece-8] ^= (Bitboard)1 << target;
        board->pieceBoards[enemyColour-8] ^= (Bitboard)1 << target;
    }

    switch(flag)
    {
        case PROMOTEQUEEN:
            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;

            movedPiece = pieceColour | QUEEN;
            board->map[target] = movedPiece;

            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEKNIGHT:
            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;

            movedPiece = pieceColour | KNIGHT;
            board->map[target] = movedPiece;

            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEROOK:
            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;

            movedPiece = pieceColour | ROOK;
            board->map[target] = movedPiece;

            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PROMOTEBISHOP:
            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] ^= (Bitboard)1 << target;

            movedPiece = pieceColour | BISHOP;
            board->map[target] = movedPiece;

            board->zobristKey ^= zobrist.pieces[target][movedPiece-WHITE];
            board->pieceBoards[movedPiece-8] |= (Bitboard)1 << target;
            break;
        case PAWN2FORDWARD:
            board->enPassant = (pieceColour == WHITE) ? target-8 : target+8;
            board->zobristKey ^= zobrist.enPassant[board->enPassant%8];
            break;
        case ENPASSANT:
            char enPassant = (pieceColour == WHITE) ? target-8 : target+8;
            capturedPiece = PAWN | enemyColour;

            board->zobristKey ^= zobrist.pieces[enPassant][capturedPiece-WHITE];
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

            board->zobristKey ^= zobrist.pieces[rookPos][rook-WHITE];
            board->zobristKey ^= zobrist.pieces[rookTarget][rook-WHITE];
            board->pieceBoards[rook-8] ^= (Bitboard)1 << rookPos;
            board->pieceBoards[rook-8] |= (Bitboard)1 << rookTarget;
            board->pieceBoards[pieceColour-8] ^= (Bitboard)1 << rookPos;
            board->pieceBoards[pieceColour-8] |= (Bitboard)1 << rookTarget;
            board->map[rookTarget] = rook;
            board->map[rookPos] = NONE;
            break;
    }   

    if(board->castleRights[0] && (target == 7 || start == 7))
    {
        board->castleRights[0] = false;
        board->zobristKey ^= zobrist.castleRights[0];
    }
    if(board->castleRights[1] && (target == 0 || start == 0))
    {
        board->castleRights[1] = false;
        board->zobristKey ^= zobrist.castleRights[1];
    }
    if(board->castleRights[2] && (target == 63 || start == 63))
    {
        board->castleRights[2] = false;
        board->zobristKey ^= zobrist.castleRights[2];
    }
    if(board->castleRights[3] && (target == 56 || start == 56))
    {
        board->castleRights[3] = false;
        board->zobristKey ^= zobrist.castleRights[3];
    }
    if(board->castleRights[colourIndex<<1]     && kingMoved)
    {
        board->castleRights[colourIndex<<1] = false;
        board->zobristKey ^= zobrist.castleRights[colourIndex<<1];
    }
    if(board->castleRights[(colourIndex<<1)+1] && kingMoved)
    {
        board->castleRights[(colourIndex<<1)+1] = false;
        board->zobristKey ^= zobrist.castleRights[(colourIndex<<1)+1];
    }

    board->zobristKey ^= zobrist.blackToMove;
    board->colourToMove = enemyColour;
}

#endif