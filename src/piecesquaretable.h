#ifndef PIECESQUARETABLE
#define PIECESQUARETABLE
#include "piece.h"

static const int pawnSquares[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    50,  50,  50,  50,  50,  50,  50,  50,
    10,  10,  20,  30,  30,  20,  10,  10,
    5,   5,  10,  25,  25,  10,   5,   5,
    0,   0,   0,  20,  20,   0,   0,   0,
    5,  -5, -10,   0,   0, -10,  -5,   5,
    5,  10,  10, -20, -20,  10,  10,   5,
    0,   0,   0,   0,   0,   0,   0,   0
};

static const int pawnSquaresEnd[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    80,  80,  80,  80,  80,  80,  80,  80,
    50,  50,  50,  50,  50,  50,  50,  50,
    30,  30,  30,  30,  30,  30,  30,  30,
    20,  20,  20,  20,  20,  20,  20,  20,
    10,  10,  10,  10,  10,  10,  10,  10,
    10,  10,  10,  10,  10,  10,  10,  10,
    0,   0,   0,   0,   0,   0,   0,   0
};

static const int rookSquares[64] =  {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};
static const int knightSquares[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};
static const int bishopSquares[64] =  {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};
static const int queenSquares[64] =  {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,   0,  5,  5,  5,  5,  0, -5,
    0,    0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
static const int kingSquaresStart[64] = 
{
    -80, -70, -70, -70, -70, -70, -70, -80, 
    -60, -60, -60, -60, -60, -60, -60, -60, 
    -40, -50, -50, -60, -60, -50, -50, -40, 
    -30, -40, -40, -50, -50, -40, -40, -30, 
    -20, -30, -30, -40, -40, -30, -30, -20, 
    -10, -20, -20, -20, -20, -20, -20, -10, 
    20,  20,  -5,  -5,  -5,  -5,  20,  20, 
    20,  30,  10,   0,   0,  10,  30,  20
};

static const int KingSquaresEnd[64] = 
{
    -20, -10, -10, -10, -10, -10, -10, -20,
    -5,   0,   5,   5,   5,   5,   0,  -5,
    -10, -5,   20,  30,  30,  20,  -5, -10,
    -15, -10,  35,  45,  45,  35, -10, -15,
    -20, -15,  30,  40,  40,  30, -15, -20,
    -25, -20,  20,  25,  25,  20, -20, -25,
    -30, -25,   0,   0,   0,   0, -25, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
};

enum pieceSquareTableIndex 
{
    TableKingEnd   = 0b000, // KING - 1
    TableKingStart = 0b001, // KING
    TablePawnStart = 0b010, // PAWN
    TableKnight    = 0b011, // KNIGHT
    TablePawnEnd   = 0b100, // KNIGHT + 1
    TableBishop    = 0b101, // BISHOP
    TableRook      = 0b110, // ROOK
    TableQueen     = 0b111, // QUEEN
};

int pieceSquareTables[64][8] = {0};

void InitPieceSquareTable()
{
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableKingStart] = kingSquaresStart[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableKingEnd] = KingSquaresEnd[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableKnight] = knightSquares[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableBishop] = bishopSquares[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableRook] = rookSquares[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TableQueen] = queenSquares[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TablePawnStart] = pawnSquares[i];
    for(int i = 0; i < 64; i++) pieceSquareTables[i][TablePawnEnd] = pawnSquaresEnd[i];
}

inline int ReadTable(char pieceType, char square, bool isWhite)
{
    if(isWhite)
    {
        int file = square % 8;
        int rank = square / 8;
        rank = 7 - rank;
        square = rank * 8 + file;
    } 
    return pieceSquareTables[square][pieceType];
}

#endif