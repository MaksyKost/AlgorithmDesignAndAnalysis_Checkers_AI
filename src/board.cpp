#include "board.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Board::Board() {
    init();
}

void Board::init() {
    // Wyczyść planszę
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cells[i][j] = nullptr;
        }
    }
    
    // Umieść pionki gracza (na dole, wiersze 5-7)
    for (int row = 5; row < 8; row++) {
        for (int col = 0; col < SIZE; col++) {
            if ((row + col) % 2 == 1) { // Tylko na ciemnych polach
                cells[row][col] = std::make_shared<Piece>(false, false);
            }
        }
    }
    
    // Umieść pionki AI (na górze, wiersze 0-2)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < SIZE; col++) {
            if ((row + col) % 2 == 1) { // Tylko na ciemnych polach
                cells[row][col] = std::make_shared<Piece>(true, false);
            }
        }
    }
}

std::vector<Move> Board::getValidMoves(bool forAI) const {
    std::vector<Move> allMoves;
    std::vector<Move> captureMoves;
    
    // Znajdź wszystkie możliwe ruchy
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (cells[row][col] && cells[row][col]->getIsAI() == forAI) {
                auto pieceMoves = getPieceMovesAndCaptures(row, col, forAI);
                for (const auto& move : pieceMoves) {
                    if (!move.capturedPositions.empty()) {
                        captureMoves.push_back(move);
                    } else {
                        allMoves.push_back(move);
                    }
                }
            }
        }
    }
    
    // Jeśli są bicia, zwróć tylko bicia (zgodnie z regułami warcabów)
    if (!captureMoves.empty()) {
        return captureMoves;
    }
    
    return allMoves;
}

std::vector<Move> Board::getPieceMovesAndCaptures(int row, int col, bool forAI) const {
    std::vector<Move> moves;
    auto piece = cells[row][col];
    
    if (!piece || piece->getIsAI() != forAI) {
        return moves;
    }
    
    // Kierunki ruchu dla zwykłych pionków
    std::vector<std::pair<int, int>> directions;
    
    if (piece->getIsKing()) {
        // Damka może się poruszać we wszystkich kierunkach
        directions = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    } else {
        // Zwykły pionek
        if (forAI) {
            // AI porusza się w dół
            directions = {{1, -1}, {1, 1}};
        } else {
            // Gracz porusza się w górę
            directions = {{-1, -1}, {-1, 1}};
        }
    }
    
    // Najpierw sprawdź bicia
    std::vector<Move> captureMoves = getCaptureMoves(row, col, forAI);
    if (!captureMoves.empty()) {
        return captureMoves;
    }
    
    // Jeśli nie ma bić, dodaj zwykłe ruchy
    return getSimpleMoves(row, col, forAI);
}

std::vector<Move> Board::getCaptureMoves(int row, int col, bool forAI) const {
    std::vector<Move> captureMoves;
    auto piece = cells[row][col];
    
    if (!piece) return captureMoves;
    
    std::vector<std::pair<int, int>> directions;
    if (piece->getIsKing()) {
        directions = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    } else {
        if (forAI) {
            directions = {{1, -1}, {1, 1}};
        } else {
            directions = {{-1, -1}, {-1, 1}};
        }
    }
    
    for (const auto& dir : directions) {
        if (canCapture(row, col, dir.first, dir.second, forAI)) {
            int enemyRow = row + dir.first;
            int enemyCol = col + dir.second;
            int jumpRow = row + 2 * dir.first;
            int jumpCol = col + 2 * dir.second;
            
            Move move(row, col, jumpRow, jumpCol);
            move.capturedPositions.push_back({enemyRow, enemyCol});
            captureMoves.push_back(move);
        }
    }
    
    return captureMoves;
}

std::vector<Move> Board::getSimpleMoves(int row, int col, bool forAI) const {
    std::vector<Move> simpleMoves;
    auto piece = cells[row][col];
    
    if (!piece) return simpleMoves;
    
    std::vector<std::pair<int, int>> directions;
    if (piece->getIsKing()) {
        directions = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    } else {
        if (forAI) {
            directions = {{1, -1}, {1, 1}};
        } else {
            directions = {{-1, -1}, {-1, 1}};
        }
    }
    
    for (const auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        
        if (isValidMove(row, col, newRow, newCol, forAI)) {
            simpleMoves.push_back(Move(row, col, newRow, newCol));
        }
    }
    
    return simpleMoves;
}

bool Board::canCapture(int row, int col, int deltaRow, int deltaCol, bool forAI) const {
    int enemyRow = row + deltaRow;
    int enemyCol = col + deltaCol;
    int jumpRow = row + 2 * deltaRow;
    int jumpCol = col + 2 * deltaCol;
    
    // Sprawdź czy pozycje są w granicach planszy
    if (!isValidPosition(enemyRow, enemyCol) || !isValidPosition(jumpRow, jumpCol)) {
        return false;
    }
    
    // Sprawdź czy na pozycji enemy jest przeciwnik
    if (!cells[enemyRow][enemyCol] || cells[enemyRow][enemyCol]->getIsAI() == forAI) {
        return false;
    }
    
    // Sprawdź czy pozycja docelowa jest pusta
    if (cells[jumpRow][jumpCol]) {
        return false;
    }
    
    return true;
}

bool Board::isValidMove(int srcRow, int srcCol, int dstRow, int dstCol, bool forAI) const {
    // Sprawdź granice planszy
    if (!isValidPosition(srcRow, srcCol) || !isValidPosition(dstRow, dstCol)) {
        return false;
    }
    
    // Sprawdź czy pole źródłowe ma pionek gracza
    auto piece = cells[srcRow][srcCol];
    if (!piece || piece->getIsAI() != forAI) {
        return false;
    }
    
    // Sprawdź czy pole docelowe jest puste
    if (cells[dstRow][dstCol]) {
        return false;
    }
    
    // Sprawdź czy ruch jest po przekątnej o jedną pozycję
    int deltaRow = abs(dstRow - srcRow);
    int deltaCol = abs(dstCol - srcCol);
    if (deltaRow != 1 || deltaCol != 1) {
        return false;
    }
    
    // Sprawdź kierunek ruchu dla zwykłych pionków
    if (!piece->getIsKing()) {
        if (forAI && dstRow <= srcRow) {
            return false; // AI musi się poruszać w dół
        }
        if (!forAI && dstRow >= srcRow) {
            return false; // Gracz musi się poruszać w górę
        }
    }
    return true;
}

void Board::applyMove(const Move& move) {
    if (!isValidPosition(move.srcRow, move.srcCol) || 
        !isValidPosition(move.dstRow, move.dstCol)) {
        return;
    }
    
    auto piece = cells[move.srcRow][move.srcCol];
    if (!piece) return;
    
    // Przesuń pionek
    cells[move.dstRow][move.dstCol] = piece;
    cells[move.srcRow][move.srcCol] = nullptr;
    
    // Usuń zbite pionki
    for (const auto& capturedPos : move.capturedPositions) {
        cells[capturedPos.first][capturedPos.second] = nullptr;
    }
    
    // Sprawdź promocję do damki
    if (!piece->getIsKing()) {
        if ((piece->getIsAI() && move.dstRow == SIZE - 1) ||
            (!piece->getIsAI() && move.dstRow == 0)) {
            piece->promote();
        }
    }
}

void Board::undoMove(const Move& move, const std::vector<std::shared_ptr<Piece>>& capturedPieces) {
    // Przywróć pionek na początkową pozycję
    auto piece = cells[move.dstRow][move.dstCol];
    cells[move.srcRow][move.srcCol] = piece;
    cells[move.dstRow][move.dstCol] = nullptr;
    
    // Przywróć zbite pionki
    for (size_t i = 0; i < move.capturedPositions.size() && i < capturedPieces.size(); i++) {
        const auto& pos = move.capturedPositions[i];
        cells[pos.first][pos.second] = capturedPieces[i];
    }
}

int Board::evaluate() const {
    int score = 0;
    
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (cells[row][col]) {
                int pieceValue = cells[row][col]->getIsKing() ? 30 : 10;
                
                if (cells[row][col]->getIsAI()) {
                    score += pieceValue;
                    // Bonus za pozycję (środek planszy)
                    score += static_cast<int>(3 - abs(row - 3.5)) + static_cast<int>(3 - abs(col - 3.5));
                } else {
                    score -= pieceValue;
                    // Kara za pozycję przeciwnika
                    score -= static_cast<int>(3 - abs(row - 3.5)) + static_cast<int>(3 - abs(col - 3.5));
                }
            }
        }
    }
    
    return score;
}

void Board::printBoard() const {
    std::cout << "  0 1 2 3 4 5 6 7" << std::endl;
    for (int i = 0; i < SIZE; i++) {
        std::cout << i << " ";
        for (int j = 0; j < SIZE; j++) {
            if (cells[i][j]) {
                std::cout << cells[i][j]->getSymbol() << " ";
            } else {
                std::cout << ((i + j) % 2 == 0 ? "□ " : "■ ");
            }
        }
        std::cout << std::endl;
    }
}

std::shared_ptr<Piece> Board::getPiece(int row, int col) const {
    if (isValidPosition(row, col)) {
        return cells[row][col];
    }
    return nullptr;
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool Board::hasValidMoves(bool forAI) const {
    return !getValidMoves(forAI).empty();
}

int Board::countPieces(bool forAI) const {
    int count = 0;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (cells[row][col] && cells[row][col]->getIsAI() == forAI) {
                count++;
            }
        }
    }
    return count;
}