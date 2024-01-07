#include "raylib.h"
#include <stdio.h>

int main() {
    printf("Hello, world\n");

    InitWindow(800, 450, "K-means clustering in 3D"); // GetRandomValue seed is created with initwindow

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    DisableCursor();
    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);

                DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
                DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

            EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}