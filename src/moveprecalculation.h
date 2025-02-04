#ifndef MOVECALC_IMPLEMENTATION
#define MOVECALC_IMPLEMENTATION

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

enum {
    NO = 0,
    EA = 1,
    SO = 2,
    WE = 3,
    NW = 4,
    NE = 5,
    SE = 6,
    SW = 7
};

const char pawnPromotionRank[2] = {7, 0};
const char directionOffsets[8] = {[NO] = 8, [EA] = 1, [SO] = -8, [WE] = -1, [NW] = 7, [NE] = 9, [SE] = -7, [SW] = -9};
const Bitboard fourthRank[2] = {0x00000000FF000000, 0x000000FF00000000 };
const Bitboard castleMasks[2][2] = {{0b00001110, 0b01100000}, {0b00001110UL << 56, 0b01100000UL << 56}};
const Bitboard castleSquares[2][2] = {{0b00000100, 0b01000000}, {0b00000100UL << 56, 0b01000000UL << 56}};
const Bitboard castleSquaresMask = castleSquares[0][0] | castleSquares[0][1] | castleSquares[1][0] | castleSquares[1][1];
Bitboard knightMoves[64] = {0};
Bitboard pawnAttacks[2][64] = {0};
Bitboard kingMoves[64] = {0};
Bitboard rookMasks[64] = {0};
Bitboard bishopMasks[64] = {0};
Bitboard fullRookMasks[64] = {0};
Bitboard fullBishopMasks[64] = {0};
Bitboard rookAttacks[64][4096] = {0};
Bitboard bishopAttacks[64][512] = {0};
Bitboard movesToEdge[64][8] = {0};
Bitboard slidingRays[64][8] = {0};
Bitboard alignMasks[64][64] = {0};

int rookBits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

int bishopBits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

Bitboard RookAttacks(int square, Bitboard blocks);
Bitboard BishopAttacks(int square, Bitboard blocks);
Bitboard IndexToBitboard(int index, Bitboard mask);
int GetIndex(Bitboard mask, Bitboard occupancy);
int CountPopulation(Bitboard board);

void CalculateMoveData()
{
    for(int file = 0; file < 8; file++)
    {
        for(int rank = 0; rank < 8; rank++)
        {
            int index = rank*8+file;

            // knight moves
            int targetRank = rank + 2;
            if (targetRank < 8)
            {
                if (file > 0) knightMoves[index] |= (Bitboard)1 << (targetRank*8+file-1); // cast 1 to bitboard to ensure the correct shift is used
                if (file < 7) knightMoves[index] |= (Bitboard)1 << (targetRank*8+file+1);
            }

            targetRank = rank - 2;
            if (targetRank >= 0)
            {
                if (file > 0) knightMoves[index] |= (Bitboard)1 << (targetRank*8+file-1);
                if (file < 7) knightMoves[index] |= (Bitboard)1 << (targetRank*8+file+1);
            }

            int targetFile = file + 2;
            if (targetFile < 8)
            {
                if (rank > 0) knightMoves[index] |= (Bitboard)1 << ((rank-1)*8+targetFile);
                if (rank < 7) knightMoves[index] |= (Bitboard)1 << ((rank+1)*8+targetFile);
            }

            targetFile = file - 2;
            if (targetFile >= 0)
            {
                if (rank > 0) knightMoves[index] |= (Bitboard)1 << ((rank-1)*8+targetFile);
                if (rank < 7) knightMoves[index] |= (Bitboard)1 << ((rank+1)*8+targetFile);
            }

            // pawn attacks
            if (rank > 0 && rank < 7)
            {
                if (file > 0) pawnAttacks[0][index] |= (Bitboard)1 << ((rank+1)*8+file-1);
                if (file < 7) pawnAttacks[0][index] |= (Bitboard)1 << ((rank+1)*8+file+1);
                if (file > 0) pawnAttacks[1][index] |= (Bitboard)1 << ((rank-1)*8+file-1);
                if (file < 7) pawnAttacks[1][index] |= (Bitboard)1 << ((rank-1)*8+file+1);
            }

            // king moves
            if (file > 0) 
            {
                kingMoves[index] |= (Bitboard)1 << (rank*8+file-1);
                if (rank < 7) kingMoves[index] |= (Bitboard)1 << ((rank+1)*8+file-1);
                if (rank > 0) kingMoves[index] |= (Bitboard)1 << ((rank-1)*8+file-1);
            }
            if (file < 7) 
            {
                kingMoves[index] |= (Bitboard)1 << (rank*8+file+1);
                if (rank < 7) kingMoves[index] |= (Bitboard)1 << ((rank+1)*8+file+1);
                if (rank > 0) kingMoves[index] |= (Bitboard)1 << ((rank-1)*8+file+1);
            }
            if (rank > 0) kingMoves[index] |= (Bitboard)1 << ((rank-1)*8+file);
            if (rank < 7) kingMoves[index] |= (Bitboard)1 << ((rank+1)*8+file);

            // rook moves
            for (int targetRank = rank + 1; targetRank < 7; targetRank++) rookMasks[index] |= (Bitboard)1 << (targetRank*8+file);
            for (int targetRank = rank - 1; targetRank > 0; targetRank--) rookMasks[index] |= (Bitboard)1 << (targetRank*8+file);
            for (int targetFile = file + 1; targetFile < 7; targetFile++) rookMasks[index] |= (Bitboard)1 << (rank*8+targetFile);
            for (int targetFile = file - 1; targetFile > 0; targetFile--) rookMasks[index] |= (Bitboard)1 << (rank*8+targetFile);

            for(int i = 0; i < (1 << rookBits[index]); i++) 
            {
                Bitboard block = IndexToBitboard(i, rookMasks[index]);
                rookAttacks[index][i] = RookAttacks(index, block);
            }

            // bishop moves
            for (int targetRank = rank + 1, targetFile = file - 1; targetRank < 7 && targetFile > 0; targetRank++, targetFile--) 
                bishopMasks[index] |= (Bitboard)1 << (targetRank*8+targetFile);
            for (int targetRank = rank + 1, targetFile = file + 1; targetRank < 7 && targetFile < 7; targetRank++, targetFile++) 
                bishopMasks[index] |= (Bitboard)1 << (targetRank*8+targetFile);
            for (int targetRank = rank - 1, targetFile = file - 1; targetRank > 0 && targetFile > 0; targetRank--, targetFile--) 
                bishopMasks[index] |= (Bitboard)1 << (targetRank*8+targetFile);
            for (int targetRank = rank - 1, targetFile = file + 1; targetRank > 0 && targetFile < 7; targetRank--, targetFile++) 
                bishopMasks[index] |= (Bitboard)1 << (targetRank*8+targetFile);

            for(int i = 0; i < 1 << (bishopBits[index]); i++)
            {
                Bitboard block = IndexToBitboard(i, bishopMasks[index]);
                bishopAttacks[index][i] = BishopAttacks(index, block);
            }
            
            int north = 7 - rank;
            int east  = 7 - file;
            int south = rank;
            int west  = file;

            movesToEdge[index][NO] = north;
            movesToEdge[index][EA] = east;
            movesToEdge[index][SO] = south;
            movesToEdge[index][WE] = west;
            movesToEdge[index][NW] = (north > west) ? west : north;
            movesToEdge[index][NE] = (north > east) ? east : north;
            movesToEdge[index][SE] = (south > east) ? east : south;
            movesToEdge[index][SW] = (south > west) ? west : south; 

            for(int i = 0; i < 8; i++)
            {
                int offset = directionOffsets[i];
                for(int x = 1; x <= movesToEdge[index][i]; x++)
                {
                    slidingRays[index][i] |= 1ULL << index + x * offset;
                }
            }

            for(int dir = 0; dir < 8; dir++)
            {
                int offset = directionOffsets[dir];
                int n = movesToEdge[index][dir];

                for (int j = 0; j <= n; j++)
                {
                    int target = index + offset * j; 
                    alignMasks[index][target] = slidingRays[index][dir];
                }
            }
        }
    }
}

Bitboard RookAttacks(int square, Bitboard blocks)
{
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = 0;
    for (int targetRank = rank + 1; targetRank <= 7; targetRank++) 
    { 
        attacks |= (Bitboard)1 << (targetRank*8+file); 
        if(blocks & ((Bitboard)1 << (targetRank*8+file))) break; 
    }
    for (int targetRank = rank - 1; targetRank >= 0; targetRank--) 
    { 
        attacks |= (Bitboard)1 << (targetRank*8+file); 
        if(blocks & ((Bitboard)1 << (targetRank*8+file))) break; 
    }
    for (int targetFile = file + 1; targetFile <= 7; targetFile++) 
    { 
        attacks |= (Bitboard)1 << (rank*8+targetFile); 
        if(blocks & ((Bitboard)1 << (rank*8+targetFile))) break; 
    }
    for (int targetFile = file - 1; targetFile >= 0; targetFile--) 
    { 
        attacks |= (Bitboard)1 << (rank*8+targetFile); 
        if(blocks & ((Bitboard)1 << (rank*8+targetFile))) break; 
    }
    return attacks;
}

Bitboard BishopAttacks(int square, Bitboard blocks)
{
    Bitboard attacks = 0;
    int rank = square / 8;
    int file = square % 8;
    for (int targetRank = rank + 1, targetFile = file - 1; targetRank <= 7 && targetFile >= 0; targetRank++, targetFile--)
    {
        attacks |= (Bitboard)1 << (targetRank*8+targetFile);
        if(blocks & ((Bitboard)1 << (targetRank*8+targetFile))) break;
    }
    for (int targetRank = rank + 1, targetFile = file + 1; targetRank <= 7 && targetFile <= 7; targetRank++, targetFile++)
    {
        attacks |= (Bitboard)1 << (targetRank*8+targetFile);
        if(blocks & ((Bitboard)1 << (targetRank*8+targetFile))) break;
    }
    for (int targetRank = rank - 1, targetFile = file - 1; targetRank >= 0 && targetFile >= 0; targetRank--, targetFile--)
    {
        attacks |= (Bitboard)1 << (targetRank*8+targetFile);
        if(blocks & ((Bitboard)1 << (targetRank*8+targetFile))) break;
    }
    for (int targetRank = rank - 1, targetFile = file + 1; targetRank >= 0 && targetFile <= 7; targetRank--, targetFile++)
    {
        attacks |= (Bitboard)1 << (targetRank*8+targetFile);
        if(blocks & ((Bitboard)1 << (targetRank*8+targetFile))) break;
    }
    return attacks;
}

inline Bitboard IndexToBitboard(int index, Bitboard mask)
{
    return _pdep_u64(index, mask);
}

inline int GetIndex(Bitboard mask, Bitboard occupancy)
{
    return _pext_u64(occupancy, mask);
}

inline int CountPopulation(Bitboard board)
{
    return __builtin_popcountll(board);
}

#endif