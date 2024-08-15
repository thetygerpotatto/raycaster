#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>

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
bool isWhole(float n);
Vector2 calculateRayX(Vector2 centerPoint, float viewAngle);
Vector2 calculateRayY(Vector2 centerPoint, float viewAngle);
Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3);

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

    Vector2 centerPoint = {2, 2};
    Vector2 seccondPoint = {6, 6};
    float viewAngle = 0;
    float norm = 8.0f;
    Vector2 dir = {cosf(viewAngle)*norm, sinf(viewAngle)*norm};


    InitWindow(screenWidth, screenHeight, "Raycast the Hell out of it");
    SetTargetFPS(60);

    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            if (x == 0 || x == 9 || y == 0 || y == 9) gameMap.cells[x][y] = true;
        }
    }

    gameMap.cells[5][5] = true;


    while (!WindowShouldClose()) {
        frameCount+=1;
        //Inteserctions with the x and y axis
        seccondPoint = getClosest(centerPoint, calculateRayX(centerPoint, viewAngle), calculateRayY(centerPoint, viewAngle));

        BeginDrawing();
        ClearBackground(backgrounColor);


        drawMap();


        drawPoint(centerPoint, 5.0f);
        drawPoint(seccondPoint, 5.0f);
        drawLine(centerPoint, seccondPoint);

        EndDrawing();
        
        if (frameCount >= 1) {
            frameCount = 0;
            if (IsKeyDown(KEY_W)) centerPoint.y -= 0.05f;
            if (IsKeyDown(KEY_S)) centerPoint.y += 0.05f;
            if (IsKeyDown(KEY_D)) centerPoint.x += 0.05f;
            if (IsKeyDown(KEY_A)) centerPoint.x -= 0.05f;
            if (IsKeyDown(KEY_LEFT)) viewAngle += PI/120.0f;
            if (IsKeyDown(KEY_RIGHT)) viewAngle -= PI/120.0f;
        }
        
    }
}

Vector2 calculateRayX(Vector2 centerPoint, float viewAngle) {
        for (float x, y, step, iter = 0; ; iter++) {
            if (cosf(viewAngle) > 0) {
                step = 1;
            } else if (cosf(viewAngle) < 0) {
                step = -1;
            } else {
                step = 0;
            }

            float offset;
            if (step == 1) {
                offset = ceilf(centerPoint.x) - centerPoint.x;
            } else if(step == -1) {
                offset = centerPoint.x - floorf(centerPoint.x);
            } else {
                return (Vector2){gameMap.cols, gameMap.rows};
            }

            x = step * (iter + offset);
            y = tanf(viewAngle)*x;
            
            float map_x = x + centerPoint.x;
            float map_y = y + centerPoint.y;

            if (map_x > 0 && map_x < gameMap.cols &&
                map_y > 0 && map_y < gameMap.rows) {

                if (step == -1) {
                    if (gameMap.cells[(int)map_x-1][(int)map_y]) {
                        return (Vector2){map_x, map_y};
                    }
                } else {
                    if (gameMap.cells[(int)map_x][(int)map_y]) {
                        return (Vector2){map_x, map_y};
                    }
                }
            } else {
                return (Vector2){map_x, map_y};
            }
        }
}

Vector2 calculateRayY(Vector2 centerPoint, float viewAngle) {
        for (float x, y, step, iter = 0;; iter++) {
            if (sinf(viewAngle) > 0) {
                step = 1;
            } else if (sinf(viewAngle) < 0) {
                step = -1;
            } else {
                step = 0;
            }

            float offset;
            if (step == 1) {
                offset = ceilf(centerPoint.y) - centerPoint.y;
            } else if(step == -1) {
                offset = centerPoint.y - floorf(centerPoint.y);
            } else {
                return (Vector2){gameMap.cols, gameMap.rows};
            }
        

            y = step*(iter + offset);
            x = y / tanf(viewAngle);
            //if (tanf(viewAngle) == 0) x = 0.0f;

            float map_x = x + centerPoint.x;
            float map_y = y + centerPoint.y;

            if (map_x > 0 && map_x < gameMap.cols &&
                map_y > 0 && map_y < gameMap.rows) {

                if (step == -1) {
                    if (gameMap.cells[(int)map_x][(int)map_y-1]) {
                        return (Vector2){map_x, map_y};
                    }
                } else {
                    if (gameMap.cells[(int)map_x][(int)map_y]) {
                        return (Vector2){map_x, map_y};
                    }
                }

            } else {
                return (Vector2){map_x, map_y};
            }
        }
}

Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3) {
    float d2 = Vector2Distance(vec1, vec2);
    float d3 = Vector2Distance(vec1, vec3);
    
    if (d2 == d3) return vec2;
    return (d2 < d3) ? vec2 : vec3;
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
bool isWhole(float n) {
    if (n == (int)n) {
        return true;
    }
    return false;
}
