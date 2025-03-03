#ifndef PIECE
#define PIECE

#define NONE   0b00000000
#define KING   0b00000001
#define PAWN   0b00000010
#define KNIGHT 0b00000011
#define BISHOP 0b00000101
#define ROOK   0b00000110
#define QUEEN  0b00000111
#define WHITE  0b00001000
#define BLACK  0b00010000

#define COLORMASK 0b00011000
#define PIECEMASK 0b00000111

const int values[8] = {[PAWN] = 100, [KNIGHT] = 300, [BISHOP] = 300, [ROOK] = 500, [QUEEN] = 900};

inline bool IsColour(char piece, char colour)
{
    return (piece & COLORMASK) == colour;
}

inline char GetPieceColour(char piece)
{
    return (piece & COLORMASK);
}

inline bool IsPieceType(char piece, char pieceType)
{
    return (piece & PIECEMASK) == pieceType;
}

inline bool IsQueenOrBishop(char piece)
{
    return (piece & BISHOP) == BISHOP;
}

inline bool IsQueenOrRook(char piece)
{
    return (piece & ROOK) == ROOK;
}

inline char GetPieceType(char piece)
{
    return (piece & PIECEMASK);
}

#endif