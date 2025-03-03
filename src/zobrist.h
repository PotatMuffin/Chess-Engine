#ifndef ZOBRIST
#define ZOBRIST
#include <stdlib.h>

typedef struct {
    uint64_t pieces[64][16];
    uint64_t enPassant[8];
    uint64_t castleRights[4];
    uint64_t blackToMove;
} Zobrist;

uint64_t rand64()
{
    uint64_t value = 0;
    for(int i = 0; i < 64; i++)
    {
        int r = rand()%2;
        value |= r << i;
    }
    return value;
}

void InitZobristKeys(Zobrist *zobrist)
{
    srand(1704256515);
    for(int i = 0; i < 64; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            zobrist->pieces[i][j] = rand64();
        }
    }

    for(int i = 0; i < 8; i++)
    {
        zobrist->enPassant[i] = rand64();
    }

    for(int i = 0; i < 4; i++)
    {
        zobrist->castleRights[i] = rand64();
    }
    zobrist->blackToMove = rand64();
}

#endif  