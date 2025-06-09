#ifndef AI_H
#define AI_H

#include "board.h"

class AI {
public:
    Move getBestMove(Board board, int depth = 5);

private:
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer);
};

#endif // AI_H