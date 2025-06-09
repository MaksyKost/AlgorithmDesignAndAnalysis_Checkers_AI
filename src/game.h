#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "ai.h"

class Game {
public:
    Game();
    ~Game() = default;

    // Główna pętla gry
    void run();
    void checkPromotion(std::shared_ptr<Piece> cells[Board::SIZE][Board::SIZE]);

private:
    Board board;
    AI ai;
    bool turnAI; // true, gdy kolej AI, false dla gracza

    void processTurn();
    bool checkVictory();
};

#endif // GAME_H