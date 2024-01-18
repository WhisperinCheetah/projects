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
int piece_at_tile(Board* board, int x, int y);
void move_piece(Board*, PIECE, int x1, int y1, int x2, int y2);
void draw_possible_moves(__uint64_t possible_moves, int tilesize);
__uint64_t get_possible_moves(Board* board, PIECE piece, int x, int y);
__uint64_t get_board_bitboard(Board* board);
__uint64_t get_white_bitboard(Board* board);
__uint64_t get_black_bitboard(Board* board);
__uint64_t get_king_moves(int x, int y);
__uint64_t get_queen_moves(int x, int y);
__uint64_t get_rook_moves(int x, int y);
__uint64_t get_bishop_moves(int x, int y);
__uint64_t get_knight_moves(int x, int y);
__uint64_t get_black_pawn_moves(int x, int y);
__uint64_t get_white_pawn_moves(int x, int y);
__uint64_t bit_at_pos(int x, int y);


int main() {
    const char* image_paths[] = {
        "./images/wK.png",
        "./images/wQ.png",
        "./images/wR.png",
        "./images/wB.png",
        "./images/wN.png",
        "./images/wP.png",
        "./images/bK.png",
        "./images/bQ.png",
        "./images/bR.png",
        "./images/bB.png",
        "./images/bN.png",
        "./images/bP.png",
    };


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(800, 800, "Hello world test");

    Texture2D* textures = load_textures(image_paths);
    Board* board = init_board();

    size_t tilesize = 0;
    int clicking = -1; // -1 for no click, +0 for bitboard which was clicked
    int clicking_x = 0;
    int clicking_y = 0;
    while (!WindowShouldClose()) {
        tilesize = compute_tilesize();
        if ((clicking == -1) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            clicking_x = GetMouseX() / (GetScreenWidth() / 8);
            clicking_y = GetMouseY() / (GetScreenHeight() / 8);
            
            clicking = piece_at_tile(board, 7 - clicking_x, 7 - clicking_y);
        }

        if ((clicking != -1) && IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
            int dropped_x = GetMouseX() / (GetScreenWidth() / 8);
            int dropped_y = GetMouseY() / (GetScreenHeight() / 8);

            move_piece(board, clicking, clicking_x, clicking_y, dropped_x, dropped_y);
            clicking = -1;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            draw_board(tilesize);
            draw_pieces(board, textures, tilesize);
            if (clicking != -1) draw_possible_moves(get_possible_moves(board, clicking, clicking_x, clicking_y), tilesize);
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
    
    __uint64_t wKing = 0b0000000000000000000000000000000000000000000000000000000000001000;
    __uint64_t wQueen = 0b0000000000000000000000000000000000000000000000000000000000010000;
    __uint64_t wBishop = 0b0000000000000000000000000000000000000000000000000000000000100100;
    __uint64_t wKnight = 0b0000000000000000000000000000000000000000000000000000000001000010;
    __uint64_t wRook = 0b0000000000000000000000000000000000000000000000000000000010000001;
    __uint64_t wPawn = 0b0000000000000000000000000000000000000000000000001111111100000000;

    __uint64_t bKing = 0b0000100000000000000000000000000000000000000000000000000000000000;
    __uint64_t bQueen = 0b0001000000000000000000000000000000000000000000000000000000000000;
    __uint64_t bBishop = 0b0010010000000000000000000000000000000000000000000000000000000000;
    __uint64_t bKnight = 0b0100001000000000000000000000000000000000000000000000000000000000;
    __uint64_t bRook = 0b1000000100000000000000000000000000000000000000000000000000000000;
    __uint64_t bPawn = 0b0000000011111111000000000000000000000000000000000000000000000000;


    *board = (Board){
        .bitboards = {0},
    };

    board->bitboards[WKING] = wKing;
    board->bitboards[WQUEEN] = wQueen;
    board->bitboards[WBISHOP] = wBishop;
    board->bitboards[WKNIGHT] = wKnight;
    board->bitboards[WROOK] = wRook;
    board->bitboards[WPAWN] = wPawn;

    board->bitboards[BKING] = bKing;
    board->bitboards[BQUEEN] = bQueen;
    board->bitboards[BBISHOP] = bBishop;
    board->bitboards[BKNIGHT] = bKnight;
    board->bitboards[BROOK] = bRook;
    board->bitboards[BPAWN] = bPawn;

    return board;
}

int piece_at_tile(Board* board, int x, int y) {
    for (int i = 0; i < 12; i++) {
        if (((board->bitboards[i] >> (x + y*8)) & 1) != 0) return i;
    }

    return -1;
}

void move_piece(Board* board, PIECE piece, int x1, int y1, int x2, int y2) {
    __uint64_t bit = 0b1000000000000000000000000000000000000000000000000000000000000000;
    board->bitboards[piece] = board->bitboards[piece] & (~(bit >> (x1 + y1*8)));
    bit = 0b1000000000000000000000000000000000000000000000000000000000000000;
    board->bitboards[piece] = board->bitboards[piece] | (bit >> (x2 + y2*8));
}

void draw_possible_moves(__uint64_t possible_moves, int tilesize) {
    for (int i = 63; i >= 0; i--) { // go back to front on bitboard
        if ((possible_moves & 1) > 0) {
            size_t x = i % 8;
            size_t y = i / 8;
            DrawCircle(x*tilesize + (tilesize/2), y*tilesize + (tilesize/2), (float)tilesize/4, (Color){0, 0, 0, 86});
        }
        possible_moves = possible_moves >> 1;
    }
}

__uint64_t get_possible_moves(Board* board, PIECE piece, int x, int y) {
    __uint64_t moves = 0;
    if (piece==WKING || piece==BKING) moves = get_king_moves(x, y);
    else if (piece==WQUEEN || piece==BQUEEN) moves = get_queen_moves(x, y);
    else if (piece==WROOK || piece==BROOK) moves = get_rook_moves(x, y);
    else if (piece==WBISHOP || piece==BBISHOP) moves = get_bishop_moves(x, y);
    else if (piece==WKNIGHT || piece==BKNIGHT) moves = get_knight_moves(x, y);
    else if (piece==WPAWN) moves = get_white_pawn_moves(x, y);
    else if (piece==BPAWN) moves = get_black_pawn_moves(x, y);

    if (piece <= 5) return moves & ~(get_white_bitboard(board));
    else return moves & ~(get_black_bitboard(board));
}

__uint64_t get_queen_moves(int x, int y) {
    return (get_rook_moves(x, y) | get_bishop_moves(x, y));
}

__uint64_t get_king_moves(int x, int y) {
    __uint64_t res = 0;
    for (int j = y-1; j <= y+1; j++) {
        for (int i = x-1; i <= x+1; i++) {
            if (j >= 0 && i >= 0 && j < 8 && i < 8) {
                res |= bit_at_pos(i, j);
            }
        }
    }

    return res;
}

__uint64_t get_rook_moves(int x, int y) {
    __uint64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= bit_at_pos(x, i);
    }

    for (int j = 0; j < 8; j++) {
        res |= bit_at_pos(j, y);
    }

    return res;
}

__uint64_t get_bishop_moves(int x, int y) {
    __uint64_t res = 0;
    for (int i = 1; i < 7; i++) {
        res |= bit_at_pos(x-i, y-i);
        res |= bit_at_pos(x+i, y-i);
        res |= bit_at_pos(x-i, y+i);
        res |= bit_at_pos(x+i, y+i);
    }
    return res;
}

__uint64_t get_knight_moves(int x, int y) {
    const int X[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
    const int Y[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

    __uint64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= bit_at_pos(x+X[i], y+Y[i]);
    }

    return res;
}

__uint64_t get_white_pawn_moves(int x, int y) {
    __uint64_t res = 0;
    res |= bit_at_pos(x, y-1);
    if (y >= 6) res |= bit_at_pos(x, y-2);
    return res;
}

__uint64_t get_black_pawn_moves(int x, int y) {
    __uint64_t res = 0;
    res |= bit_at_pos(x, y+1);
    if (y <= 1) res |= bit_at_pos(x, y+2);
    return res;
}

__uint64_t bit_at_pos(int x, int y) {
    if (x < 0 || y < 0 || x > 7 || y > 7) return 0;
    __uint64_t bit = 0b1000000000000000000000000000000000000000000000000000000000000000;
    return bit >> (x + y*8);
}

__uint64_t get_board_bitboard(Board* board) {
    __uint64_t res = 0;
    for (int i = 0; i < 12; i++) {
        res |= board->bitboards[i];
    }

    return res;
}

__uint64_t get_white_bitboard(Board* board) {
    __uint64_t res = 0;
    for (int i = 0; i <= 5; i++) {
        res |= board->bitboards[i];
    }

    return res;
}

__uint64_t get_black_bitboard(Board* board) {
    __uint64_t res = 0;
    for (int i = 6; i < 12; i++) {
        res |= board->bitboards[i];
    }

    return res;
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