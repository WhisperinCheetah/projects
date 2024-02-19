#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "raylib.h"

typedef enum _PIECE {
    NOPIECE=-1,
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

__uint64_t bit_at_pos(int x, int y) {
    if (x < 0 || y < 0 || x > 7 || y > 7) return 0;
    __uint64_t bit = 0b1000000000000000000000000000000000000000000000000000000000000000;
    return bit >> (x + y*8);
}

int piece_at_tile(bool white_to_play, Board* board, int x, int y, PIECE piece) {
    if (piece == NOPIECE) {
        if (white_to_play) {
            for (int i = 0; i < 6; i++) {
                if (((board->bitboards[i] >> (x + y*8)) & 1) != 0) return i;
            }
        } else {
            for (int i = 6; i < 12; i++) {
                if (((board->bitboards[i] >> (x + y*8)) & 1) != 0) return i;
            }
        }
    } else {
        printf("Checking bitboard: %lu\n", board->bitboards[piece]);
        printf("Piece at place? %lu\n", board->bitboards[piece] >> (x + y*8));
        printf("For piece: %d\n\n", piece);
        if ((board->bitboards[piece] & (bit_at_pos(x, y))) != 0) return piece;
    }

    return -1;
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

__uint64_t get_board_bitboard(Board* board) {
    __uint64_t res = 0;
    for (int i = 0; i < 12; i++) {
        res |= board->bitboards[i];
    }

    return res;
}

__uint64_t get_white_pawn_moves(Board* board, int x, int y) {
    __uint64_t full_board = get_board_bitboard(board);
    __uint64_t res = 0;
    res |= bit_at_pos(x, y-1) & (~full_board);
    if (y >= 6 && res != 0) res |= bit_at_pos(x, y-2) & (~full_board); // res!=0 check to see if pawn is blocked
    
    __uint64_t black_board = get_black_bitboard(board);
    res |= bit_at_pos(x+1, y-1) & (black_board);
    res |= bit_at_pos(x-1, y-1) & (black_board);
    return res;
}

__uint64_t get_black_pawn_moves(Board* board, int x, int y) {
    __uint64_t full_board = get_board_bitboard(board);
    __uint64_t res = 0;
    res |= bit_at_pos(x, y+1) & (~full_board);
    if (y <= 1 && res != 0) res |= bit_at_pos(x, y+2) & (~full_board);

    __uint64_t white_board = get_white_bitboard(board);
    res |= bit_at_pos(x+1, y+1) & (white_board);
    res |= bit_at_pos(x-1, y+1) & (white_board);
    return res;
}

__uint64_t get_white_king_moves(Board* board, int x, int y) {
    __uint64_t res = 0;
    for (int j = y-1; j <= y+1; j++) {
        for (int i = x-1; i <= x+1; i++) {
            if (j >= 0 && i >= 0 && j < 8 && i < 8) {
                res |= bit_at_pos(i, j);
            }
        }
    }

    printf("x=%d\n", x);
    printf("y=%d\n", y);
    printf("Piece at tile? %d\n", piece_at_tile(true, board, 7, 7, WROOK));
    if (x==4 && y==7 && (piece_at_tile(true, board, 7, 7, WROOK) != NOPIECE)) {
        res |= bit_at_pos(7, 7);
    }

    return res;
}

__uint64_t get_black_king_moves(Board* board, int x, int y) {
        __uint64_t res = 0;
    for (int j = y-1; j <= y+1; j++) {
        for (int i = x-1; i <= x+1; i++) {
            if (j >= 0 && i >= 0 && j < 8 && i < 8) {
                res |= bit_at_pos(i, j);
            }
        }
    }

    // if (piece_at_tile(true, board, 7, 7, WROOK)) {
    //     res |= bit_at_pos(7, 7); // TODO abstracter + check if there are no pieces in between
    // }

    return res;
}

__uint64_t get_knight_moves(Board* board, int x, int y) {
    const int X[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
    const int Y[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };

    __uint64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= bit_at_pos(x+X[i], y+Y[i]);
    }

    return res;
}

__uint64_t get_bishop_moves(Board* board, int x, int y) {
    __uint64_t full_board = get_board_bitboard(board);
    __uint64_t res = 0;
    __uint8_t blocked = 0;
    for (int i = 1; i < 7; i++) {
        if (!(0b0001 & blocked)) {
            res |= bit_at_pos(x-i, y-i);
            if (!(bit_at_pos(x-i, y-i) & (~full_board))) blocked |= 0b0001;
        }
        if (!(0b0010 & blocked)) {
            res |= bit_at_pos(x+i, y-i);
            if (!(bit_at_pos(x+i, y-i) & (~full_board))) blocked |= 0b0010;
        }
        if (!(0b0100 & blocked)) {
            res |= bit_at_pos(x-i, y+i);
            if (!(bit_at_pos(x-i, y+i) & (~full_board))) blocked |= 0b0100;
        }
        if (!(0b1000 & blocked)) {
            res |= bit_at_pos(x+i, y+i);
            if (!(bit_at_pos(x+i, y+i) & (~full_board))) blocked |= 0b1000;
        }
    }
    return res;
}

__uint64_t get_rook_moves(Board* board, int x, int y) {
    // TODO cleanup same way as bishop moves
    __uint64_t full_board = get_board_bitboard(board);
    __uint64_t res = 0;
    for (int i = x+1; i < 8; i++) {
        __uint64_t move = bit_at_pos(i, y) & (~full_board);
        if (move != 0) res |= move;
        else {
            res |= bit_at_pos(i, y);
            break;
        }
    }

    for (int i = x-1; i >= 0; i--) {
        __uint64_t move = bit_at_pos(i, y) & (~full_board);
        if (move != 0) res |= move;
        else {
            res |= bit_at_pos(i, y);
            break;
        }
    }

    for (int j = y+1; j < 8; j++) {
        __uint64_t move = bit_at_pos(x, j) & (~full_board);
        if (move != 0) {
            res |= move;
        } else {
            res |= bit_at_pos(x, j);
            break;
        }
    }

    for (int j = y-1; j >= 0; j--) {
        __uint64_t move = bit_at_pos(x, j) & (~full_board);
        if (move != 0) {
            res |= move;
        } else {
            res |= bit_at_pos(x, j);
            break;
        }
    }

    return res ;
}

__uint64_t get_queen_moves(Board* board, int x, int y) {
    return (get_rook_moves(board, x, y) | get_bishop_moves(board, x, y));
}


__uint64_t get_possible_moves(Board* board, PIECE piece, int x, int y) {
    __uint64_t moves = 0;
    if (piece==WKING) moves = get_white_king_moves(board, x, y);
    if (piece==BKING) moves = get_black_king_moves(board, x, y);
    else if (piece==WQUEEN || piece==BQUEEN) moves = get_queen_moves(board, x, y);
    else if (piece==WROOK || piece==BROOK) moves = get_rook_moves(board, x, y);
    else if (piece==WBISHOP || piece==BBISHOP) moves = get_bishop_moves(board, x, y);
    else if (piece==WKNIGHT || piece==BKNIGHT) moves = get_knight_moves(board, x, y);
    else if (piece==WPAWN) moves = get_white_pawn_moves(board, x, y);
    else if (piece==BPAWN) moves = get_black_pawn_moves(board, x, y);

    if (piece <= 5) return moves & ~(get_white_bitboard(board));
    else return moves & ~(get_black_bitboard(board));
}

void move_piece(Board* board, PIECE piece, int x1, int y1, int x2, int y2) {
    board->bitboards[piece] = board->bitboards[piece] & (~(bit_at_pos(x1, y1)));
    for (int i = 0; i < 12; i++) {
        board->bitboards[i] = board->bitboards[i] & (~(bit_at_pos(x2, y2)));
    }
    board->bitboards[piece] = board->bitboards[piece] | (bit_at_pos(x2, y2));
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

bool is_possible_move(Board* board, PIECE piece, int x1, int y1, int x2, int y2) {
    __uint64_t possible_moves = get_possible_moves(board, piece, x1, y1);
    return (possible_moves & bit_at_pos(x2, y2)) != 0; 
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

void destroy_board(Board** board) {
    if (*board != NULL) {
        free(*board);
        *board = NULL;
    }
}

Texture2D* load_textures(const char** file_paths) {
    Texture2D* textures = (Texture2D*)malloc(sizeof(Texture2D) * 12);
    for (int i = 0; i < 12; i++) {
        textures[i] = LoadTexture(file_paths[i]);
    }

    return textures;
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

void draw_pieces(Board* board, Texture2D* textures, size_t tilesize) {
    for (int i = 0; i < 12; i++) {
        draw_piece(board->bitboards[i], &textures[i], tilesize);
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
    InitWindow(800, 800, "Chess");

    Texture2D* textures = load_textures(image_paths);
    Board* board = init_board();

    size_t tilesize = 0;
    bool white_to_play = true;
    int clicked_piece = -1; // -1 for no click, >=0 for bitboard which was clicked
    int clicked_piece_x = 0;
    int clicked_piece_y = 0;
    while (!WindowShouldClose()) {
        tilesize = compute_tilesize();
        
        if ((clicked_piece != -1) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int boardSize;  
            if (GetScreenWidth() < GetScreenHeight()) boardSize = GetScreenWidth();
            else boardSize = GetScreenHeight();

            int dropped_x = GetMouseX() / (boardSize / 8);
            int dropped_y = GetMouseY() / (boardSize / 8);

            if (is_possible_move(board, clicked_piece, clicked_piece_x, clicked_piece_y, dropped_x, dropped_y)) {
                move_piece(board, clicked_piece, clicked_piece_x, clicked_piece_y, dropped_x, dropped_y);
                white_to_play = !white_to_play;
            }
            clicked_piece = -1;
        } else if ((clicked_piece == -1) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int boardSize;
            if (GetScreenWidth() < GetScreenHeight()) boardSize = GetScreenWidth();
            else boardSize = GetScreenHeight();
            clicked_piece_x = GetMouseX() / (boardSize / 8);
            clicked_piece_y = GetMouseY() / (boardSize / 8);
            
            clicked_piece = piece_at_tile(white_to_play, board, 7 - clicked_piece_x, 7 - clicked_piece_y, -1);
        }

        BeginDrawing();
            ClearBackground(BLACK);
            draw_board(tilesize);
            if (clicked_piece != -1) draw_possible_moves(get_possible_moves(board, clicked_piece, clicked_piece_x, clicked_piece_y), tilesize);
            draw_pieces(board, textures, tilesize);
        EndDrawing();

    }

    for (int i = 0; i < 12; i++) {
        UnloadTexture(textures[i]);
    }
    free(textures);
    CloseWindow();

    destroy_board(&board);


    return 0;
}