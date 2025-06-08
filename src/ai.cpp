#include "ai.h"
#include <limits>

// Algorytm MinMax z przycinaniem alfa-beta
int AI::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || board.isTerminal()) {
        return board.evaluate();
    }
    
    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        auto moves = board.getValidMoves(true); // ruchy AI
        for (auto move : moves) {
            board.applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.undoMove(move);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // przycinanie
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        auto moves = board.getValidMoves(false); // ruchy gracza
        for (auto move : moves) {
            board.applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.undoMove(move);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break; // przycinanie
        }
        return minEval;
    }
}

Move AI::getBestMove(Board board, int depth) {
    Move bestMove{};
    int bestValue = std::numeric_limits<int>::min();
    auto moves = board.getValidMoves(true); // ruchy AI
    for (auto move : moves) {
        board.applyMove(move);
        int moveValue = minimax(board, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        board.undoMove(move);
        if (moveValue > bestValue) {
            bestValue = moveValue;
            bestMove = move;
        }
    }
    return bestMove;
}