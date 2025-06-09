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
        checkPromotion(board.cells);
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

    // Sprawdź czy któraś strona nie ma pionków lub ruchów
    if (!board.hasValidMoves(false) || board.countPieces(false) == 0) {
        std::cout << "AI wygrało!" << std::endl;
        return true;
    }
    if (!board.hasValidMoves(true) || board.countPieces(true) == 0) {
        std::cout << "Gracz wygrał!" << std::endl;
        return true;
    }
    return false;
}

void Game::checkPromotion(std::shared_ptr<Piece> cells[Board::SIZE][Board::SIZE]) {
    // Promocja gracza (wiersz 0)
    for (int col = 0; col < 8; ++col) {
        auto& piece = cells[0][col];
        if (piece && !piece->getIsAI() && !piece->getIsKing()) {
            piece->promote();
            std::cout << "Gracz pionek awansował do damki na (0," << col << ")!" << std::endl;
        }
    }
    // Promocja AI (wiersz 7)
    for (int col = 0; col < 8; ++col) {
        auto& piece = cells[7][col];
        if (piece && piece->getIsAI() && !piece->getIsKing()) {
            piece->promote();
            std::cout << "AI pionek awansował do damki na (7," << col << ")!" << std::endl;
        }
    }
}