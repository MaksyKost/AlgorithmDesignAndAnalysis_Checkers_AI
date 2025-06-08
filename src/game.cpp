#include "game.h"
#include <iostream>

Game::Game() : gameOver(false), playersTurn(true) {}

Game::~Game() {
    clean();
}

void Game::init() {
    board.initBoard();
    if (!gui.init()) {
        std::cerr << "Błąd inicjalizacji GUI." << std::endl;
        gameOver = true;
    }
}

void Game::handleInput() {
    gui.handleEvents();
    // Jeśli to ruch gracza, pobierz dane – poniżej uproszczony przykład wejścia tekstowego
    if (playersTurn) {
        int sx, sy, dx, dy;
        std::cout << "Podaj współrzędne ruchu (sx sy dx dy): ";
        std::cin >> sx >> sy >> dx >> dy;
        Move move { sx, sy, dx, dy };
        // Tu możesz dodać walidację poprawności ruchu:
        if (true /* lub warunek poprawności */) {
            board.applyMove(move);
            playersTurn = false;
        }
    }
}

void Game::update() {
    // Jeśli to ruch AI, wybierz i wykonaj ruch
    if (!playersTurn && !gameOver) {
        Move bestMove = ai.getBestMove(board, 3); // głębokość np. 3
        board.applyMove(bestMove);
        playersTurn = true;
    }

    // Aktualizacja stanu gry
    if (board.isTerminal())
        gameOver = true;
}

void Game::render() {
    gui.render(board);
}

bool Game::isGameOver() const {
    return gameOver;
}

void Game::clean() {
    gui.clean();
}