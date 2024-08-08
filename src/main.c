#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>

void drawMap();
void drawPoint(Vector2 point, float r);
Vector2 screenCordsToMapCords(Vector2 pos);
void drawLine(Vector2 pos1, Vector2 pos2);

int const screenHeight = 720;
int const screenWidth = 1250;
int frameCount = 0;
int mapSegments = 10;

const int mapWidth = 480;
const int mapHeight = 560;

float spacing_x;
float spacing_y;

Color backgrounColor = {0x45,0x45,0x45,0xff};
Color rayColor = {0xe5,0xbe,0x01,0xff};

int main(int argc, char ** argv) {

    spacing_x = (float)mapWidth/(float)mapSegments;
    spacing_y = (float)mapHeight/(float)mapSegments;

    Vector2 centerPoint = {1, 1};
    Vector2 seccondPoint = {6, 6};


    InitWindow(screenWidth, screenHeight, "Raycast the Hell out of it");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        frameCount+=1;
        BeginDrawing();
        ClearBackground(backgrounColor);

        drawMap();

        drawPoint(centerPoint, 5.0f);
        drawPoint(seccondPoint, 5.0f);
        drawLine(centerPoint, seccondPoint);

        EndDrawing();
        
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            seccondPoint = screenCordsToMapCords(GetMousePosition());
        } 
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            seccondPoint = screenCordsToMapCords(GetMousePosition());
        }
            
        if (frameCount >= 1) {
            frameCount = 0;
            if (IsKeyDown(KEY_W)) centerPoint.y -= 0.05f;
            if (IsKeyDown(KEY_S)) centerPoint.y += 0.05f;
            if (IsKeyDown(KEY_D)) centerPoint.x += 0.05f;
            if (IsKeyDown(KEY_A)) centerPoint.x -= 0.05f;;
        }
        
    }
}

void drawMap() {
        for (unsigned int i  = 0 ; i < mapSegments + 1 ; ++i) {
                DrawLineV((Vector2){0, spacing_y*i+1}, (Vector2){mapWidth, spacing_y*i+1}, RAYWHITE);
        }

        for (unsigned int i  = 0 ; i < mapSegments + 1 ; ++i) {
                DrawLineV((Vector2){spacing_x*i+1, 0}, (Vector2){spacing_x*i+1, mapHeight}, RAYWHITE);
        }
}

void drawPoint(Vector2 point, float r) {
    Vector2 screenPoint = {point.x*mapWidth/mapSegments, point.y*mapHeight/mapSegments};
    DrawCircleV(screenPoint, r, rayColor);
}

void drawLine(Vector2 pos1, Vector2 pos2) {
    Vector2 screenPos1 = {pos1.x*spacing_x, pos1.y*spacing_y,};
    Vector2 screenPos2 = {pos2.x*spacing_x, pos2.y*spacing_y};

    DrawLineV(screenPos1, screenPos2, rayColor);
}

Vector2 screenCordsToMapCords(Vector2 pos) {
    return (Vector2){pos.x/mapWidth*mapSegments, pos.y/mapHeight*mapSegments};
}
