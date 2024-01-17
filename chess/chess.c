#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "raylib.h"

typedef enum _PIECE {
    WKING=0,
    WQUEEN,
    WROOK,
    WBISHOP,
    WKNIGHT,
    WPAWN,
    BKING,
    BQUEEN,
    BROOK,
    BBISHOP,
    BKNIGHT,
    BPAWN,
} PIECE;

typedef struct _Board {
    __uint64_t bitboards[12];
} Board;

void draw_board(int tilesize);
int compute_tilesize();
Board* init_board();
Texture2D* load_textures(const char** file_paths);
void draw_pieces(Board* board, Texture2D* textures, size_t tilesize);
void draw_piece(__uint64_t bitboard, Texture2D* texture, size_t tilesize);


int main() {
    const char* image_paths[] = {
        "./images/wK.png",
        "./images/wQ.png",
        "./images/wR.png",
        "./images/wB.png",
        "./images/wK.png",
        "./images/wP.png",
        "./images/bK.png",
        "./images/bQ.png",
        "./images/bR.png",
        "./images/bB.png",
        "./images/bK.png",
        "./images/bP.png",
    };


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(1);
    InitWindow(800, 800, "Hello world test");

    Texture2D* textures = load_textures(image_paths);
    Board* board = init_board();

    size_t tilesize = 0;
    while (!WindowShouldClose()) {
        tilesize = compute_tilesize();

        BeginDrawing();
            ClearBackground(BLACK);
            draw_board(tilesize);
            draw_pieces(board, textures, tilesize);
        EndDrawing();
    }

    CloseWindow();

    for (int i = 0; i < 12; i++) {
        UnloadTexture(textures[i]);
    }
    free(textures);

    return 0;
}

Board* init_board() {
    Board* board = (Board*)malloc(sizeof(Board));

    __uint64_t bKing = 0b0000100000000000000000000000000000000000000000000000000000000000;
    __uint64_t wKing = 0b0000000000000000000000000000000000000000000000000000000000001000;
    __uint64_t bQueen = 0b0001000000000000000000000000000000000000000000000000000000000000;
    __uint64_t wQueen = 0b0000000000000000000000000000000000000000000000000000000000010000;
    __uint64_t bBishop = 0b0010010000000000000000000000000000000000000000000000000000000000;
    __uint64_t wBishop = 0b0000000000000000000000000000000000000000000000000000000000100100;
    __uint64_t bKnight = 0b0100001000000000000000000000000000000000000000000000000000000000;
    __uint64_t wKnight = 0b0000000000000000000000000000000000000000000000000000000001000010;


    *board = (Board){
        .bitboards = {0},
    };

    board->bitboards[WKING] = wKing;
    board->bitboards[WQUEEN] = wQueen;
    board->bitboards[WBISHOP] = wBishop;
    board->bitboards[WKNIGHT] = wKnight;
    board->bitboards[BKING] = bKing;
    board->bitboards[BQUEEN] = bQueen;
    board->bitboards[BBISHOP] = bBishop;
    board->bitboards[BKNIGHT] = bKnight;

    return board;
}

Texture2D* load_textures(const char** file_paths) {
    Texture2D* textures = (Texture2D*)malloc(sizeof(Texture2D) * 12);
    for (int i = 0; i < 12; i++) {
        textures[i] = LoadTexture(file_paths[i]);
    }

    return textures;
}

void draw_pieces(Board* board, Texture2D* textures, size_t tilesize) {
    for (int i = 0; i < 12; i++) {
        draw_piece(board->bitboards[i], &textures[i], tilesize);
    }
}

void draw_piece(__uint64_t bitboard, Texture2D* texture, size_t tilesize) {
    for (int i = 63; i >= 0; i--) { // go back to front on bitboard
        if ((bitboard & 1) > 0) {
            size_t x = i % 8;
            size_t y = i / 8;
            DrawTexturePro(*texture, (Rectangle){0, 0, 1024, 1024}, (Rectangle){x*tilesize, y*tilesize, tilesize, tilesize}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        }
        bitboard = bitboard >> 1;
    }
}

void draw_board(int tilesize) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i+j) % 2 == 0) DrawRectangle(i*tilesize, j*tilesize, tilesize, tilesize, (Color){240,217,181,255});
            else DrawRectangle(i*tilesize, j*tilesize, tilesize, tilesize, (Color){181,136,99,255});
        }
    }
} 


int compute_tilesize() {
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    if (width < height) {
        return (int)floor(width / 8);
    } else {
        return (int)floor(height / 8);
    }
}