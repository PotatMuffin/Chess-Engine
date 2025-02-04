#ifndef INPUT_IMPLEMENTATION
#define INPUT_IMPLEMENTATION

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "interface.h"
#include "board.h"
#include "piece.h"
#include "move.h"
#include "movegeneration.h"

static int selectedSquare = -1;

Move HandleInput(Board *board, int startX, int startY)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePosition = GetMousePosition();
        float x = mousePosition.x - startX;
        float y = mousePosition.y - startY;
        if (x >= 0 && x <= TILESIZE*8 && y >= 0 && y <= TILESIZE*8)
        {
            int file = x / TILESIZE;
            int rank = (800-y) / TILESIZE;
            int square = rank*8+file;
            
            if (square == selectedSquare) 
            {
                ResetSquare(selectedSquare);
                ResetHighlight();
                selectedSquare = -1;
                return nullMove;
            }

            if (board->map[square] == NONE && selectedSquare == -1)
            {
                return nullMove;
            }

            if (selectedSquare == -1)
            {
                MoveList *list = GenerateMoves(board);

                for (int i = 0; i < list->count; i++)
                {
                    Move move = list->moves[i];
                    if (START(move) == square)
                    {
                        selectedSquare = square;
                        SelectSquare(square);
                        HighlightSquares(list, square);
                        break;
                    } 
                }
                free(list);
            }
            else 
            {
                MoveList *list = GenerateMoves(board);
                for (int i = 0; i < list->count; i++)
                {
                    Move move = list->moves[i];
                    char start = START(move);
                    char target = TARGET(move);
                    if(start == selectedSquare && target == square)
                    {
                        free(list);
                        selectedSquare = -1;
                        ResetSquares();
                        return move;
                        break;
                    }
                }
                free(list);
                ResetSquare(selectedSquare);
                ResetHighlight();
                selectedSquare = -1;
            }
        }
    }
    return nullMove;
}

#endif