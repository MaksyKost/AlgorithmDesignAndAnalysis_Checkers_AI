#include <iostream>
#include <chrono>
#include <fstream>
#include "src/board.h"
#include "src/ai.h"

using namespace std;
using namespace std::chrono;

// Pomiar czasu działania funkcji
template<typename Function>
double measure_time(Function func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    return elapsed.count();
}

// Klasyczny minimax bez przycinania alfa-beta
int minimax_no_pruning(Board board, int depth, bool maximizingPlayer) {
    if (depth == 0) {
        return board.evaluate();
    }
    
    auto moves = board.getValidMoves(maximizingPlayer);
    if (moves.empty()) {
        return maximizingPlayer ? numeric_limits<int>::min() + 1 
                                : numeric_limits<int>::max() - 1;
    }

    if (maximizingPlayer) {
        int maxEval = numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board newBoard = board;
            newBoard.applyMove(move);
            int eval = minimax_no_pruning(newBoard, depth - 1, false);
            maxEval = max(maxEval, eval);
        }
        return maxEval;
    } else {
        int minEval = numeric_limits<int>::max();
        for (const auto& move : moves) {
            Board newBoard = board;
            newBoard.applyMove(move);
            int eval = minimax_no_pruning(newBoard, depth - 1, true);
            minEval = min(minEval, eval);
        }
        return minEval;
    }
}

int main() {
    const Difficulty LEVELS[] = {Difficulty::EASY, Difficulty::MEDIUM, Difficulty::HARD};
    const int TESTS = 50;

    ofstream resultsFile("results/MinimaxComparison.txt");

    if (!resultsFile.is_open()) {
        cerr << "Nie udało się otworzyć pliku do zapisu wyników." << endl;
        return 1;
    }

    Board board;
    AI ai;

    for (Difficulty level : LEVELS) {
        cout << "\n=== Testowanie minimax dla poziomu trudności " 
             << static_cast<int>(level) << " ===\n";
        resultsFile << "\n=== Testowanie minimax dla poziomu trudności " 
                    << static_cast<int>(level) << " ===\n";

        double total_time_pruning = 0.0;
        double total_time_no_pruning = 0.0;

        int depth = static_cast<int>(level); // Konwersja poziomu na głębokość

        for (int t = 0; t < TESTS; t++) {
            total_time_pruning += measure_time([&]() { ai.getBestMove(board, depth); });
            total_time_no_pruning += measure_time([&]() { minimax_no_pruning(board, depth, true); });
        }

        double avg_time_pruning = total_time_pruning / TESTS;
        double avg_time_no_pruning = total_time_no_pruning / TESTS;

        cout << "🔹 Minimax z przycinaniem: " << avg_time_pruning << " s\n";
        cout << "🔸 Minimax bez przycinania: " << avg_time_no_pruning << " s\n";

        resultsFile << "🔹 Minimax z przycinaniem: " << avg_time_pruning << " s\n";
        resultsFile << "🔸 Minimax bez przycinania: " << avg_time_no_pruning << " s\n";
    }

    resultsFile.close();
    return 0;
}