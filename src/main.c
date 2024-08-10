#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>

typedef struct map {
    size_t rows;
    size_t cols;
    bool **cells;
} map;

void drawMap();
void drawPoint(Vector2 point, float r);
Vector2 screenCordsToMapCords(Vector2 pos);
void drawLine(Vector2 pos1, Vector2 pos2);
bool** createMap(map m);


int const screenHeight = 720;
int const screenWidth = 1250;
int frameCount = 0;
const int mapSegments = 10;

const int mapWidth = 480;
const int mapHeight = 560;

map gameMap = {.rows = mapSegments, .cols = mapSegments};

float spacing_x;
float spacing_y;

Color backgrounColor = {0x45,0x45,0x45,0xff};
Color wallColor = {0x19, 0x19, 0x19, 0xff};
Color rayColor = {0xe5,0xbe,0x01,0xff};

int main(int argc, char ** argv) {
    gameMap.cells = createMap(gameMap);

    spacing_x = (float)mapWidth/(float)mapSegments;
    spacing_y = (float)mapHeight/(float)mapSegments;

    Vector2 centerPoint = {1, 1};
    Vector2 seccondPoint = {6, 6};


    InitWindow(screenWidth, screenHeight, "Raycast the Hell out of it");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        gameMap.cells[0][0] = true;
        gameMap.cells[0][1] = true;
        gameMap.cells[0][2] = true;
        gameMap.cells[0][3]= true;
        gameMap.cells[0][4] = true;
        gameMap.cells[0][5] = true;
        gameMap.cells[0][6] = true;
        gameMap.cells[0][8] = true;
        gameMap.cells[0][9] = true;

        gameMap.cells[9][0] = true;
        gameMap.cells[9][1] = true;
        gameMap.cells[9][2] = true;
        gameMap.cells[9][3]= true;
        gameMap.cells[9][4] = true;
        gameMap.cells[9][5] = true;
        gameMap.cells[9][6] = true;
        gameMap.cells[9][8] = true;
        gameMap.cells[9][9] = true;
        
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

    for (size_t rows = 0; rows < gameMap.rows; ++rows) {
        for (size_t cols = 0 ; cols < gameMap.cols; ++cols) {
            if (gameMap.cells[rows][cols]) {
                DrawRectangle(spacing_x*cols+1, spacing_y*rows+1, spacing_x-2, spacing_y-2 , wallColor);
            }
        }
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

bool** createMap(map m) {
    bool** newMap;
    newMap = calloc(m.rows, sizeof(bool**));
    for (size_t i = 0; i < m.rows; ++i) {
        newMap[i] = calloc(m.cols, sizeof(bool));
    }
    return newMap;
}
