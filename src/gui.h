#ifndef GUI_H
#define GUI_H

#include "board.h"
#include "ai.h"
#include <SDL_ttf.h>
#include <string>

class GUI {
public:
    GUI();
    ~GUI();

    bool init();
    void run(Board &board);
    void close();

    void checkPromotion(std::shared_ptr<Piece> cells[Board::SIZE][Board::SIZE]);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;  // Dodane pole dla czcionki
    AI ai;

    Difficulty currentDifficulty = Difficulty::MEDIUM;
    
    bool gameRunning;
    bool playerTurn;
    bool gameOver;
    std::string gameStatus;
    
    // Stan wyboru pionka
    int selectedRow;
    int selectedCol;
    bool pieceSelected;
    std::vector<Move> validMoves;

    int moveCount = 0;
    
    // Stałe dla wyświetlania
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    static const int BOARD_SIZE = 480;
    static const int CELL_SIZE = BOARD_SIZE / 8;
    static const int BOARD_OFFSET_X = 50;
    static const int BOARD_OFFSET_Y = 50;

    void render(Board &board);
    void handleEvents(Board &board);
    void drawBoard();
    void drawPieces(Board &board);
    void drawSelectedCell();
    void drawValidMoves();
    void drawUI(Board& board);
    
    // Funkcje pomocnicze
    void getCellFromMouse(int mouseX, int mouseY, int &row, int &col);
    bool isValidCell(int row, int col);
    void selectPiece(Board &board, int row, int col);
    void makeMove(Board &board, int row, int col);
    void processAITurn(Board &board);
    void checkGameEnd(Board &board);
    void resetSelection();
    Board& getBoardRef(); // Funkcja pomocnicza
    
    // Kolory
    void setColor(int r, int g, int b, int a = 255);

    void drawText(const std::string& text, int x, int y, SDL_Color color);

    // Stałe dla przycisków
    static const int BUTTON_WIDTH = 80;
    static const int BUTTON_HEIGHT = 30;
    static const int BUTTON_SPACING = 10;

    // Nowe funkcje
    void drawDifficultyButtons();
    void handleDifficultyClick(int mouseX, int mouseY);
    SDL_Rect getButtonRect(int buttonIndex); // 0=Easy, 1=Medium, 2=Hard
};

#endif // GUI_H