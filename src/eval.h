#ifndef EVAL_IMPLEMENTATION
#define EVAL_IMPLEMENTATION
#include "board.h"
#include "moveprecalculation.h"
#include "piece.h"

const int pawnValue   = 100;
const int knightValue = 300;
const int bishopValue = 350;
const int rookValue   = 500;
const int queenValue  = 900;

int Eval(Board *board)
{
    int value = 0;
    int whiteValue;
    int blackValue;
    int colourToMove = board->colourToMove;
    int colourIndex  = colourToMove >> 4;

    int whitePawns   = CountPopulation(board->pieceBoards[(PAWN   | WHITE)-8]);
    int whiteBishops = CountPopulation(board->pieceBoards[(BISHOP | WHITE)-8]);
    int whiteKnights = CountPopulation(board->pieceBoards[(KNIGHT | WHITE)-8]);
    int whiteRooks   = CountPopulation(board->pieceBoards[(ROOK   | WHITE)-8]);
    int whiteQueens  = CountPopulation(board->pieceBoards[(QUEEN  | WHITE)-8]);
    int blackPawns   = CountPopulation(board->pieceBoards[(PAWN   | BLACK)-8]);
    int blackBishops = CountPopulation(board->pieceBoards[(BISHOP | BLACK)-8]);
    int blackKnights = CountPopulation(board->pieceBoards[(KNIGHT | BLACK)-8]);
    int blackRooks   = CountPopulation(board->pieceBoards[(ROOK   | BLACK)-8]);
    int blackQueens  = CountPopulation(board->pieceBoards[(QUEEN  | BLACK)-8]);

    whiteValue += whitePawns   * pawnValue;
    whiteValue += whiteBishops * bishopValue;   
    whiteValue += whiteKnights * knightValue;   
    whiteValue += whiteRooks   * rookValue;   
    whiteValue += whiteQueens  * queenValue;
    blackValue += blackPawns   * pawnValue;
    blackValue += blackBishops * bishopValue;   
    blackValue += blackKnights * knightValue;   
    blackValue += blackRooks   * rookValue;   
    blackValue += blackQueens  * queenValue;  
    value = whiteValue - blackValue;

    if(colourToMove == WHITE) return value;
    else return -value;
}

#endif