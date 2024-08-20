#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>

typedef struct map {
    size_t rows;
    size_t cols;
    Color **cells;
} map;

void drawMiniMap();
void drawPoint(Vector2 point, float r);
Vector2 screenCordsToMapCords(Vector2 pos);
void drawLine(Vector2 pos1, Vector2 pos2);
Color** createMap(map m);
bool isWhole(float n);
bool cmpColor(Color a, Color b);
Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3);
float snap(float p1, float sign);
float sign(float x);
Vector2 step(Vector2 pc, float angle);
void drawFov(Vector2 p1, float fov, float angle, Vector2 dir);
Vector2 collision(Vector2 newcords, Vector2 oldCords);

const int SCREEN_HEIGHT = 720;
const int SCREEN_WIDTH = 1080;
const int MINIMAP_HEIGHT = 200;
const int MINIMAP_WIDTH = 200;
const int H_SEGMENTS = 15;
const int V_SEGMENTS = 15;
const int FRAME_TARGET = 60;

const Vector2 miniMapCords = {SCREEN_WIDTH - MINIMAP_WIDTH, 0};

const float spacing_x = (float)MINIMAP_WIDTH/(float)H_SEGMENTS;
const float spacing_y = (float)MINIMAP_WIDTH/(float)H_SEGMENTS;

const float EPS = 1e-3;


map gameMap = {.cols = H_SEGMENTS, .rows = V_SEGMENTS};



const Color backgroundColor = {0x45,0x45,0x45,0xff};
const Color wallColor = {0x19, 0x19, 0x19, 0xff};
const Color rayColor = {0xe5,0xbe,0x01,0xff};

int main(int argc, char ** argv) {
    gameMap.cells = createMap(gameMap);

    Vector2 centerPoint = {2, 2};
    Vector2 seccondPoint = {6, 6};
    float viewAngle = 0;

    Vector2 dir = {cosf(viewAngle), sinf(viewAngle)};
    const float fov = PI/2;
    
    bool render = true;


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycast the Hell out of it");
    SetTargetFPS(FRAME_TARGET);

    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            gameMap.cells[x][y] = wallColor;
        }
    }

    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            if (x == 0 || x == gameMap.cols-1 || y == 0 || y == gameMap.rows-1) gameMap.cells[x][y] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
        }
    }

    gameMap.cells[5][5] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[6][5] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[7][5] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[8][5] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[5][6] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[8][6] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[8][7] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[8][9] = (Color){rand()%255, rand()%255, rand()%255, 0xff};
    gameMap.cells[7][9] = (Color){rand()%255, rand()%255, rand()%255, 0xff};

    while (!WindowShouldClose()) {

        BeginDrawing();
        if (render) {
            //tace ray
            seccondPoint = step(centerPoint, viewAngle);

            ClearBackground(backgroundColor);

            drawFov(centerPoint, fov, viewAngle, dir);
            drawMiniMap();


            drawPoint(centerPoint, 5.0f);
            drawPoint(seccondPoint, 5.0f);
            drawLine(centerPoint, seccondPoint);

            render = false;
        }
        EndDrawing();

        
        dir = (Vector2){cosf(viewAngle), sinf(viewAngle)};

        if (IsKeyDown(KEY_W)) centerPoint = collision(Vector2Add(centerPoint, Vector2Scale(dir, 0.06f)), centerPoint);
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
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
            IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) render = true;
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

            if (!cmpColor(gameMap.cells[(int)p2.x][(int)p2.y], wallColor)) {
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
    for (unsigned int i  = 0 ; i < H_SEGMENTS + 1 ; ++i) {
            DrawLineV((Vector2){miniMapCords.x + 0, + miniMapCords.y + spacing_y*i+1},
                      (Vector2){miniMapCords.x + MINIMAP_WIDTH, miniMapCords.y + spacing_y*i+1}, RAYWHITE);
    }

    for (unsigned int i  = 0 ; i < H_SEGMENTS + 1 ; ++i) {
            DrawLineV((Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + 0},
                      (Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + MINIMAP_HEIGHT}, RAYWHITE);
    }

    for (size_t cols = 0; cols < gameMap.cols; ++cols) {
        for (size_t rows = 0 ; rows < gameMap.rows; ++rows) {
            if (!cmpColor(gameMap.cells[cols][rows], wallColor)) {
                DrawRectangle(miniMapCords.x + spacing_x*cols+1, miniMapCords.y + spacing_y*rows+1, spacing_x-2, spacing_y-2 , wallColor);
            }
        }
    }
}

void drawFov(Vector2 pc, float fov, float angle, Vector2 dir) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float currentAngle = (angle - fov / 2) + fov/SCREEN_WIDTH*x; 
        Vector2 p1 = step(pc, currentAngle);
        drawLine(pc, p1);
        Vector2 v = Vector2Subtract(p1, pc);
        float distance = Vector2DotProduct(v, dir);

        float wallHeight = SCREEN_HEIGHT / distance;
        float startDrawing = SCREEN_HEIGHT/2 - wallHeight/2;
        float endDrawing = SCREEN_HEIGHT/2 + wallHeight/2;
        float defaultDistance = H_SEGMENTS;
        
        Color wallShade = ColorBrightness(gameMap.cells[(int)p1.x][(int)p1.y], distance/defaultDistance - 0.5f);
        DrawLine(x, startDrawing, x, endDrawing, wallShade);
    }
}

void drawPoint(Vector2 point, float r) {
    Vector2 screenPoint = {miniMapCords.x + point.x*MINIMAP_WIDTH/H_SEGMENTS, miniMapCords.y + point.y*MINIMAP_HEIGHT/H_SEGMENTS};
    DrawCircleV(screenPoint, r, rayColor);
}

void drawLine(Vector2 pos1, Vector2 pos2) {
    Vector2 screenPos1 = {miniMapCords.x + pos1.x*spacing_x, miniMapCords.y + pos1.y*spacing_y,};
    Vector2 screenPos2 = {miniMapCords.x + pos2.x*spacing_x, miniMapCords.y + pos2.y*spacing_y};

    DrawLineV(screenPos1, screenPos2, rayColor);
}

Vector2 screenCordsToMapCords(Vector2 pos) {
    return (Vector2){miniMapCords.x + pos.x/MINIMAP_WIDTH*H_SEGMENTS, miniMapCords.y + pos.y/MINIMAP_HEIGHT*H_SEGMENTS};
}

Vector2 collision(Vector2 newcords, Vector2 oldCords) {
    if (newcords.x > 0 && newcords.x < gameMap.cols &&
        newcords.y > 0 && newcords.y < gameMap.rows) {
        if (!cmpColor(gameMap.cells[(int)newcords.x][(int)newcords.y], wallColor)) {
            return oldCords;
        } 
        else return newcords;
    }
    return oldCords;
}

Color** createMap(map m) {
    Color** newMap;
    newMap = calloc(m.rows, sizeof(Color**));
    for (size_t i = 0; i < m.rows; ++i) {
        newMap[i] = calloc(m.cols, sizeof(Color));
    }
    return newMap;
}

bool isWhole(float n) {
    if (n == (int)n) {
        return true;
    }
    return false;
}

bool cmpColor(Color a, Color b) {
    if (a.r != b.r) return false;
    if (a.g != b.g) return false;
    if (a.b != b.b) return false;
    if (a.a != b.a) return false;
    return true;
}

float sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
