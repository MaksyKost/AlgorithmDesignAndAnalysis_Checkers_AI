#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>

enum PieceType { EMPTY, PLAYER, AIP, PLAYER_KING, AI_KING };

struct Move {
    int fromX, fromY;
    int toX, toY;
    // Możesz dodać dodatkowe pola, np. informacje o zbitym pionku
};

class Board {
public:
    Board();
    void initBoard();
    std::vector<Move> getValidMoves(bool forAI);
    bool applyMove(const Move &move);
    void undoMove(const Move &move);
    int evaluate() const;           // Heurystyka oceniająca pozycję
    bool isTerminal() const;        // Sprawdza, czy gra się zakończyła
    void printBoard() const;
    PieceType getPieceAt(int i, int j) const; // Ułatwia odczyt pionka dla GUI

private:
    PieceType board[8][8];
};

#endif