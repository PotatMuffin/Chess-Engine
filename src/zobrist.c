// this is used to find a seed where there are no collisions
// although it seems like a lot of seeds just don't generate collisions xD

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

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

int main()
{
    Zobrist zobrist = {0};

    for(int _ = 0; _ < 1000000; _++)
    {
        bool collision = false;
        srand(time(NULL));
        uint64_t seed = rand();
        srand(seed);
        for(int i = 0; i < 64 && !collision; i++)
        {
            for(int j = 0; j < 16 && !collision; j++)
            {
                zobrist.pieces[i][j] = rand();

                for(int i2 = 0; i2 < i+1 && i2 < 64 && !collision; i2++)
                {
                    for(int j2 = 0; j2 < j && !collision; j2++)
                    {
                        if(j2 == j) continue;
                        if(zobrist.pieces[i][j] == zobrist.pieces[i2][j2]) collision = true;
                    }
                }
            }
        }
        for(int i = 0; i < 8 && !collision; i++)
        {
            zobrist.enPassant[i] = rand();
            for(int i2 = 0; i2 < i+1 && i2 < 8 && !collision; i2++)
            {
                if(i2 == i) continue;
                if(zobrist.enPassant[i] == zobrist.enPassant[i2]) collision = true;
            }
        }
        for(int i = 0; i < 4 && !collision; i++)
        {
            zobrist.castleRights[i] = rand();
            for(int i2 = 0; i2 < i+1 && i2 < 4 && !collision; i2++)
            {
                if(i2 == i) continue;
                if(zobrist.castleRights[i] == zobrist.castleRights[i2]) collision = true;

            }
        }
        if(!collision) 
        {
            zobrist.blackToMove = rand();
            printf("found no collisions with seed: %lu\n", seed);
            break;
        }
        else printf("found a collision\n");
    }
}