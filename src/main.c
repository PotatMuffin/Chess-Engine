#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/raylib.h"
#include "player.h"
#include "board.h"
#include "fen.h"
#include "input.h"
#include "interface.h"
#include "moveprecalculation.h"
#include "movegeneration.h"
#include "zobrist.h"
#include "transpositiontable.h"

Player players[2] = {0};

static MoveList playedMoves = {0};

void DrawBoard(Board *board, Texture2D spriteSheet, int offsetX, int offsetY);
Rectangle GetPieceSprite(char piece);

int main(int argc, char **argv) 
{
    printf("Transposition size: %d\n", sizeof(Transposition));
    players[0] = NEGA;
    players[1] = NEGA;
    CalculateMoveData();
    InitPieceSquareTable();
    InitZobristKeys(&zobrist);

    TranspositionTable tables[2] = {0};
    if(players[0] == NEGA) InitTranspositionTable(&tables[0], 1024*1024*2);
    if(players[1] == NEGA) InitTranspositionTable(&tables[1], 1024*1024*2);

    Board board = {0};
    InitBoard(&board, DEFAULT_FEN);

    InitWindow(0, 0, "Chess Engine");
    SetTargetFPS(60);

    Texture2D spriteSheet = LoadTexture("./sprites/Chess_Pieces_Sprite.png");

    int monitor = GetCurrentMonitor();
    int width = GetMonitorWidth(monitor);
    int height = GetMonitorHeight(monitor);
    SetWindowSize(width, height);
    ToggleFullscreen();
    // SetExitKey(KEY_NULL);

    int offsetX = width/2-4*TILESIZE; 
    int offsetY = height/2-4*TILESIZE;

    printf("zobrist key: %lu\n", board.zobristKey);
    StartTurn(players[board.colourToMove >> 4], &board, &tables[board.colourToMove >> 4]);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        DrawBoard(&board, spriteSheet, offsetX, offsetY);
        EndDrawing();

        int playerIndex = board.colourToMove >> 4;
        Move move = GetMove(players[playerIndex], &board, offsetX, offsetY);
        
        if (move != nullMove)
        {
            ResetSquares();
            SelectSquare(START(move));
            SelectSquare(TARGET(move));
            MakeMove(&board, move);
            printf("new zobrist: %lu\n", board.zobristKey);
            playedMoves.moves[playedMoves.count++] = move;
            StartTurn(players[board.colourToMove >> 4], &board, &tables[board.colourToMove >> 4]);
        }
    }
    CloseWindow();
}