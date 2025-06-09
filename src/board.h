#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include "piece.h"

struct Move {
    int srcRow, srcCol;
    int dstRow, dstCol;
    std::vector<std::pair<int, int>> capturedPositions; // Pozycje zbitych pionków
    
    Move() : srcRow(-1), srcCol(-1), dstRow(-1), dstCol(-1) {}
    Move(int sr, int sc, int dr, int dc) : srcRow(sr), srcCol(sc), dstRow(dr), dstCol(dc) {}
};

class Board {
public:
    static const int SIZE = 8;
    std::shared_ptr<Piece> cells[SIZE][SIZE]; // Plansza 8x8 z pionkami

    Board();
    ~Board() = default;

    void init();
    std::vector<Move> getValidMoves(bool forAI) const;

    

    void applyMove(const Move& move);
    void undoMove(const Move& move, const std::vector<std::shared_ptr<Piece>>& capturedPieces);
    int evaluate() const;
    void printBoard() const;

    //void checkPromotion(const Move& move, std::shared_ptr<Piece> piece);
    
    // Dodatkowe metody dla GUI
    std::shared_ptr<Piece> getPiece(int row, int col) const;
    bool isValidPosition(int row, int col) const;
    bool hasValidMoves(bool forAI) const;
    int countPieces(bool forAI) const;

private:
    
    
    // Funkcje pomocnicze
    std::vector<Move> getPieceMovesAndCaptures(int row, int col, bool forAI) const;
    std::vector<Move> getCaptureMoves(int row, int col, bool forAI) const;
    std::vector<Move> getSimpleMoves(int row, int col, bool forAI) const;

    std::vector<Move> getMultiCaptureMoves(int row, int col, bool forAI, Move currentMove = Move()) const;


    bool canCapture(int row, int col, int deltaRow, int deltaCol, bool forAI) const;
    bool isValidMove(int srcRow, int srcCol, int dstRow, int dstCol, bool forAI) const;

    bool canCaptureKing(int row, int col, int deltaRow, int deltaCol, bool forAI, int& enemyRow, int& enemyCol, int& jumpRow, int& jumpCol) const;
};

#endif // BOARD_H