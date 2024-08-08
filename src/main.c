#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>

int main(int argc, char ** argv) {
    int const screenHeight = 720;
    int const screenWidth = 1250;
    int frameCount = 0;

    const int mapWidth = 480;
    const int mapHeight = 560;

    Vector2 spacing = {mapHeight/10.0f, mapWidth/10.0f};
    Vector2 centerPoint = {3*spacing.x, 4*spacing.y};
    Vector2 seccondPoint = {6*spacing.x, 6*spacing.y};

    Color backgrounColor = {0x45,0x45,0x45,0xff};
    Color rayColor = {0xe5,0xbe,0x01,0xff};

    InitWindow(screenWidth, screenHeight, "Raycast the Hell out of it");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        frameCount+=1;
        BeginDrawing();
        ClearBackground(backgrounColor);

        for (unsigned int i  = 0 ; i < 10 + 1 ; ++i) {
                DrawLineV((Vector2){0, spacing.x*i+1}, (Vector2){mapWidth, spacing.x*i+1}, RAYWHITE);
        }

        for (unsigned int i  = 0 ; i < 10 + 1 ; ++i) {
                DrawLineV((Vector2){spacing.y*i+1, 0}, (Vector2){spacing.y*i+1, mapHeight}, RAYWHITE);
        }

        DrawCircleV(centerPoint, 5.0f, rayColor);
        DrawCircleV(seccondPoint, 5.0f, rayColor);
        DrawLineV(centerPoint, seccondPoint, rayColor);

        EndDrawing();
        
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            seccondPoint = GetMousePosition();
        } 
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            seccondPoint = GetMousePosition();
        }
            
        if (frameCount >= 1) {
            frameCount = 0;
            if (IsKeyDown(KEY_W)) centerPoint.y -= 0.1*spacing.y;
            if (IsKeyDown(KEY_S)) centerPoint.y += 0.1*spacing.y;
            if (IsKeyDown(KEY_D)) centerPoint.x += 0.1*spacing.x;
            if (IsKeyDown(KEY_A)) centerPoint.x -= 0.1*spacing.x;
        }
        
    }
}
