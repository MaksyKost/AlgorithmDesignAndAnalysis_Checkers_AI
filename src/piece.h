#ifndef PIECE_H
#define PIECE_H

class Piece {
public:
    Piece(bool isAI, bool isKing = false) : isAI(isAI), isKing(isKing) {}

    bool getIsAI() const { return isAI; }
    bool getIsKing() const { return isKing; }
    void promote() { isKing = true; }
    
    char getSymbol() const {
        if (isAI) return isKing ? 'A' : 'a';
        else     return isKing ? 'P' : 'p';
    }

private:
    bool isAI;
    bool isKing;
};

#endif // PIECE_H