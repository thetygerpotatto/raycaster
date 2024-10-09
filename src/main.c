#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <string.h>

typedef struct Cell {
    bool textured;
    bool bound;
    Color sides[4];
    Color** TextureColors;
    Image* img;
} Cell;

typedef struct map {
    size_t rows;
    size_t cols;
    Cell **cells;
} map;

enum SIDE {
    NORTH = 0,
    SOUTH,
    EAST,
    WEST
};

// Drawing functions 
void drawMiniMap();
void drawPointInMap(Vector2 point, float r);
void drawLineInMap(Vector2 pos1, Vector2 pos2);
void renderFOV(Vector2 p1, float fov, float angle, Vector2 dir);

// logic functions
Vector2 screenCordsToMapCords(Vector2 pos);
Cell** createMap(map m);
Cell getRandomCel();
bool isWhole(float n);
bool cmpColor(Color a, Color b);
Vector2 getClosest(Vector2 vec1, Vector2 vec2, Vector2 vec3);
float snap(float p1, float sign);
float sign(float x);
Vector2 step(Vector2 pc, float angle);
Vector2 collision(Vector2 newcords, Vector2 oldCords);
float getWallTouchPoint(Vector2 point);
enum SIDE getTouchSide(Vector2 p);


// load and unload resources
void loadResources();
void unloadResources();

// Game constants
const int SCREEN_HEIGHT = 720;
const int SCREEN_WIDTH = 1080;
const int MINIMAP_HEIGHT = 200;
const int MINIMAP_WIDTH = 200;
const int H_SEGMENTS = 15;
const int V_SEGMENTS = 15;
const int WALL_TEXTURE_WIDTH = 16;
const int FRAME_TARGET = 60;

const Vector2 miniMapCords = {SCREEN_WIDTH - MINIMAP_WIDTH, 0};
const float spacing_x = (float)MINIMAP_WIDTH/(float)H_SEGMENTS;
const float spacing_y = (float)MINIMAP_WIDTH/(float)H_SEGMENTS;

const float EPS = 1e-3;

const Color backgroundColor = {0x45,0x45,0x45,0xff};
const Color wallColor = {0x19, 0x19, 0x19, 0xff};
const Color rayColor = {0xe5,0xbe,0x01,0xff};

Color *BrickColors;

map gameMap = {.cols = H_SEGMENTS, .rows = V_SEGMENTS};
// Texture variables
Image brick;

const Cell BoundCell = {.textured = true,
                        .bound = true,
                        .img = &brick,
                        .sides = {wallColor, wallColor, wallColor, wallColor},
                        .TextureColors = &BrickColors};
const Cell EmptyCell = {.textured = false,
                        .bound = false};

int main(int argc, char ** argv) {
    gameMap.cells = createMap(gameMap);
    
    Vector2 centerPoint = {2, 2};
    Vector2 seccondPoint = {6, 6};
    float viewAngle = 0;

    Vector2 dir = {cosf(viewAngle), sinf(viewAngle)};
    const float fov = PI/3;
    
    bool render = true;


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycast the Hell out of it");
    loadResources();
    SetTargetFPS(FRAME_TARGET);


    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            memcpy(&gameMap.cells[x][y], &EmptyCell, sizeof(Cell));
        }
    }

    for (size_t x = 0; x < gameMap.cols; x++) {
        for (size_t y = 0; y < gameMap.rows; y++) {
            if (x == 0 || x == gameMap.cols-1 || y == 0 || y == gameMap.rows-1) 
                memcpy(&gameMap.cells[x][y], &BoundCell, sizeof(Cell));
        }
    }

    gameMap.cells[2][2] = (Cell){.sides = {RED, BLUE, YELLOW, GREEN}, .bound = true, .textured = false};
    gameMap.cells[5][5] = getRandomCel();
    gameMap.cells[6][5] = getRandomCel();
    gameMap.cells[7][5] = getRandomCel();
    gameMap.cells[8][5] = getRandomCel();
    gameMap.cells[5][6] = getRandomCel();
    gameMap.cells[8][6] = getRandomCel();
    gameMap.cells[8][7] = getRandomCel();
    gameMap.cells[8][9] = getRandomCel();
    gameMap.cells[7][9] = getRandomCel();

    while (!WindowShouldClose()) {

        BeginDrawing();
        if (render) {
            //trace ray
            seccondPoint = step(centerPoint, viewAngle);

            ClearBackground(backgroundColor);

            renderFOV(centerPoint, fov, viewAngle, dir);
            drawMiniMap();


            drawPointInMap(centerPoint, 5.0f);
            drawPointInMap(seccondPoint, 5.0f);
            drawLineInMap(centerPoint, seccondPoint);

            render = false;
        }
        EndDrawing();

        
        dir = (Vector2){cosf(viewAngle), sinf(viewAngle)};

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
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) ||
            IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) render = true;
    }
    unloadResources();
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
            xp2.y = (dir_y < 0) ? 0 : gameMap.cols;
            xp2.x = p1.x;
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
            p2.y > 0 && p2.y < gameMap.rows) {

            if (gameMap.cells[(int)p2.x][(int)p2.y].bound) {
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

    for (unsigned int i  = 0 ; i < V_SEGMENTS + 1 ; ++i) {
            DrawLineV((Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + 0},
                      (Vector2){miniMapCords.x + spacing_x*i+1, miniMapCords.y + MINIMAP_HEIGHT}, RAYWHITE);
    }

    for (size_t cols = 0; cols < gameMap.cols; ++cols) {
        for (size_t rows = 0 ; rows < gameMap.rows; ++rows) {
            if (gameMap.cells[cols][rows].bound) {
                DrawRectangle(miniMapCords.x + spacing_x*cols+1, miniMapCords.y + spacing_y*rows+1, spacing_x-2, spacing_y-2 , wallColor);
            }
        }
    }
}

void renderFOV(Vector2 pc, float fov, float angle, Vector2 dir) {
    for (int x = 0; x < SCREEN_WIDTH; x+=4) {
        float currentAngle = (angle - fov / 2) + fov/SCREEN_WIDTH*x; 
        Vector2 p1 = step(pc, currentAngle);


        drawLineInMap(pc, p1);
        Vector2 v = Vector2Subtract(p1, pc);
        float distance = Vector2DotProduct(v, dir);
        float wallHeight = SCREEN_HEIGHT / distance;
        float startDrawing = (float)SCREEN_HEIGHT/2 - wallHeight/2;
        float endDrawing = (float)SCREEN_HEIGHT/2 + wallHeight/2;
        float defaultDistance = H_SEGMENTS;
        
        int wallXOffset = (int)(getWallTouchPoint(p1) * (float)brick.width);
        enum SIDE touchSide = getTouchSide(p1);
        
        //Color wallShade = ColorBrightness(gameMap.cells[(int)p1.x][(int)p1.y], distance/defaultDistance - 0.5f);
        
        int upperBound = endDrawing;
        for (int start = startDrawing; start < upperBound; start+=4) {
            if (start < 0 || start > SCREEN_HEIGHT) {
                start = 0;
                upperBound = SCREEN_HEIGHT;
            }
            int pixelHeight = (start - startDrawing);
            int pixelYIndex = (int)((float)pixelHeight/(endDrawing - startDrawing) * (float)brick.height);
            Cell CurrentCell = gameMap.cells[(int)p1.x][(int)p1.y];

            if (CurrentCell.textured) {
                DrawRectangle(x, start, 4, 4, 
                              Fade((*CurrentCell.TextureColors)[brick.width*pixelYIndex + wallXOffset],
                                1/distance + 0.3f));
            } else {
                DrawRectangle(x, start, 4, 4, 
                              Fade(CurrentCell.sides[touchSide],
                                1/distance + 0.3f));
            }
        }
    }

}

void drawPointInMap(Vector2 point, float r) {
    Vector2 screenPoint = {miniMapCords.x + point.x*MINIMAP_WIDTH/H_SEGMENTS, miniMapCords.y + point.y*MINIMAP_HEIGHT/V_SEGMENTS};
    DrawCircleV(screenPoint, r, rayColor);
}

void drawLineInMap(Vector2 pos1, Vector2 pos2) {
    Vector2 screenPos1 = {miniMapCords.x + pos1.x*spacing_x, miniMapCords.y + pos1.y*spacing_y,};
    Vector2 screenPos2 = {miniMapCords.x + pos2.x*spacing_x, miniMapCords.y + pos2.y*spacing_y};

    DrawLineV(screenPos1, screenPos2, rayColor);
}

void createStripeFromImage(Image *source, Image *dest,  float x) {
    Rectangle crop = {(int)(x), 0, 1 , source->height};
    *dest = ImageCopy(*source);
    ImageCrop(dest, crop);
}

float getWallTouchPoint(Vector2 point) {
    float under = fabsf(point.x - floorf(point.x));
    float upper = fabsf(ceilf(point.x) - point.x);
    float dx = (under > upper)  ? upper : under;
    under = fabsf(point.y - floorf(point.y));
    upper = fabsf(ceilf(point.y) - point.y);
    float dy = (under > upper)  ? upper : under;
    
    if (dx > dy ) return point.x - floorf(point.x);
    else return point.y - floorf(point.y);
}

enum SIDE getTouchSide(Vector2 point) {
    Vector2 Cardinal[4] = {{0.5f, 0.0f},
                            {0.5f, 1},
                            {1, 0.5f},
                            {0, 0.5f}};
    Vector2 newPoint = {point.x - floorf(point.x), point.y - floorf(point.y)};
    float distance = 1000;
    int facing;
    for (int i = 0; i < 4 ; ++i) {
        if (Vector2Distance(newPoint, Cardinal[i]) < distance) {
            facing = i;
            distance = Vector2Distance(newPoint, Cardinal[i]);
        }

    }

    return facing;
}

Vector2 screenCordsToMapCords(Vector2 pos) {
    return (Vector2){miniMapCords.x + pos.x/MINIMAP_WIDTH*H_SEGMENTS, miniMapCords.y + pos.y/MINIMAP_HEIGHT*H_SEGMENTS};
}

Vector2 collision(Vector2 newcords, Vector2 oldCords) {
    if (newcords.x > 0 && newcords.x < gameMap.cols &&
        newcords.y > 0 && newcords.y < gameMap.rows) {
        if (gameMap.cells[(int)newcords.x][(int)newcords.y].bound) {
            return oldCords;
        } 
        else return newcords;
    }
    return oldCords;
}

Cell** createMap(map m) {
    Cell** newMap;
    newMap = calloc(m.rows, sizeof(Cell**));
    for (size_t i = 0; i < m.rows; ++i) {
        newMap[i] = calloc(m.cols, sizeof(Cell));
    }
    return newMap;
}

Cell getRandomCel() {
    Cell c = {.textured = false,
            .bound = true,
            .sides = {(Color){rand()%255, rand()%255, rand()%255, rand()%255},
                    (Color){rand()%255, rand()%255, rand()%255, rand()%255},
                    (Color){rand()%255, rand()%255, rand()%255, rand()%255},
                    (Color){rand()%255, rand()%255, rand()%255, rand()%255}}
            };
    return c;
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

void loadResources() {
    brick = LoadImage("resources/bricks.png");
    BrickColors = LoadImageColors(brick);
}

void unloadResources() {
    UnloadImage(brick);
}
