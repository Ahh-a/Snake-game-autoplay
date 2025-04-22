#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BLOCK_SIZE 20
#define GRID_WIDTH (SCREEN_WIDTH / BLOCK_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / BLOCK_SIZE)
#define MAX_TAIL (GRID_WIDTH * GRID_HEIGHT) 
#define DEFAULT_DELAY 100
#define PADDING 2  
extern int gameDelay;
extern bool running;

typedef struct {
    int x, y;
} Point;

typedef struct {
    int x, y;
    int nextX, nextY;
} GridCell;



void DrawText(const char* text, int x, int y);
void GenerateHamiltonianCycle();
void Init();
void Draw();
void Update();
void HandleEvents();
void CleanUp();