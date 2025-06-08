#include "board.h"

Board::Board() { 
    initBoard();
}

void Board::initBoard() {
    // Inicjalizacja planszy: ustawienie pustych pól
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            board[i][j] = EMPTY;

    // Ustawienie pionków AI (np. na górnych 3 rzędach) i gracza (dolne 3 rzedy)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 8; ++j)
            if ((i + j) % 2 == 1)
                board[i][j] = AI;
    }
    for (int i = 5; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
            if ((i + j) % 2 == 1)
                board[i][j] = PLAYER;
    }
}

std::vector<Move> Board::getValidMoves(bool forAI) {
    std::vector<Move> moves;
    // Tutaj musisz zaimplementować logikę generowania legalnych ruchów
    // - ruchy zwykłych pionków (przekątne, zbicia)
    // - ruchy damki
    return moves;
}

bool Board::applyMove(const Move &move) {
    board[move.toX][move.toY] = board[move.fromX][move.fromY];
    board[move.fromX][move.fromY] = EMPTY;
    // Dodatkowo: jeśli ruch wiąże się z biciem, usuń zbity pionek
    return true;
}

void Board::undoMove(const Move &move) {
    // Cofnij ruch – pamiętaj, że przy kopiowaniu ruchów trzeba przechować informację o zbitych pionkach
    board[move.fromX][move.fromY] = board[move.toX][move.toY];
    board[move.toX][move.toY] = EMPTY;
}

int Board::evaluate() const {
    int score = 0;
    // Przykładowa heurystyka: dodaj punkty za pionki AI, odejmij za pionki gracza
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == PLAYER)
                score -= 10;
            else if (board[i][j] == AI)
                score += 10;
            else if (board[i][j] == PLAYER_KING)
                score -= 15;
            else if (board[i][j] == AI_KING)
                score += 15;
        }
    return score;
}

bool Board::isTerminal() const {
    // Prosty warunek zakończenia: jeden z graczy nie ma pionków
    int playerCount = 0, aiCount = 0;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == PLAYER || board[i][j] == PLAYER_KING)
                playerCount++;
            if (board[i][j] == AI || board[i][j] == AI_KING)
                aiCount++;
        }
    return (playerCount == 0 || aiCount == 0);
}

void Board::printBoard() const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
            std::cout << board[i][j] << " ";
        std::cout << std::endl;
    }
}

PieceType Board::getPieceAt(int i, int j) const {
    return board[i][j];
}