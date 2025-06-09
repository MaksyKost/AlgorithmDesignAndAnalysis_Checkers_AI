#ifndef AI_H
#define AI_H

#include "board.h"

enum class Difficulty {
    EASY = 2,    // głębokość 2
    MEDIUM = 4,  // głębokość 4
    HARD = 8     // głębokość 8
};

class AI {
public:
    Move getBestMove(Board board, Difficulty difficulty);
    Move getBestMove(Board board, int depth = 5);

private:
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);
};

#endif // AI_H