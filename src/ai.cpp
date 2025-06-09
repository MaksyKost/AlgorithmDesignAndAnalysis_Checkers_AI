#include "ai.h"
#include <limits>
#include <algorithm>

Move AI::getBestMove(Board board, int depth) {
    Move bestMove;
    int bestValue = std::numeric_limits<int>::min();
    auto moves = board.getValidMoves(true);

    if (moves.empty()) {
        return bestMove; // Brak możliwych ruchów
    }

    for (const auto& move : moves) {
        Board newBoard = board;
        newBoard.applyMove(move);
        
        int moveValue = minimax(newBoard, depth - 1, 
                               std::numeric_limits<int>::min(),
                               std::numeric_limits<int>::max(), false);
        
        if (moveValue > bestValue) {
            bestValue = moveValue;
            bestMove = move;
        }
    }
    
    return bestMove;
}

int AI::minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
    // Warunki końcowe
    if (depth == 0) {
        return board.evaluate();
    }
    
    // Sprawdź czy gra się skończyła
    auto moves = board.getValidMoves(maximizingPlayer);
    if (moves.empty()) {
        // Brak ruchów = przegrana
        return maximizingPlayer ? std::numeric_limits<int>::min() + 1 : 
                                 std::numeric_limits<int>::max() - 1;
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        
        for (const auto& move : moves) {
            Board newBoard = board;
            newBoard.applyMove(move);
            
            int eval = minimax(newBoard, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            
            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        
        for (const auto& move : moves) {
            Board newBoard = board;
            newBoard.applyMove(move);
            
            int eval = minimax(newBoard, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            
            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        
        return minEval;
    }
}

Move AI::getBestMove(Board board, Difficulty difficulty) {
    return getBestMove(board, static_cast<int>(difficulty));
}