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
    return getMultiCaptureMoves(row, col, forAI);
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
        if (piece->getIsKing()) {
            // Damka może się poruszać na dowolną odległość
            for (int distance = 1; distance < SIZE; distance++) {
                int newRow = row + dir.first * distance;
                int newCol = col + dir.second * distance;
                
                if (!isValidPosition(newRow, newCol)) break;
                if (cells[newRow][newCol]) break; // Pole zajęte
                
                simpleMoves.push_back(Move(row, col, newRow, newCol));
            }
        } else {
            // Zwykły pionek - tylko jedno pole
            int newRow = row + dir.first;
            int newCol = col + dir.second;
            
            if (isValidMove(row, col, newRow, newCol, forAI)) {
                simpleMoves.push_back(Move(row, col, newRow, newCol));
            }
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

bool Board::canCaptureKing(int row, int col, int deltaRow, int deltaCol, bool forAI, int& enemyRow, int& enemyCol, int& jumpRow, int& jumpCol) const {
    for (int distance = 1; distance < SIZE - 1; distance++) {
        int checkRow = row + deltaRow * distance;
        int checkCol = col + deltaCol * distance;
        
        if (!isValidPosition(checkRow, checkCol)) return false;
        
        if (cells[checkRow][checkCol]) {
            // Znaleziono pionek
            if (cells[checkRow][checkCol]->getIsAI() != forAI) {
                // To przeciwnik - sprawdź czy można go zbić
                enemyRow = checkRow;
                enemyCol = checkCol;
                
                // Sprawdź wszystkie możliwe pozycje lądowania za przeciwnikiem
                for (int landDistance = 1; landDistance < SIZE; landDistance++) {
                    jumpRow = enemyRow + deltaRow * landDistance;
                    jumpCol = enemyCol + deltaCol * landDistance;
                    
                    if (!isValidPosition(jumpRow, jumpCol)) break;
                    if (cells[jumpRow][jumpCol]) break;
                    
                    return true; // Można zbić
                }
            }
            return false; // Własny pionek lub nie można zbić
        }
    }
    return false;
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
    
    // Usuń WSZYSTKIE zbite pionki
    for (const auto& capturedPos : move.capturedPositions) {
        if (isValidPosition(capturedPos.first, capturedPos.second)) {
            cells[capturedPos.first][capturedPos.second] = nullptr;
        }
    }
    
    // Sprawdź promocję do damki na podstawie współrzędnych
    //checkPromotion(move, piece);
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
                int pieceValue = 10;
                int kingValue = 50;
                int value = cells[row][col]->getIsKing() ? kingValue : pieceValue;

                // Premia za bliskość promocji (dla pionków)
                if (!cells[row][col]->getIsKing()) {
                    if (cells[row][col]->getIsAI()) {
                        value += row; // im bliżej 7, tym lepiej dla AI
                    } else {
                        value += (7 - row); // im bliżej 0, tym lepiej dla gracza
                    }
                }

                // Premia za centrum planszy
                int centerBonus = 3 - std::abs(3 - col);
                value += centerBonus;

                // Kara za samotność (brak sąsiadów po bokach)
                bool lonely = true;
                for (int d = -1; d <= 1; d += 2) {
                    int ncol = col + d;
                    if (ncol >= 0 && ncol < SIZE && cells[row][ncol] && cells[row][ncol]->getIsAI() == cells[row][col]->getIsAI())
                        lonely = false;
                }
                if (lonely) value -= 2;

                // Premia za możliwość bicia
                if (canCapture(row, col, 1, 1, cells[row][col]->getIsAI()) ||
                    canCapture(row, col, 1, -1, cells[row][col]->getIsAI()) ||
                    canCapture(row, col, -1, 1, cells[row][col]->getIsAI()) ||
                    canCapture(row, col, -1, -1, cells[row][col]->getIsAI())) {
                    value += 5;
                }

                if (cells[row][col]->getIsAI()) {
                    score += value;
                } else {
                    score -= value;
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

std::vector<Move> Board::getMultiCaptureMoves(int row, int col, bool forAI, Move currentMove) const {
    std::vector<Move> allCaptures;
    auto piece = cells[row][col];

    if (!piece) return allCaptures;

    std::vector<std::pair<int, int>> directions;
    if (piece->getIsKing()) {
        directions = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    } else {
        directions = forAI ? std::vector<std::pair<int, int>>{{1, -1}, {1, 1}} : std::vector<std::pair<int, int>>{{-1, -1}, {-1, 1}};
    }

    bool hasCaptures = false;

    for (const auto& dir : directions) {
        if (piece->getIsKing()) {
            int step = 1;
            while (true) {
                int enemyRow = row + step * dir.first;
                int enemyCol = col + step * dir.second;
                int jumpRow = row + (step + 1) * dir.first;
                int jumpCol = col + (step + 1) * dir.second;

                if (!isValidPosition(enemyRow, enemyCol) || !isValidPosition(jumpRow, jumpCol)) break;
                if (!cells[enemyRow][enemyCol]) { 
                    step++; 
                    continue; 
                }

                if (canCaptureKing(row, col, dir.first, dir.second, forAI, enemyRow, enemyCol, jumpRow, jumpCol)) {
                    bool alreadyCaptured = false;
                    for (const auto& capturedPos : currentMove.capturedPositions) {
                        if (capturedPos.first == enemyRow && capturedPos.second == enemyCol) {
                            alreadyCaptured = true;
                            break;
                        }
                    }

                    if (!alreadyCaptured) {
                        hasCaptures = true;

                        Move newMove = currentMove;
                        if (newMove.srcRow == -1) {
                            newMove.srcRow = row;
                            newMove.srcCol = col;
                        }
                        newMove.dstRow = jumpRow;
                        newMove.dstCol = jumpCol;
                        newMove.capturedPositions.push_back({enemyRow, enemyCol});

                        Board tempBoard = *this;
                        tempBoard.cells[row][col] = nullptr;
                        tempBoard.cells[jumpRow][jumpCol] = piece;
                        tempBoard.cells[enemyRow][enemyCol] = nullptr;

                        auto furtherCaptures = tempBoard.getMultiCaptureMoves(jumpRow, jumpCol, forAI, newMove);
                        
                        if (furtherCaptures.empty()) {
                            allCaptures.push_back(newMove);
                        } else {
                            allCaptures.insert(allCaptures.end(), furtherCaptures.begin(), furtherCaptures.end());
                        }
                    }
                }
                break;
            }
        } else {
            if (canCapture(row, col, dir.first, dir.second, forAI)) {
                int enemyRow = row + dir.first;
                int enemyCol = col + dir.second;
                int jumpRow = row + 2 * dir.first;
                int jumpCol = col + 2 * dir.second;

                bool alreadyCaptured = false;
                for (const auto& capturedPos : currentMove.capturedPositions) {
                    if (capturedPos.first == enemyRow && capturedPos.second == enemyCol) {
                        alreadyCaptured = true;
                        break;
                    }
                }

                if (!alreadyCaptured) {
                    hasCaptures = true;

                    Move newMove = currentMove;
                    if (newMove.srcRow == -1) {
                        newMove.srcRow = row;
                        newMove.srcCol = col;
                    }
                    newMove.dstRow = jumpRow;
                    newMove.dstCol = jumpCol;
                    newMove.capturedPositions.push_back({enemyRow, enemyCol});

                    Board tempBoard = *this;
                    tempBoard.cells[row][col] = nullptr;
                    tempBoard.cells[jumpRow][jumpCol] = piece;
                    tempBoard.cells[enemyRow][enemyCol] = nullptr;

                    auto furtherCaptures = tempBoard.getMultiCaptureMoves(jumpRow, jumpCol, forAI, newMove);

                    if (furtherCaptures.empty()) {
                        allCaptures.push_back(newMove);
                    } else {
                        allCaptures.insert(allCaptures.end(), furtherCaptures.begin(), furtherCaptures.end());
                    }
                }
            }
        }
    }

    if (currentMove.srcRow == -1 && !hasCaptures) return allCaptures;
    if (currentMove.srcRow != -1 && !hasCaptures) allCaptures.push_back(currentMove);

    return allCaptures;
}
/*
void Board::checkPromotion(const Move& move, std::shared_ptr<Piece> piece) {
    if (!piece->getIsKing()) {
        if (piece->getIsAI() && move.dstRow == 7) {
            piece->promote();
        }
        else if (!(piece->getIsAI()) && move.dstRow == 0) {
            piece->promote();
        }
    }
}*/