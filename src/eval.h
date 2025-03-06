#ifndef EVAL
#define EVAL
#include <math.h>
#include "board.h"
#include "moveprecalculation.h"
#include "piece.h"

static inline int EvaluatePieceSquareTables(double endgameTWhite, double endgameTBlack);
static inline int EvaluatePieceSquareTable(enum pieceSquareTableIndex table, Bitboard pieces, bool isWhite);

const int mateScore = 1000000;
const int endgameStart = values[ROOK]*2+values[BISHOP]+values[KNIGHT];

static Bitboard whitePawns;   
static Bitboard whiteBishops; 
static Bitboard whiteKnights; 
static Bitboard whiteRooks;   
static Bitboard whiteQueens;
static Bitboard whiteKing; 
static Bitboard blackPawns;   
static Bitboard blackBishops; 
static Bitboard blackKnights; 
static Bitboard blackRooks;   
static Bitboard blackQueens;  
static Bitboard blackKing;

int Eval(Board *board)
{
    int value = 0;
    int whiteMaterial;
    int blackMaterial;
    int colourToMove = board->colourToMove;
    int colourIndex  = colourToMove >> 4;

    whitePawns   = board->pieceBoards[(PAWN   | WHITE)-8];
    whiteBishops = board->pieceBoards[(BISHOP | WHITE)-8];
    whiteKnights = board->pieceBoards[(KNIGHT | WHITE)-8];
    whiteRooks   = board->pieceBoards[(ROOK   | WHITE)-8];
    whiteQueens  = board->pieceBoards[(QUEEN  | WHITE)-8];
    whiteKing    = board->pieceBoards[(KING   | WHITE)-8];
    blackPawns   = board->pieceBoards[(PAWN   | BLACK)-8];
    blackBishops = board->pieceBoards[(BISHOP | BLACK)-8];
    blackKnights = board->pieceBoards[(KNIGHT | BLACK)-8];
    blackRooks   = board->pieceBoards[(ROOK   | BLACK)-8];
    blackQueens  = board->pieceBoards[(QUEEN  | BLACK)-8];
    blackKing    = board->pieceBoards[(KING   | BLACK)-8];

    int nWhitePawns   = CountPopulation(whitePawns);
    int nWhiteBishops = CountPopulation(whiteBishops);
    int nWhiteKnights = CountPopulation(whiteKnights);
    int nWhiteRooks   = CountPopulation(whiteRooks);
    int nWhiteQueens  = CountPopulation(whiteQueens);
    int nBlackPawns   = CountPopulation(blackPawns);
    int nBlackBishops = CountPopulation(blackBishops);
    int nBlackKnights = CountPopulation(blackKnights);
    int nBlackRooks   = CountPopulation(blackRooks);
    int nBlackQueens  = CountPopulation(blackQueens);

    whiteMaterial += nWhitePawns   * values[PAWN];
    whiteMaterial += nWhiteBishops * values[BISHOP];   
    whiteMaterial += nWhiteKnights * values[KNIGHT];   
    whiteMaterial += nWhiteRooks   * values[ROOK];   
    whiteMaterial += nWhiteQueens  * values[QUEEN];
    blackMaterial += nBlackPawns   * values[PAWN];
    blackMaterial += nBlackBishops * values[BISHOP];   
    blackMaterial += nBlackKnights * values[KNIGHT];   
    blackMaterial += nBlackRooks   * values[ROOK];   
    blackMaterial += nBlackQueens  * values[QUEEN];  

    double endgameTWhite = 1 - fmin(1, (double)whiteMaterial / (double)endgameStart);
    double endgameTBlack = 1 - fmin(1, (double)blackMaterial / (double)endgameStart);
    
    value = whiteMaterial - blackMaterial;
    value += EvaluatePieceSquareTables(endgameTWhite, endgameTBlack);

    if(colourToMove == WHITE) return value;
    else return -value;
}

static inline int EvaluatePieceSquareTables(double endgameTWhite, double endgameTBlack)
{
    int value = 0;
    value += EvaluatePieceSquareTable(TableKnight, whiteKnights, true);
    value += EvaluatePieceSquareTable(TableBishop, whiteBishops, true);
    value += EvaluatePieceSquareTable(TableRook,   whiteRooks,   true);
    value += EvaluatePieceSquareTable(TableQueen,  whiteQueens,  true);

    value -= EvaluatePieceSquareTable(TableKnight, blackKnights, false);
    value -= EvaluatePieceSquareTable(TableBishop, blackBishops, false);
    value -= EvaluatePieceSquareTable(TableRook,   blackRooks,   false);
    value -= EvaluatePieceSquareTable(TableQueen,  blackQueens,  false);

    int whitePawnValueStart = EvaluatePieceSquareTable(TablePawnStart, whitePawns, true);
    int whitePawnValueEnd   = EvaluatePieceSquareTable(TablePawnEnd,   whitePawns, true);
    int whiteKingValueStart = EvaluatePieceSquareTable(TableKingStart, whiteKing,  true);
    int whiteKingValueEnd   = EvaluatePieceSquareTable(TableKingEnd,   whiteKing,  true);
    value += (double)whitePawnValueStart * (1.0f - endgameTBlack);
    value += (double)whitePawnValueEnd   * endgameTBlack;
    value += (double)whiteKingValueStart * (1.0f - endgameTBlack);
    value += (double)whiteKingValueEnd   * endgameTBlack;

    int blackPawnValueStart = EvaluatePieceSquareTable(TablePawnStart, blackPawns, false);
    int blackPawnValueEnd   = EvaluatePieceSquareTable(TablePawnEnd,   blackPawns, false);
    int blackKingValueStart = EvaluatePieceSquareTable(TableKingStart, blackKing,  false);
    int blackKingValueEnd   = EvaluatePieceSquareTable(TableKingEnd,   blackKing,  false);
    value -= (double)blackPawnValueStart * (1.0f - endgameTWhite);
    value -= (double)blackPawnValueEnd   * endgameTWhite;
    value -= (double)blackKingValueStart * (1.0f - endgameTWhite);
    value -= (double)blackKingValueEnd   * endgameTWhite;
    return value;
}

static inline int EvaluatePieceSquareTable(enum pieceSquareTableIndex table, Bitboard pieces, bool isWhite)
{
    int value = 0;
    while(pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        value += ReadTable(table, piecePos, isWhite);
    }
    return value;
}

#endif