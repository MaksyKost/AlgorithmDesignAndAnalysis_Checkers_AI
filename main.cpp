#include "src/game.h"
#include <SDL.h> // wymagane do SDL_Delay

int main(int argc, char* argv[]) {
    Game game;
    game.init();

    while (!game.isGameOver() && !game.shouldQuit()) {
        game.handleInput();
        game.update();
        game.render();
        SDL_Delay(50); // opóźnienie kontrolujące pętlę gry
    }
    
    game.clean();
    return 0;
}