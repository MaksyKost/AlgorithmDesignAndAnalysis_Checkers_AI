#include "game.h"
#include <iostream>

Game::Game() : turnAI(false) { 
    // Rozpocznij grę – plansza już inicjalizowana przez konstruktor Board
}

void Game::run() {
    while (!checkVictory()) {
        board.printBoard();
        processTurn();
        turnAI = !turnAI;
    }
    std::cout << "Koniec gry!" << std::endl;
}

void Game::processTurn() {
    if (turnAI) {
        // Dla AI wykorzystaj funkcję getBestMove
        Move bestMove = ai.getBestMove(board, 5);
        board.applyMove(bestMove);
        std::cout << "AI wykonało ruch" << std::endl;
    } else {
        // Opcja użytkownika – pobranie ruchu z GUI lub konsoli
        // Na potrzeby demonstracji możemy pobrać dane wejściowe z konsoli
        Move move;
        std::cout << "Podaj ruch (srcRow srcCol dstRow dstCol): ";
        std::cin >> move.srcRow >> move.srcCol >> move.dstRow >> move.dstCol;
        board.applyMove(move);
    }
}

bool Game::checkVictory() {
    // Przykładowa logika – sprawdzenie warunków zwycięstwa
    // np. brak ruchów lub pionków którejś ze stron
    return false;
}