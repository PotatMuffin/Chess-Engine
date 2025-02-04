#ifndef UI_IMPLEMENTATION
#define UI_IMPLEMENTATION

#include "move.h"
#include "board.h"

#define TILESIZE 100
#define WHITECOLOUR GetColor(0xeed8c0FF)
#define BLACKCOLOUR GetColor(0xab7a65FF)
#define WHITEHIGHLIGHTCOLOUR GetColor(0xdd5959ff)
#define BLACKHIGHLIGHTCOLOUR GetColor(0xc5444fff)
#define WHITESELECTCOLOUR GetColor(0xddd07cFF)
#define BLACKSELECTCOLOUR GetColor(0xc59e5eFF)
#define BORDERCOLOUR GetColor(0x664a3eFF)

enum SquareStatus {
    NORMAL,
    HIGHLIGHTED,
    SELECTEDHIGHLIGHT, 
    SELECTED
};

static enum SquareStatus squareStatus[64] = {0};

void SelectSquare(char index)
{
    squareStatus[index] = SELECTED;
}

void HighlightSquare(char index)
{
     if (squareStatus[index] == SELECTED) squareStatus[index] = SELECTEDHIGHLIGHT;
     else squareStatus[index] = HIGHLIGHTED;
}

void HighlightSquares(MoveList *list, char start)
{
    for (int i = 0; i < list->count; i++)
    {
        Move move = list->moves[i];
        if (START(move) == start) HighlightSquare(TARGET(move));
    }
}

void ResetSquare(char index)
{
    squareStatus[index] = NORMAL;
}

void ResetSquares()
{
    for (int i = 0; i < 64; i++)
    {
        squareStatus[i] = NORMAL;
    }
}

void ResetHighlight()
{
    for(int i = 0; i < 64; i++)
    {
        if(squareStatus[i] == HIGHLIGHTED) squareStatus[i] = NORMAL;
        if(squareStatus[i] == SELECTEDHIGHLIGHT) squareStatus[i] = SELECTED;
    }
}

void DrawBoard(Board *board, Texture2D spriteSheet, int offsetX, int offsetY) 
{
    DrawRectangle(offsetX-TILESIZE/4, offsetY-TILESIZE/4, 8.5f*TILESIZE, 8.5f*TILESIZE, BORDERCOLOUR);
    
    for (int x = 0; x < 8; x++) 
    {
        for(int y = 0; y < 8; y++)
        {
            int index = (7-y)*8+x;
            bool isWhite = (x+y)%2 == 0;
            Color colour = (isWhite) ? WHITECOLOUR : BLACKCOLOUR;
            if(squareStatus[index] == HIGHLIGHTED || squareStatus[index] == SELECTEDHIGHLIGHT) colour = (isWhite) ? WHITEHIGHLIGHTCOLOUR : BLACKHIGHLIGHTCOLOUR;
            else if(squareStatus[index] == SELECTED) colour = (isWhite) ? WHITESELECTCOLOUR : BLACKSELECTCOLOUR;

            DrawRectangle(x*TILESIZE+offsetX, y*TILESIZE+offsetY, TILESIZE, TILESIZE, colour);

            char piece = board->map[index];
            if (piece != 0)
            {
                Rectangle pieceSprite = {0, 0, 450.0f, 450.0f};
                char pieceType = GetPieceType(piece);

                switch (pieceType) {
                    case KING:
                        break;
                    case QUEEN:
                        pieceSprite.x = 450.0f;
                        break;
                    case BISHOP:
                        pieceSprite.x = 900.0f;
                        break;
                    case KNIGHT:
                        pieceSprite.x = 1350.0f;
                        break;
                    case ROOK:
                        pieceSprite.x = 1800.0f;
                        break;
                    case PAWN:
                        pieceSprite.x = 2250.0f;
                        break;
                }

                if (IsColour(piece, BLACK))
                {
                    pieceSprite.y = 450.0f;
                }

                DrawTexturePro(spriteSheet, pieceSprite, 
                    (Rectangle){x*TILESIZE+offsetX, y*TILESIZE+offsetY, TILESIZE, TILESIZE}, 
                    (Vector2){0, 0}, 0, GetColor(0xFFFFFFFF));
            }
        }
    }
}

#endif