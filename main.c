#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "raylib.h"
#include "piece.h"

#define TileSize 100

void DrawBoard(int screenWidth, int screenHeight);

int main(int argc, char **argv) 
{
    InitWindow(800, 600, "Hello World!");
    SetTargetFPS(60);

    int monitor = GetCurrentMonitor();
    int width = GetMonitorWidth(monitor);
    int height = GetMonitorHeight(monitor);
    printf("width: %i, height: %i\n", width, height);
    SetWindowSize(width, height);
    ToggleFullscreen();

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF));
        DrawBoard(width, height);
        EndDrawing();
    }
    CloseWindow();
}

void DrawBoard(int screenWidth, int screenHeight) 
{
    int offsetX = screenWidth/2-4*TileSize; 
    int offsetY = screenHeight/2-4*TileSize;
    for (int x = 0; x < 8; x++) 
    {
        for(int y = 0; y < 8; y++)
        {
            bool isWhite = (x+y)%2 == 0;
            DrawRectangle(x*TileSize+offsetX, y*TileSize+offsetY, TileSize, TileSize, GetColor(isWhite ? 0xeed8c0FF : 0xab7a65FF));
        }
    }
}