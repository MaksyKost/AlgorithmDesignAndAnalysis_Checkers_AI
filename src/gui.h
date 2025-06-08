#ifndef GUI_H
#define GUI_H

#include "board.h"
#include <SDL.h>

class GUI {
public:
    GUI();
    ~GUI();
    bool init();
    void render(Board &board);
    void handleEvents();
    bool shouldQuit();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
};

#endif