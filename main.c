#include "snake.h"
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    Init();  // Use o nome correto
    
    while (running) {  // Variável global declarada em snake.h
        HandleEvents();
        Update();      // Use o nome correto
        Draw();        // Use o nome correto
        SDL_Delay(gameDelay);  // Variável global declarada em snake.h
    }
    
    CleanUp();
    return 0;
}