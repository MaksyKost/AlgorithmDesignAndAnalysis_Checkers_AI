#include "gui.h"
#include <iostream>
#include <sstream>

GUI::GUI() : window(nullptr), renderer(nullptr), gameRunning(true), 
             playerTurn(true), gameOver(false), selectedRow(-1), 
             selectedCol(-1), pieceSelected(false) {
    gameStatus = "Twoja kolej - wybierz pionek";
}

GUI::~GUI() {
    close();
}

bool GUI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Nie można uruchomić SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Warcaby - AI vs Gracz",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             WINDOW_WIDTH, WINDOW_HEIGHT,
                             SDL_WINDOW_SHOWN);
    
    if (!window) {
        std::cerr << "Nie można utworzyć okna: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Nie można utworzyć renderera: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void GUI::run(Board &board) {
    while (gameRunning) {
        handleEvents(board);
        
        if (!gameOver && !playerTurn) {
            processAITurn(board);
        }
        
        render(board);
        checkGameEnd(board);
        SDL_Delay(16); // ~60 FPS
    }
}

void GUI::handleEvents(Board &board) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gameRunning = false;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN && playerTurn && !gameOver) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int row, col;
                getCellFromMouse(e.button.x, e.button.y, row, col);
                
                if (isValidCell(row, col)) {
                    if (!pieceSelected) {
                        selectPiece(board, row, col);
                    } else {
                        makeMove(board, row, col);
                    }
                }
            }
            else if (e.button.button == SDL_BUTTON_RIGHT) {
                resetSelection();
            }
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_r && gameOver) {
                // Restart gry
                board.init();
                gameOver = false;
                playerTurn = true;
                resetSelection();
                gameStatus = "Nowa gra - Twoja kolej";
            }
        }
    }
}

void GUI::render(Board &board) {
    // Wyczyść ekran
    setColor(240, 217, 181); // Jasny beż
    SDL_RenderClear(renderer);
    
    drawBoard();
    drawPieces(board);
    
    if (pieceSelected) {
        drawSelectedCell();
        drawValidMoves();
    }
    
    drawUI();
    
    SDL_RenderPresent(renderer);
}

void GUI::drawBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            SDL_Rect cellRect = {
                BOARD_OFFSET_X + col * CELL_SIZE,
                BOARD_OFFSET_Y + row * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            };
            
            if ((row + col) % 2 == 0) {
                setColor(240, 217, 181); // Jasne pola
            } else {
                setColor(181, 136, 99);  // Ciemne pola
            }
            
            SDL_RenderFillRect(renderer, &cellRect);
            
            // Ramka
            setColor(0, 0, 0);
            SDL_RenderDrawRect(renderer, &cellRect);
        }
    }
}

void GUI::drawPieces(Board &board) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            auto piece = board.getPiece(row, col);
            if (piece) {
                int centerX = BOARD_OFFSET_X + col * CELL_SIZE + CELL_SIZE / 2;
                int centerY = BOARD_OFFSET_Y + row * CELL_SIZE + CELL_SIZE / 2;
                int radius = CELL_SIZE / 3;
                
                // Rysuj pionek (okrąg)
                if (piece->getIsAI()) {
                    setColor(139, 69, 19); // Brązowy dla AI
                } else {
                    setColor(255, 255, 255); // Biały dla gracza
                }
                
                // Prosty sposób rysowania okręgu
                for (int y = -radius; y <= radius; y++) {
                    for (int x = -radius; x <= radius; x++) {
                        if (x*x + y*y <= radius*radius) {
                            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                        }
                    }
                }
                
                // Ramka pionka
                setColor(0, 0, 0);
                for (int y = -radius; y <= radius; y++) {
                    for (int x = -radius; x <= radius; x++) {
                        int distSq = x*x + y*y;
                        if (distSq <= radius*radius && distSq >= (radius-2)*(radius-2)) {
                            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                        }
                    }
                }
                
                // Oznaczenie damki
                if (piece->getIsKing()) {
                    setColor(255, 215, 0); // Złoty dla damki
                    for (int y = -radius/2; y <= radius/2; y++) {
                        for (int x = -radius/2; x <= radius/2; x++) {
                            if (x*x + y*y <= (radius/2)*(radius/2)) {
                                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                            }
                        }
                    }
                }
            }
        }
    }
}

void GUI::drawSelectedCell() {
    if (selectedRow >= 0 && selectedCol >= 0) {
        SDL_Rect cellRect = {
            BOARD_OFFSET_X + selectedCol * CELL_SIZE,
            BOARD_OFFSET_Y + selectedRow * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE
        };
        
        setColor(0, 255, 0); // Zielony dla wybranego pola
        for (int i = 0; i < 3; i++) {
            SDL_Rect borderRect = {
                cellRect.x - i, cellRect.y - i,
                cellRect.w + 2*i, cellRect.h + 2*i
            };
            SDL_RenderDrawRect(renderer, &borderRect);
        }
    }
}

void GUI::drawValidMoves() {
    setColor(0, 0, 255, 128); // Półprzezroczysty niebieski
    
    for (const auto& move : validMoves) {
        int centerX = BOARD_OFFSET_X + move.dstCol * CELL_SIZE + CELL_SIZE / 2;
        int centerY = BOARD_OFFSET_Y + move.dstRow * CELL_SIZE + CELL_SIZE / 2;
        int radius = CELL_SIZE / 6;
        
        // Rysuj małe kółko jako wskazówkę ruchu
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
    }
}

void GUI::drawUI() {
    // Rysuj status gry po prawej stronie planszy
    SDL_Rect statusRect = {BOARD_OFFSET_X + BOARD_SIZE + 20, BOARD_OFFSET_Y, 200, 100};
    setColor(255, 255, 255); // Biały tło
    SDL_RenderFillRect(renderer, &statusRect);
    
    setColor(0, 0, 0); // Czarna ramka
    SDL_RenderDrawRect(renderer, &statusRect);
    
    // Dodaj wizualny wskaźnik aktualnej tury
    SDL_Rect turnIndicator = {statusRect.x + 10, statusRect.y + 10, 20, 20};
    if (playerTurn) {
        setColor(255, 255, 255); // Biały dla gracza
    } else {
        setColor(139, 69, 19); // Brązowy dla AI
    }
    SDL_RenderFillRect(renderer, &turnIndicator);
    setColor(0, 0, 0);
    SDL_RenderDrawRect(renderer, &turnIndicator);
    
    if (gameOver) {
        // Rysuj prostokąt "Game Over"
        SDL_Rect gameOverRect = {statusRect.x + 10, statusRect.y + 50, 180, 30};
        setColor(255, 0, 0); // Czerwony
        SDL_RenderFillRect(renderer, &gameOverRect);
        setColor(0, 0, 0);
        SDL_RenderDrawRect(renderer, &gameOverRect);
    }
}

void GUI::getCellFromMouse(int mouseX, int mouseY, int &row, int &col) {
    if (mouseX >= BOARD_OFFSET_X && mouseX < BOARD_OFFSET_X + BOARD_SIZE &&
        mouseY >= BOARD_OFFSET_Y && mouseY < BOARD_OFFSET_Y + BOARD_SIZE) {
        col = (mouseX - BOARD_OFFSET_X) / CELL_SIZE;
        row = (mouseY - BOARD_OFFSET_Y) / CELL_SIZE;
    } else {
        row = -1;
        col = -1;
    }
}

bool GUI::isValidCell(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

void GUI::selectPiece(Board &board, int row, int col) {
    auto piece = board.getPiece(row, col);
    if (piece && !piece->getIsAI()) { // Tylko pionki gracza
        selectedRow = row;
        selectedCol = col;
        pieceSelected = true;
        
        // Znajdź dostępne ruchy dla wybranego pionka
        validMoves.clear();
        auto allMoves = board.getValidMoves(false); // false = gracz
        
        for (const auto& move : allMoves) {
            if (move.srcRow == row && move.srcCol == col) {
                validMoves.push_back(move);
            }
        }
        
        if (validMoves.empty()) {
            gameStatus = "Ten pionek nie może się ruszyć!";
            resetSelection();
        } else {
            gameStatus = "Wybierz pole docelowe";
        }
    } else {
        gameStatus = "Wybierz swój pionek!";
    }
}

void GUI::makeMove(Board &board, int row, int col) {
    // Sprawdź czy kliknięte pole to dostępny ruch
    Move selectedMove;
    bool validMove = false;
    
    for (const auto& move : validMoves) {
        if (move.dstRow == row && move.dstCol == col) {
            selectedMove = move;
            validMove = true;
            break;
        }
    }
    
    if (validMove) {
        board.applyMove(selectedMove);
        playerTurn = false;
        gameStatus = "Kolej AI...";
        resetSelection();
    } else {
        // Sprawdź czy kliknął na inny swój pionek
        auto piece = board.getPiece(row, col);
        if (piece && !piece->getIsAI()) {
            // Wybierz nowy pionek
            selectPiece(board, row, col);
        } else {
            gameStatus = "Nieprawidłowy ruch!";
        }
    }
}

void GUI::processAITurn(Board &board) {
    if (!board.hasValidMoves(true)) {
        gameOver = true;
        gameStatus = "Wygrałeś! Naciśnij R dla nowej gry";
        return;
    }
    
    Move aiMove = ai.getBestMove(board, 5);
    if (aiMove.srcRow != -1) { // Sprawdź czy AI znalazło ruch
        board.applyMove(aiMove);
        playerTurn = true;
        gameStatus = "Twoja kolej - wybierz pionek";
    } else {
        gameOver = true;
        gameStatus = "Wygrałeś! AI nie ma ruchów. Naciśnij R dla nowej gry";
    }
}

void GUI::checkGameEnd(Board &board) {
    if (gameOver) return;
    
    // Sprawdź czy gracz ma dostępne ruchy
    if (!board.hasValidMoves(false)) {
        gameOver = true;
        gameStatus = "Przegrałeś! Naciśnij R dla nowej gry";
        return;
    }
    
    // Sprawdź czy AI ma dostępne ruchy
    if (!board.hasValidMoves(true)) {
        gameOver = true;
        gameStatus = "Wygrałeś! Naciśnij R dla nowej gry";
        return;
    }
    
    // Sprawdź czy któraś strona nie ma pionków
    if (board.countPieces(false) == 0) {
        gameOver = true;
        gameStatus = "Przegrałeś! Naciśnij R dla nowej gry";
    } else if (board.countPieces(true) == 0) {
        gameOver = true;
        gameStatus = "Wygrałeś! Naciśnij R dla nowej gry";
    }
}

void GUI::resetSelection() {
    selectedRow = -1;
    selectedCol = -1;
    pieceSelected = false;
    validMoves.clear();
    if (!gameOver && playerTurn) {
        gameStatus = "Twoja kolej - wybierz pionek";
    }
}

void GUI::setColor(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void GUI::close() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}