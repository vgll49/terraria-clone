#include "raylib.h"
#include <iostream>

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Gothic");

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Welcome to the colony", 190, 200, 20, RED);

        EndDrawing();
    }
    
    CloseWindow();

	return 0;
}