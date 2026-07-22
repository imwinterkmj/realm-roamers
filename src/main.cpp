#include <raylib.h>

int main() {
    constexpr int windowWidth = 960;
    constexpr int windowHeight = 540;

    InitWindow(windowWidth, windowHeight, "Realm Roamers");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib is running", 48, 48, 32, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
