#ifndef FEN
#define FEN

#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "piece.h"
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

char *CreateFen(Board *board)
{
    char *fen = malloc(128);
    int strIndex = 0;
    int empty = 0;

    for(int rank = 7; rank >= 0; rank--)
    {
        for(int file = 0; file < 8; file++)
        {
            int square = rank*8+file;
            char piece = board->map[square];
            if(piece == NONE)
            {
                empty++;
                continue;
            }

            if(piece != NONE && empty)
            {
                fen[strIndex++] = '0'+empty;
                empty = 0;
            }

            switch(board->map[square])
            {
                case KING   | WHITE: { fen[strIndex++] = 'K'; break; }
                case QUEEN  | WHITE: { fen[strIndex++] = 'Q'; break; }
                case ROOK   | WHITE: { fen[strIndex++] = 'R'; break; }
                case BISHOP | WHITE: { fen[strIndex++] = 'B'; break; }
                case KNIGHT | WHITE: { fen[strIndex++] = 'N'; break; }
                case PAWN   | WHITE: { fen[strIndex++] = 'P'; break; }
                case KING   | BLACK: { fen[strIndex++] = 'k'; break; }
                case QUEEN  | BLACK: { fen[strIndex++] = 'q'; break; }
                case ROOK   | BLACK: { fen[strIndex++] = 'r'; break; }
                case BISHOP | BLACK: { fen[strIndex++] = 'b'; break; }
                case KNIGHT | BLACK: { fen[strIndex++] = 'n'; break; }
                case PAWN   | BLACK: { fen[strIndex++] = 'p'; break; }
                case NONE: empty++;
            }
        }
        if(empty) 
        {
            fen[strIndex++] = '0'+empty;
            empty = 0;
        }
        if(rank != 0) fen[strIndex++] = '/';
    }

    fen[strIndex++] = ' ';
    fen[strIndex++] = (board->colourToMove == WHITE) ? 'w' : 'b';
    fen[strIndex++] = ' ';

    if(board->castleRights[0] || board->castleRights[1] || board->castleRights[2] || board->castleRights[3])
    {
        if(board->castleRights[0]) fen[strIndex++] = 'K';
        if(board->castleRights[1]) fen[strIndex++] = 'Q';
        if(board->castleRights[2]) fen[strIndex++] = 'k';
        if(board->castleRights[3]) fen[strIndex++] = 'q';
    }
    else fen[strIndex++] = '-';

    fen[strIndex++] = ' ';

    int enPassant = board->enPassant;
    if(enPassant)
    {
        int rank = enPassant / 8;
        int file = enPassant % 8;
        fen[strIndex++] = 'a'+file;
        fen[strIndex++] = '1'+rank;
    }
    else fen[strIndex++] = '-';

    fen[strIndex++] = ' ';

    return fen;
}

void LoadFen(Board *board, char *fen)
{
    char c;
    int i = -1;
    int file = 0;
    int rank = 7;
    while(true)
    {
        i++;
        c = fen[i];

        if (c == ' ')
        {
            i++;
            break;
        }

        if(c >= '0' && c <= '9')
        {
            file += c - '0';
            continue;
        }

        if(c == '/')
        {
            rank--;
            file = 0;
            continue;
        }

        int square = rank*8+file;
        if (c == 'p')
        {
            board->map[square] = BLACK | PAWN;
        }
        else if (c == 'n')
        {
            board->map[square] = BLACK | KNIGHT;
        }
        else if (c == 'b')
        {
            board->map[square] = BLACK | BISHOP;
        }
        else if (c == 'r')
        {
            board->map[square] = BLACK | ROOK;
        }
        else if (c == 'q')
        {
            board->map[square] = BLACK | QUEEN;
        }
        else if (c == 'k')
        {
            board->map[square] = BLACK | KING;
        }
        else if (c == 'P')
        {
            board->map[square] = WHITE | PAWN;
        }
        else if (c == 'N')
        {
            board->map[square] = WHITE | KNIGHT;
        }
        else if (c == 'B')
        {
            board->map[square] = WHITE | BISHOP;
        }
        else if (c == 'R')
        {
            board->map[square] = WHITE | ROOK;
        }
        else if (c == 'Q')
        {
            board->map[square] = WHITE | QUEEN;
        }
        else if (c == 'K')
        {
            board->map[square] = WHITE | KING;
        }
        else return;
        file++;
    }

    board->colourToMove = fen[i] == 'w' ? WHITE : BLACK;
    i += 2;

    c = fen[i];

    if (c != '-')
    {
        for (c = fen[i]; c != ' '; i++)
        {
            c = fen[i];
            if (c == 'K') board->castleRights[0] = true;
            else if (c == 'Q') board->castleRights[1] = true;
            else if (c == 'k') board->castleRights[2] = true;
            else if (c == 'q') board->castleRights[3] = true;
        }
    }
    else i+=2;


    c = fen[i++];
    if (c >= 'a' && c <= 'h')
    {
        board->enPassant = c - 'a';
        c = fen[i++];

        board->enPassant += (c - '0' - 1)*8;
        i++;
    }
    else if (c == '-') 
    {
        board->enPassant = -1;
        i++;
    }
    else return;
}

#endif