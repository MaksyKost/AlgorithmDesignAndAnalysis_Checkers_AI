#ifndef GUI_H
#define GUI_H

#include "board.h"
#include "ai.h"
#include <SDL.h>
#include <string>

class GUI {
public:
    GUI();
    ~GUI();

    bool init();
    void run(Board &board);
    void close();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    AI ai;
    
    bool gameRunning;
    bool playerTurn;
    bool gameOver;
    std::string gameStatus;
    
    // Stan wyboru pionka
    int selectedRow;
    int selectedCol;
    bool pieceSelected;
    std::vector<Move> validMoves;
    
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
    void drawUI();
    
    // Funkcje pomocnicze
    void getCellFromMouse(int mouseX, int mouseY, int &row, int &col);
    bool isValidCell(int row, int col);
    void selectPiece(Board &board, int row, int col);
    void makeMove(Board &board, int row, int col);
    void processAITurn(Board &board);
    void checkGameEnd(Board &board);
    void resetSelection();
    
    // Kolory
    void setColor(int r, int g, int b, int a = 255);
};

#endif // GUI_H