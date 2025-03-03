#ifndef MOVE_IMPLEMENTATION
#define MOVE_IMPLEMENTATION

#include <stdint.h>
#include "piece.h"

enum MoveFlag {
    NOFLAG = 0,
    PAWN2FORDWARD = 1,
    ENPASSANT,
    PROMOTEKNIGHT,
    PROMOTEBISHOP,
    PROMOTEROOK,
    PROMOTEQUEEN, 
    CASTLE
};

typedef uint16_t Move;
typedef struct {
    Move moves[256];
    int count;
} MoveList;

const Move nullMove = 0;

#define MOVE(start, target, flag) (start | target << 6 | flag << 12)
#define START(move)  (move & 0b00111111)
#define TARGET(move) ((move >> 6) & 0b00111111)
#define FLAG(move) (move >> 12)

#endif