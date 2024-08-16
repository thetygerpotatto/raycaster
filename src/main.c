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

void drawMiniMap();
void drawPoint(Vector2 point, float r);
Vector2 screenCordsToMapCords(Vector2 pos);
void drawLine(Vector2 pos1, Vector2 pos2);
bool** createMap(map m);
bool isWhole(float n);
Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3);
float snap(float p1, float sign);
float sign(float x);
Vector2 step(Vector2 pc, float angle);
void drawPov(Vector2 p1, float fov, float angle, Vector2 dir);
Vector2 collision(Vector2 newcords, Vector2 oldCords);

const int screenHeight = 1080;
const int screenWidth = 1920;
const int miniMapHeight = 200;
const int miniMapwidth = 200;
const int mapSegments = 15;

const Vector2 miniMapCords = {screenWidth - miniMapwidth, 0};

int frameCount = 0;
float EPS = 1e-3;


map gameMap = {.rows = mapSegments, .cols = mapSegments};

float spacing_x;
float spacing_y;

Color backgrounColor = {0x45,0x45,0x45,0xff};
Color wallColor = {0x19, 0x19, 0x19, 0xff};
Color rayColor = {0xe5,0xbe,0x01,0xff};

int main(int argc, char ** argv) {
    gameMap.cells = createMap(gameMap);

    spacing_x = (float)miniMapwidth/(float)mapSegments;
    spacing_y = (float)miniMapwidth/(float)mapSegments;

    Vector2 centerPoint = {2, 2};
    Vector2 seccondPoint = {6, 6};
    float viewAngle = 0;

    Vector2 dir = {cosf(viewAngle), sinf(viewAngle)};
    const float pov = PI/2;


    InitWindow(screenWidth, screenHeight, "Raycast the Hell out of it");
    SetTargetFPS(60);

    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            if (x == 0 || x == gameMap.cols-1 || y == 0 || y == gameMap.rows-1) gameMap.cells[x][y] = true;
        }
    }

    gameMap.cells[5][5] = true;
    gameMap.cells[6][5] = true;
    gameMap.cells[7][5] = true;
    gameMap.cells[8][5] = true;
    gameMap.cells[5][6] = true;
    gameMap.cells[8][6] = true;
    gameMap.cells[8][7] = true;


    while (!WindowShouldClose()) {
        frameCount+=1;

        //trace ray
        seccondPoint = step(centerPoint, viewAngle);

        BeginDrawing();
        ClearBackground(backgrounColor);

        drawPov(centerPoint, pov, viewAngle, dir);
        drawMiniMap();


        drawPoint(centerPoint, 5.0f);
        drawPoint(seccondPoint, 5.0f);
        drawLine(centerPoint, seccondPoint);

        EndDrawing();
        
        dir = (Vector2){cosf(viewAngle), sinf(viewAngle)};

        if (frameCount >= 1) {
            frameCount = 0;
            if (IsKeyDown(KEY_W)) centerPoint = collision(Vector2Add(centerPoint, Vector2Scale(dir, 0.06f)),
                                                          centerPoint);
            if (IsKeyDown(KEY_S)) centerPoint = collision(Vector2Subtract(centerPoint, Vector2Scale(dir, 0.06f)),
                                                          centerPoint);
            if (IsKeyDown(KEY_D)) centerPoint = collision(Vector2Add(centerPoint,
                                                           Vector2Scale(Vector2Rotate(dir, PI/2.0f), 0.06f)),
                                                          centerPoint);
            if (IsKeyDown(KEY_A)) centerPoint = collision(Vector2Add(centerPoint,
                                                           Vector2Scale(Vector2Rotate(dir, -PI/2.0f), 0.06f))
                                                            , centerPoint);
            if (IsKeyDown(KEY_LEFT)) viewAngle -= PI/120.0f;
            if (IsKeyDown(KEY_RIGHT)) viewAngle += PI/120.0f;
        }

        
    }
}

Vector2 step(Vector2 pc, float angle) {
    Vector2 p1 = pc;
    Vector2 p2;
    float dir_x = sign(cosf(angle));
    float dir_y = sign(sinf(angle));
    
    for (;;) {
        Vector2 xp2;
        Vector2 yp2;

        if (dir_x != 0) {
            xp2.x = snap(p1.x, dir_x);
            xp2.y = p1.y;
            xp2.y += (xp2.x - p1.x)*tanf(angle);
        } else {
            xp2.x = (dir_y < 0) ? 0 : gameMap.cols;
            xp2.y = p1.y;
        }
        
        if (dir_y != 0){
            yp2.y = snap(p1.y, dir_y);
            yp2.x = p1.x;
            yp2.x += (yp2.y - p1.y)/tanf(angle);
        } else {
            yp2.x = (dir_x < 0) ? 0 : gameMap.rows;
            yp2.y = p1.y;
        }

        p2 = getClosest(p1, xp2, yp2);
        p1 = p2;

        if (p2.x > 0 && p2.x < gameMap.cols &&
            p2.y > 0 && p2.x < gameMap.rows) {

            if (gameMap.cells[(int)p2.x][(int)p2.y]) {
                return p2;
            }
        } 
        else 
        {

            return pc;
        }
    }

}

float snap(float x, float sign) {
    if (sign == 1) return ceilf(x) +sign*EPS;
    if (sign == -1) return floorf(x) + sign*EPS;
    return x;
}


Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3) {
    float d2 = Vector2Distance(vec1, vec2);
    float d3 = Vector2Distance(vec1, vec3);
    
    if (d2 == d3) return vec2;
    return (d2 < d3) ? vec2 : vec3;
}


void drawMiniMap() {
    for (unsigned int i  = 0 ; i < mapSegments + 1 ; ++i) {
            DrawLineV((Vector2){miniMapCords.x + 0, + miniMapCords.y + spacing_y*i+1},
                      (Vector2){miniMapCords.x + miniMapwidth, miniMapCords.y + spacing_y*i+1}, RAYWHITE);
    }

    for (unsigned int i  = 0 ; i < mapSegments + 1 ; ++i) {
            DrawLineV((Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + 0},
                      (Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + miniMapHeight}, RAYWHITE);
    }

    for (size_t cols = 0; cols < gameMap.cols; ++cols) {
        for (size_t rows = 0 ; rows < gameMap.rows; ++rows) {
            if (gameMap.cells[cols][rows]) {
                DrawRectangle(miniMapCords.x + spacing_x*cols+1, miniMapCords.y + spacing_y*rows+1, spacing_x-2, spacing_y-2 , wallColor);
            }
        }
    }
}

void drawPov(Vector2 pc, float fov, float angle, Vector2 dir) {
    for (int x = 0; x < screenWidth; ++x) {
        float currentAngle = (angle - fov / 2) + fov/screenWidth*x; 
        Vector2 p1 = step(pc, currentAngle);
        Vector2 v = Vector2Subtract(p1, pc);
        float distance = Vector2DotProduct(v, dir);

        float wallHeight = screenHeight / distance;
        float startDrawing = screenHeight/2 - wallHeight/2;
        float endDrawing = screenHeight/2 + wallHeight/2;
        
        Color wallShade = {0, 0, 180/distance, 0xff};
        DrawLine(x, startDrawing, x, endDrawing, wallShade);
    }
}

void drawPoint(Vector2 point, float r) {
    Vector2 screenPoint = {miniMapCords.x + point.x*miniMapwidth/mapSegments, miniMapCords.y + point.y*miniMapHeight/mapSegments};
    DrawCircleV(screenPoint, r, rayColor);
}

void drawLine(Vector2 pos1, Vector2 pos2) {
    Vector2 screenPos1 = {miniMapCords.x + pos1.x*spacing_x, miniMapCords.y + pos1.y*spacing_y,};
    Vector2 screenPos2 = {miniMapCords.x + pos2.x*spacing_x, miniMapCords.y + pos2.y*spacing_y};

    DrawLineV(screenPos1, screenPos2, rayColor);
}

Vector2 screenCordsToMapCords(Vector2 pos) {
    return (Vector2){miniMapCords.x + pos.x/miniMapwidth*mapSegments, miniMapCords.y + pos.y/miniMapHeight*mapSegments};
}

Vector2 collision(Vector2 newcords, Vector2 oldCords) {
    if (newcords.x > 0 && newcords.x < gameMap.cols &&
        newcords.y > 0 && newcords.y < gameMap.rows) {
        if (gameMap.cells[(int)newcords.x][(int)newcords.y]) {
            return oldCords;
        } 
        else return newcords;
    }
    return oldCords;
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


float sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
