#include "game.hpp"

int main()
{
    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT
    );
    InitWindow(G::ini_win_w, G::ini_win_h, G::win_title);
    SetTargetFPS(G::max_fps);

    Game game;
    game.start();

    while (!WindowShouldClose()) {
        G::win_w = GetScreenWidth();
        G::win_h = GetScreenHeight();
        G::dt = GetFrameTime();
        G::fps = 1/G::dt;
        game.input();
        game.loop();

        BeginDrawing();
        ClearBackground(BLACK);
        game.render();
        EndDrawing();
    }

    game.quit();
    CloseWindow();
}
