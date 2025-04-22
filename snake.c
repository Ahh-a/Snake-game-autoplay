#include "snake.h"


Point snake[MAX_TAIL];
int snakeLength = 1;
Point fruit;
int dirX = 1, dirY = 0;
int score = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Matriz para armazenar o ciclo hamiltoniano
GridCell hamiltonianCycle[GRID_WIDTH][GRID_HEIGHT];
int gameDelay = DEFAULT_DELAY; 
bool running = true;
bool gameStarted = false;

bool gameWon = false; 

TTF_Font* font = NULL;


void DrawText(const char* text, int x, int y) {
    if (!font) return;
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void GenerateHamiltonianCycle() {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            hamiltonianCycle[x][y].x = x;
            hamiltonianCycle[x][y].y = y;
            hamiltonianCycle[x][y].nextX = -1;
            hamiltonianCycle[x][y].nextY = -1;
        }
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        if (y % 2 == 0) {
            for (int x = 0; x < GRID_WIDTH - 1; x++) {
                hamiltonianCycle[x][y].nextX = x + 1;
                hamiltonianCycle[x][y].nextY = y;
            }
            if (y < GRID_HEIGHT - 1) {
                hamiltonianCycle[GRID_WIDTH - 1][y].nextX = GRID_WIDTH - 1;
                hamiltonianCycle[GRID_WIDTH - 1][y].nextY = y + 1;
            }
        } else {
            for (int x = GRID_WIDTH - 1; x > 0; x--) {
                hamiltonianCycle[x][y].nextX = x - 1;
                hamiltonianCycle[x][y].nextY = y;
            }
            if (y < GRID_HEIGHT - 1) {
                hamiltonianCycle[0][y].nextX = 0;
                hamiltonianCycle[0][y].nextY = y + 1;
            }
        }
    }

    hamiltonianCycle[0][GRID_HEIGHT - 1].nextX = 0;
    hamiltonianCycle[0][GRID_HEIGHT - 1].nextY = 0;
}

void Init() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init(); 
    
    window = SDL_CreateWindow("Snake SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
    }
    
    srand(time(NULL));

    GenerateHamiltonianCycle();

    snake[0].x = SCREEN_WIDTH / 2;
    snake[0].y = SCREEN_HEIGHT / 2;

    fruit.x = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    fruit.y = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

void Draw() {
    
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); 
    SDL_RenderClear(renderer);


    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);  
    for (int x = 0; x < SCREEN_WIDTH; x += BLOCK_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }
    for (int y = 0; y < SCREEN_HEIGHT; y += BLOCK_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect fruitRect = { 
        fruit.x + PADDING, 
        fruit.y + PADDING, 
        BLOCK_SIZE - (2 * PADDING), 
        BLOCK_SIZE - (2 * PADDING) 
    };
    SDL_RenderFillRect(renderer, &fruitRect);


    for (int i = 0; i < snakeLength; i++) {

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect outerRect = { 
            snake[i].x + PADDING, 
            snake[i].y + PADDING, 
            BLOCK_SIZE - (2 * PADDING), 
            BLOCK_SIZE - (2 * PADDING) 
        };
        SDL_RenderFillRect(renderer, &outerRect);

        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_Rect innerRect = { 
            snake[i].x + PADDING + 1, 
            snake[i].y + PADDING + 1, 
            BLOCK_SIZE - (2 * PADDING) - 2, 
            BLOCK_SIZE - (2 * PADDING) - 2 
        };
        SDL_RenderFillRect(renderer, &innerRect);
    }

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Pontos: %d", score);
    DrawText(scoreText, SCREEN_WIDTH - 150, 10);

    if (!gameStarted) {
        DrawText("Aperte algo", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 15);
    }

    if (gameWon) {
        DrawText("VITÓRIA! Você completou o jogo!", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 15);
    }


    SDL_RenderPresent(renderer);
}

void Update() {
    if (!gameStarted) return;

    if (snakeLength >= MAX_TAIL) {
        gameWon = true;
        return;
    }

    int currentGridX = snake[0].x / BLOCK_SIZE;
    int currentGridY = snake[0].y / BLOCK_SIZE;
    
    int nextGridX = hamiltonianCycle[currentGridX][currentGridY].nextX;
    int nextGridY = hamiltonianCycle[currentGridX][currentGridY].nextY;

    dirX = nextGridX - currentGridX;
    dirY = nextGridY - currentGridY;

    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i-1];

    snake[0].x = nextGridX * BLOCK_SIZE;
    snake[0].y = nextGridY * BLOCK_SIZE;

    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        if (snakeLength < MAX_TAIL)
            snakeLength++;
        score++;
        fruit.x = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        fruit.y = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
    }
}

void HandleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            running = false;
        else if (e.type == SDL_KEYDOWN) {
            if (!gameStarted) {
                gameStarted = true;
                break;
            }
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (dirY != 1) { dirX = 0; dirY = -1; }
                    break;
                case SDLK_DOWN:
                    if (dirY != -1) { dirX = 0; dirY = 1; }
                    break;
                case SDLK_LEFT:
                    if (dirX != 1) { dirX = -1; dirY = 0; }
                    break;
                case SDLK_RIGHT:
                    if (dirX != -1) { dirX = 1; dirY = 0; }
                    break;
                case SDLK_EQUALS:
                    gameDelay -= 10;
                    printf("Velocidade aumentada! Delay atual: %dms\n", gameDelay);
                    break;
                case SDLK_MINUS:
                    gameDelay += 10;
                    printf("Velocidade diminuída! Delay atual: %dms\n", gameDelay);
                    break;
            }
        }
    }
}

void CleanUp() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Pontuação final: %d\n", score);
}

