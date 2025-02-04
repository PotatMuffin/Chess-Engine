#ifndef MOVEGEN_IMPLEMENTATION
#define MOVEGEN_IMPLEMENTATION
#include <malloc.h>
#include <x86intrin.h>
#include "moveprecalculation.h"
#include "move.h"
#include "board.h"
#include "piece.h"

char checks;
static char friendColour;
static char friendindex;
static char enemyColour;
static char enemyIndex;
static char kingSquare;
static Bitboard allPieces;
static Bitboard allPiecesInverted;
static Bitboard enemyPieces;
static Bitboard enemyPiecesEnPassant;
static Bitboard friendPiecesInverted;
static Bitboard enemyAttacks;
static Bitboard attackBlocks;
static Bitboard pinRays;
static Bitboard queens;
static Bitboard rooks;
static Bitboard bishops;

Bitboard GeneralShift(Bitboard bitboard, int x);
void GenerateKingMoves(MoveList *list, Board *board);
void GeneratePawnMoves(MoveList *list, Board *board);
void GenerateKnightMoves(MoveList *list, Board *board);
void GenerateSlidingMoves(MoveList *list, Board *board);
void GenerateAttackData(Board *board);
void PrintBitboard(Bitboard board);
bool IsPinned(char square);
bool IsEnPassantPin(char start, char target);

MoveList *GenerateMoves(Board *board)
{
    MoveList *moveList = malloc(sizeof(MoveList));
    moveList->count = 0;

    friendColour = board->colourToMove;
    friendindex = friendColour >> 4;
    enemyColour  = friendColour ^ COLORMASK;
    enemyIndex = enemyColour >> 4;
    checks = 0;

    enemyPieces = board->pieceBoards[enemyColour-8];
    enemyPiecesEnPassant = enemyPieces | 1ULL << board->enPassant;
    allPieces = enemyPieces | board->pieceBoards[friendColour-8];
    allPiecesInverted = ~allPieces;
    friendPiecesInverted = ~(board->pieceBoards[friendColour-8]);
    enemyAttacks = 0;

    Bitboard king = board->pieceBoards[(KING | friendColour)-8];
    kingSquare = 63 - __lzcnt64(king);

    attackBlocks = 0;
    pinRays = 0;
    GenerateAttackData(board);
    GenerateKingMoves(moveList, board);
    if (checks >= 2) return moveList;

    GeneratePawnMoves(moveList, board);
    GenerateKnightMoves(moveList, board);
    GenerateSlidingMoves(moveList, board);

    // printf("moves: %i\n", moveList->count);
    return moveList;
}

void GenerateKingMoves(MoveList *list, Board *board)
{
    Bitboard movesBitboard = kingMoves[kingSquare];
    movesBitboard &= friendPiecesInverted;

    Bitboard castleMask = castleMasks[friendindex][0];
    castleMask &= allPieces;
    Bitboard castleBitboard = castleSquares[friendindex][0] * !castleMask * board->castleRights[(friendindex<<1)+1];
    castleBitboard &= ~(enemyAttacks >> 1 | enemyAttacks >> 2);
    movesBitboard |= castleBitboard;
    
    castleMask = castleMasks[friendindex][1];
    castleMask &= allPieces;
    castleBitboard = castleSquares[friendindex][1] * !castleMask * board->castleRights[friendindex<<1];
    castleBitboard &= ~(enemyAttacks << 1 | enemyAttacks << 2);
    movesBitboard |= castleBitboard;

    movesBitboard &= ~enemyAttacks;

    while (movesBitboard)
    {
        int target = 63 - __lzcnt64(movesBitboard);
        movesBitboard ^= (Bitboard)1 << target;
        char flag = ((castleSquaresMask & (1ULL << target)) & ~kingMoves[kingSquare]) ? CASTLE : NOFLAG;
        list->moves[list->count++] = MOVE(kingSquare, target, flag);
    }
}

void GeneratePawnMoves(MoveList *list, Board *board)
{
    Bitboard pieces = board->pieceBoards[(PAWN | friendColour)-8];
    
    int shiftOffset = 8 - (friendindex<<4);
    Bitboard _fourthRank = fourthRank[friendindex];
    _fourthRank &= GeneralShift(allPiecesInverted, shiftOffset);

    Bitboard movesBitboard = GeneralShift(pieces, shiftOffset);
    movesBitboard &= allPiecesInverted;
    movesBitboard &= attackBlocks;

    while (movesBitboard)
    {
        int target = 63 - __lzcnt64(movesBitboard);
        int start = target - shiftOffset;
        movesBitboard ^= (Bitboard)1 << target;
        if(IsPinned(start) && alignMasks[kingSquare][start] != alignMasks[kingSquare][target]) continue;

        int rank = target >> 3;
        if (rank == pawnPromotionRank[friendindex])
        {
            list->moves[list->count++] = MOVE(start, target, PROMOTEQUEEN);
            list->moves[list->count++] = MOVE(start, target, PROMOTEROOK);
            list->moves[list->count++] = MOVE(start, target, PROMOTEKNIGHT);
            list->moves[list->count++] = MOVE(start, target, PROMOTEBISHOP);
        }
        else
        {
            list->moves[list->count++] = MOVE(start, target, 0);
        }
    }

    shiftOffset = 16 - (friendindex << 5);
    movesBitboard = GeneralShift(pieces, shiftOffset);
    movesBitboard &= allPiecesInverted;
    movesBitboard &= _fourthRank;
    movesBitboard &= attackBlocks;

    while (movesBitboard)
    {
        int target = 63 - __lzcnt64(movesBitboard);
        int start = target-shiftOffset;
        movesBitboard ^= (Bitboard)1 << target;
        if(IsPinned(start) && alignMasks[kingSquare][start] != alignMasks[kingSquare][target]) continue;
        list->moves[list->count++] = MOVE(start, target, PAWN2FORDWARD);
    }

    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        Bitboard attackBlocksEnPassant = (board->enPassant) ? attackBlocks | 1ULL << board->enPassant : attackBlocks;
        Bitboard movesBitboard = pawnAttacks[friendindex][piecePos];
        movesBitboard &= enemyPiecesEnPassant;
        movesBitboard &= attackBlocksEnPassant;
        movesBitboard &= (IsPinned(piecePos)) ? alignMasks[kingSquare][piecePos] : -1ULL;


        while (movesBitboard)
        {
            int target = 63 - __lzcnt64(movesBitboard);
            movesBitboard ^= (Bitboard)1 << target;

            int rank = target >> 3;
            if (rank == pawnPromotionRank[friendindex])
            {
                list->moves[list->count++] = MOVE(piecePos, target, PROMOTEQUEEN);
                list->moves[list->count++] = MOVE(piecePos, target, PROMOTEROOK);
                list->moves[list->count++] = MOVE(piecePos, target, PROMOTEKNIGHT);
                list->moves[list->count++] = MOVE(piecePos, target, PROMOTEBISHOP);
            }
            else
            {
                char flag = NOFLAG;
                if(target == board->enPassant)
                {
                    if(!IsEnPassantPin(piecePos, target)) flag = ENPASSANT;
                    else continue;
                }

                list->moves[list->count++] = MOVE(piecePos, target, flag);
            }
        }
    }
}

void GenerateKnightMoves(MoveList *list, Board *board)
{
    Bitboard pieces = board->pieceBoards[(KNIGHT | friendColour)-8];

    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        Bitboard movesBitboard = knightMoves[piecePos];
        movesBitboard &= friendPiecesInverted;
        movesBitboard &= attackBlocks;
        movesBitboard *= !IsPinned(piecePos);
        
        while (movesBitboard)
        {
            int target = 63 - __lzcnt64(movesBitboard);
            movesBitboard ^= (Bitboard)1 << target;
            list->moves[list->count++] = MOVE(piecePos, target, 0);
        }
    }
}

void GenerateSlidingMoves(MoveList *list, Board *board)
{
    Bitboard pieces = board->pieceBoards[(ROOK | friendColour)-8];
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(rookMasks[piecePos], allPieces);
        Bitboard movesBitboard = rookAttacks[piecePos][index];
        movesBitboard &= friendPiecesInverted;
        movesBitboard &= attackBlocks;
        movesBitboard &= (IsPinned(piecePos)) ? alignMasks[kingSquare][piecePos] : -1ULL;

        while (movesBitboard)
        {
            int target = 63 - __lzcnt64(movesBitboard);
            movesBitboard ^= (Bitboard)1 << target;
            list->moves[list->count++] = MOVE(piecePos, target, 0);
        }
    }

    pieces = board->pieceBoards[(BISHOP | friendColour)-8];
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(bishopMasks[piecePos], allPieces);
        Bitboard movesBitboard = bishopAttacks[piecePos][index];
        movesBitboard &= friendPiecesInverted;
        movesBitboard &= attackBlocks;
        movesBitboard &= (IsPinned(piecePos)) ? alignMasks[kingSquare][piecePos] : -1ULL;

        while (movesBitboard)
        {
            int target = 63 - __lzcnt64(movesBitboard);
            movesBitboard ^= (Bitboard)1 << target;
            list->moves[list->count++] = MOVE(piecePos, target, 0);
        }
    }

    pieces = board->pieceBoards[(QUEEN | friendColour)-8];
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(bishopMasks[piecePos], allPieces);
        Bitboard movesBitboard = bishopAttacks[piecePos][index];
        index = GetIndex(rookMasks[piecePos], allPieces);
        movesBitboard |= rookAttacks[piecePos][index];
        movesBitboard &= friendPiecesInverted;
        movesBitboard &= attackBlocks;
        movesBitboard &= (IsPinned(piecePos)) ? alignMasks[kingSquare][piecePos] : -1ULL;

        while (movesBitboard)
        {
            int target = 63 - __lzcnt64(movesBitboard);
            movesBitboard ^= (Bitboard)1 << target;
            list->moves[list->count++] = MOVE(piecePos, target, 0);
        }
    }
}

void GenerateAttackData(Board *board)
{
    int piecePos;
    Bitboard friendKing = board->pieceBoards[(KING | friendColour)-8];
    Bitboard pieces = board->pieceBoards[(KNIGHT | enemyColour)-8];

    Bitboard _knightAttacks = 0;    
    while (pieces)
    {
        piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        Bitboard moves = knightMoves[piecePos];
        _knightAttacks |= moves;
        if (moves & friendKing)
        {
            attackBlocks |= 1ULL << piecePos;
            checks++;
        } 
    }
    
    Bitboard _pawnAttacks = 0;
    pieces = board->pieceBoards[(PAWN | enemyColour)-8];
    while (pieces)
    {
        piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        Bitboard moves = pawnAttacks[enemyIndex][piecePos]; 
        _pawnAttacks |= moves;
        if (moves & friendKing)
        {
            attackBlocks |= 1ULL << piecePos;
            checks++;
        } 
    }

    rooks   = board->pieceBoards[(ROOK   | enemyColour)-8];
    bishops = board->pieceBoards[(BISHOP | enemyColour)-8];
    queens  = board->pieceBoards[(QUEEN  | enemyColour)-8];

    Bitboard _rookAttacks = 0;
    Bitboard allPiecesNoKing = allPieces ^ board->pieceBoards[(KING | friendColour)-8];
    pieces = rooks;
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(rookMasks[piecePos], allPiecesNoKing);
        _rookAttacks |= rookAttacks[piecePos][index];
    }

    Bitboard _bishopAttacks = 0;
    pieces = bishops;
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(bishopMasks[piecePos], allPiecesNoKing);
        _bishopAttacks |= bishopAttacks[piecePos][index];
    }

    Bitboard _queenAttacks = 0;
    pieces = queens;
    while (pieces)
    {
        int piecePos = 63 - __lzcnt64(pieces);
        pieces ^= (Bitboard)1 << piecePos;
        int index = GetIndex(bishopMasks[piecePos], allPiecesNoKing);
        _queenAttacks |= bishopAttacks[piecePos][index];
        index = GetIndex(rookMasks[piecePos], allPiecesNoKing);
        _queenAttacks |= rookAttacks[piecePos][index];
    }

    int startDirection = 0;
    int endDirection   = 8;
    if(!CountPopulation(queens))
    {
        startDirection = (CountPopulation(rooks))   ? 0 : 4;
        endDirection   = (CountPopulation(bishops)) ? 8 : 4;
    }

    for(int dir = startDirection; dir < endDirection; dir++)
    {
        bool isDiagonal = dir > 3;
        Bitboard pieces = (isDiagonal) ? queens | bishops : queens | rooks; 

        if ((slidingRays[kingSquare][dir] & pieces) == 0) continue;

        int n = movesToEdge[kingSquare][dir];
        int offset = directionOffsets[dir];
        bool friendAlongRay = false;
        Bitboard ray = 0;

        for(int i = 1; i <= n; i++)
        {
            int target = kingSquare + offset * i;
            ray |= 1ULL << target;
            int piece = board->map[target];
            if(piece == NONE) continue;

            bool isEnemy = IsColour(piece, enemyColour);

            if(!isEnemy)
            {
                if(friendAlongRay) break;
                friendAlongRay = true;
                continue;
            }
            bool isDanger = (isDiagonal) ? IsQueenOrBishop(piece) : IsQueenOrRook(piece);
            
            if(isDanger) 
            {
                if (friendAlongRay)
                {
                    pinRays |= ray;
                    break;
                }
                else
                {
                    attackBlocks |= ray;
                    checks++;
                    break;
                }
            }
            else break;
            if(checks >= 2) break;
        }
    }

    if(!checks) attackBlocks = -1ULL;

    pieces = board->pieceBoards[(KING | enemyColour)-8];
    piecePos = 63 - __lzcnt64(pieces);
    enemyAttacks = _knightAttacks | _pawnAttacks | _bishopAttacks | _rookAttacks | _queenAttacks;
    enemyAttacks |= kingMoves[piecePos];
}

bool IsEnPassantPin(char start, char target)
{
    Bitboard mask = rookMasks[kingSquare];
    int offset = (friendColour == WHITE) ? -8 : 8;
    Bitboard occupancy = allPieces; 
    occupancy ^= 1ULL << start;
    occupancy |= 1ULL << target;
    occupancy ^= 1ULL << (target+offset);
    return (rookAttacks[kingSquare][GetIndex(mask, occupancy)] & (queens | rooks)) != 0;
}

void PrintBitboard(Bitboard bitboard)
{
    for (int i = 7; i >= 0; i--)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%li", (bitboard >> (i*8+j)) & 1);
        }
        printf("\n");
    }
}

inline bool IsPinned(char square)
{
    return (pinRays & (1ULL << square)) != 0;
}

inline Bitboard GeneralShift(Bitboard bitboard, int x)
{
    return (x > 0) ? (bitboard << x) : (bitboard >> -x);
}

#endif