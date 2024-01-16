#include <stdio.h>
#include "raylib.h"

int main() {
    printf("Hello, world\n");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    InitWindow(800, 450, "Hello world test");
    
    while (!WindowShouldClose()) {
        int tilesize = compute_tilesize();

        BeginDrawing();
            ClearBackground(BLACK);
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if ((i+j) % 2 == 0) DrawRectangle(i*tilesize, j*tilesize, tilesize, tilesize, (Color){240,217,181,255});
                    else DrawRectangle(i*tilesize, j*tilesize, tilesize, tilesize, (Color){181,136,99,255});
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
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