#ifndef EVAL
#define EVAL
#include "board.h"
#include "moveprecalculation.h"
#include "piece.h"

const int mateScore = 1000000;
const int endgameStart = values[ROOK]*2+values[BISHOP]+values[PAWN]*6;

int Eval(Board *board)
{
    int value = 0;
    int whiteValue;
    int blackValue;
    int colourToMove = board->colourToMove;
    int colourIndex  = colourToMove >> 4;

    Bitboard whitePawns   = board->pieceBoards[(PAWN   | WHITE)-8];
    Bitboard whiteBishops = board->pieceBoards[(BISHOP | WHITE)-8];
    Bitboard whiteKnights = board->pieceBoards[(KNIGHT | WHITE)-8];
    Bitboard whiteRooks   = board->pieceBoards[(ROOK   | WHITE)-8];
    Bitboard whiteQueens  = board->pieceBoards[(QUEEN  | WHITE)-8];
    Bitboard blackPawns   = board->pieceBoards[(PAWN   | BLACK)-8];
    Bitboard blackBishops = board->pieceBoards[(BISHOP | BLACK)-8];
    Bitboard blackKnights = board->pieceBoards[(KNIGHT | BLACK)-8];
    Bitboard blackRooks   = board->pieceBoards[(ROOK   | BLACK)-8];
    Bitboard blackQueens  = board->pieceBoards[(QUEEN  | BLACK)-8];

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

    whiteValue += nWhitePawns   * values[PAWN];
    whiteValue += nWhiteBishops * values[BISHOP];   
    whiteValue += nWhiteKnights * values[KNIGHT];   
    whiteValue += nWhiteRooks   * values[ROOK];   
    whiteValue += nWhiteQueens  * values[QUEEN];
    blackValue += nBlackPawns   * values[PAWN];
    blackValue += nBlackBishops * values[BISHOP];   
    blackValue += nBlackKnights * values[KNIGHT];   
    blackValue += nBlackRooks   * values[ROOK];   
    blackValue += nBlackQueens  * values[QUEEN];  

    for(Bitboard pieces = board->pieceBoards[0]; pieces;)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        char piece = board->map[piecePos];
        whiteValue += ReadTable(GetPieceType(piece), piecePos, true);
    }

    for(Bitboard pieces = board->pieceBoards[8]; pieces;)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        char piece = board->map[piecePos];
        blackValue += ReadTable(GetPieceType(piece), piecePos, false);
    }

    value = whiteValue - blackValue;
    if(colourToMove == WHITE) return value;
    else return -value;
}

#endif