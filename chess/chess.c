#include <stdio.h>
#include <math.h>
#include "raylib.h"

void draw_board(int tilesize);
int compute_tilesize();
void draw_pieces();
Board* init_board();

typedef struct _BitBoard {
    __uint64_t board;
} BitBoard;

typedef struct _ColoredBoard {
    BitBoard kings;
    BitBoard queens;
    BitBoard rooks;
    BitBoard bishops;
    BitBoard knights;
    BitBoard pawns;
} ColoredBoard;

typedef struct _Board {
    ColoredBoard white;
    ColoredBoard black;
} Board;

int main() {
    const char* image_paths[] = {
        "./images/bB.png",
    };


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(800, 800, "Hello world test");

    Image bB_image = LoadImage(image_paths[0]);
    Texture2D bB_texture = LoadTextureFromImage(bB_image);
    bB_texture = LoadTextureFromImage(bB_image);
    UnloadImage(bB_image);

    bool has_screen_changed = true;
    int old_tilesize = compute_tilesize();
    int new_tilesize = 0;
    while (!WindowShouldClose()) {
        new_tilesize = compute_tilesize();

        if (new_tilesize != old_tilesize) {
            has_screen_changed = true;
            old_tilesize = new_tilesize;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            draw_board(new_tilesize);
            DrawTexturePro(bB_texture, (Rectangle){0, 0, 1024, 1024}, (Rectangle){0, 0, new_tilesize, new_tilesize}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

Board* init_board() {
    Board* board = (Board*)malloc(sizeof(Board));

    __uint64_t bKing = 0b0000100000000000000000000000000000000000000000000000000000000000;
    __uint64_t wKing = 0b0000000000000000000000000000000000000000000000000000000000001000;

    ColoredBoard white = {
        .kings = wKing,
        .queens = 0,
        .rooks = 0,
        .bishops = 0,
        .knights = 0,
        .pawns = 0,
    };

    ColoredBoard black = {
        .kings = bKing,
        .queens = 0,
        .rooks = 0,
        .bishops = 0,
        .knights = 0,
        .pawns = 0,
    };

    *board = (Board){
        .white = white,
        .black = black,
    };

    return board;
}

void draw_pieces() {

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