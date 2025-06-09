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
    void setDifficulty(Difficulty diff) { difficulty = diff; }

private:
    Board board;
    AI ai;
    bool turnAI; // true, gdy kolej AI, false dla gracza
    Difficulty difficulty = Difficulty::MEDIUM; // domyślnie średni

    void processTurn();
    bool checkVictory();
    void showDifficultyMenu(); // nowa funkcja
};

#endif // GAME_H