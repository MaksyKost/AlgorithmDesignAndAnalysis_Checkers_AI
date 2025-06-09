#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "ai.h"
#include "gui.h"

class Game {
private:
    Board board;
    AI ai;
    GUI gui;
    bool gameOver;
    bool playersTurn; // true = ruch gracza, false = ruch AI
public:
    Game();
    ~Game();
    void init();
    void handleInput();
    void update();
    void render();
    bool isGameOver() const;
    void clean();

    // Umożliwia sprawdzenie, czy okno powinno się zamknąć
    bool shouldQuit() const { return gui.shouldQuit(); }

};

#endif