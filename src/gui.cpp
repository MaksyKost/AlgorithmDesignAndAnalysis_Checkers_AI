#include "gui.h"
#include <iostream>
#include <sstream>
#include <SDL_ttf.h>
#include <string>

GUI::GUI() : window(nullptr), renderer(nullptr), font(nullptr), gameRunning(true), 
             playerTurn(true), gameOver(false), selectedRow(-1), 
             selectedCol(-1), pieceSelected(false) {
    gameStatus = "Twoja kolej - wybierz pionek";
}

GUI::~GUI() {
    close();
}

bool GUI::init() {
    std::cout << "Inicjalizacja SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Nie można uruchomić SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL OK" << std::endl;

    window = SDL_CreateWindow("Warcaby - AI vs Gracz",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Nie można utworzyć okna: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Okno OK" << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Nie można utworzyć renderera: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Renderer OK" << std::endl;

    if (TTF_Init() == -1) {
        std::cerr << "Nie można zainicjować SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL_ttf OK" << std::endl;

    font = TTF_OpenFont("font/ARIAL.TTF", 13);
    if (!font) {
        std::cerr << "Nie można załadować czcionki: " << TTF_GetError() << std::endl;
        std::cerr << "Gra będzie działać bez tekstu." << std::endl;
    }
    std::cout << "Czcionka OK" << std::endl;

    return true;
}

void GUI::run(Board &board) {
    while (gameRunning) {
        handleEvents(board);
        
        if (!gameOver && !playerTurn) {
            processAITurn(board);
        }

        checkPromotion(board.cells);
        
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
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                // Sprawdź czy kliknięto przycisk trudności
                handleDifficultyClick(e.button.x, e.button.y);

                // Obsługa planszy tylko jeśli tura gracza i gra nie jest zakończona
                if (playerTurn && !gameOver) {
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
                moveCount = 0;
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
    
    drawUI(board);
    
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
    setColor(0, 0, 255); // Niebieski dla zwykłych ruchów
    
    for (const auto& move : validMoves) {
        // Rysuj pole docelowe
        int centerX = BOARD_OFFSET_X + move.dstCol * CELL_SIZE + CELL_SIZE / 2;
        int centerY = BOARD_OFFSET_Y + move.dstRow * CELL_SIZE + CELL_SIZE / 2;
        int radius = CELL_SIZE / 6;
        
        // Jeśli to bicie, użyj czerwonego koloru
        if (!move.capturedPositions.empty()) {
            setColor(255, 0, 0); // Czerwony dla bić
            radius = CELL_SIZE / 4; // Większy znacznik
        }
        
        // Rysuj znacznik ruchu
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
        
        // Dla bić - oznacz zbite pionki
        if (!move.capturedPositions.empty()) {
            setColor(255, 100, 100); // Jasny czerwony
            for (const auto& capturedPos : move.capturedPositions) {
                int capX = BOARD_OFFSET_X + capturedPos.second * CELL_SIZE + CELL_SIZE / 2;
                int capY = BOARD_OFFSET_Y + capturedPos.first * CELL_SIZE + CELL_SIZE / 2;
                int capRadius = CELL_SIZE / 8;
                
                for (int y = -capRadius; y <= capRadius; y++) {
                    for (int x = -capRadius; x <= capRadius; x++) {
                        if (x*x + y*y <= capRadius*capRadius) {
                            SDL_RenderDrawPoint(renderer, capX + x, capY + y);
                        }
                    }
                }
            }
        }
        
        // Przywróć kolor dla następnego ruchu
        setColor(0, 0, 255);
    }
}

void GUI::drawUI(Board& board) {
    // Zwiększ rozmiar prostokąta statusu
    SDL_Rect statusRect = {BOARD_OFFSET_X + BOARD_SIZE + 20, BOARD_OFFSET_Y, 220, 180};
    setColor(255, 255, 255); // Biały tło
    SDL_RenderFillRect(renderer, &statusRect);

    setColor(0, 0, 0); // Czarna ramka
    SDL_RenderDrawRect(renderer, &statusRect);

    /* // Dodaj wizualny wskaźnik aktualnej tury
    SDL_Rect turnIndicator = {statusRect.x + 10, statusRect.y + 10, 20, 20};
    if (playerTurn) {
        setColor(255, 255, 255); // Biały dla gracza
    } else {
        setColor(139, 69, 19); // Brązowy dla AI
    }
    SDL_RenderFillRect(renderer, &turnIndicator);
    setColor(0, 0, 0);
    SDL_RenderDrawRect(renderer, &turnIndicator);*/

    drawDifficultyButtons();

    // Rysuj tekst tylko jeśli czcionka jest dostępna
    int lastTextY = statusRect.y + 15;
    int textHeight = 0;
    if (font) {
        SDL_Color black = {0, 0, 0, 255};

        // --- NOWOŚĆ: Łamanie długiego tekstu na linie ---
        std::istringstream iss(gameStatus);
        std::string word, line;
        int y = statusRect.y + 15;
        int maxWidth = statusRect.w - 20;
        while (iss >> word) {
            std::string testLine = line + (line.empty() ? "" : " ") + word;
            int w = 0, h = 0;
            TTF_SizeUTF8(font, testLine.c_str(), &w, &h);
            if (w > maxWidth && !line.empty()) {
                drawText(line, statusRect.x + 10, y, black);
                y += h + 2;
                line = word;
                textHeight = h;
            } else {
                line = testLine;
                textHeight = h;
            }
        }
        if (!line.empty()) {
            drawText(line, statusRect.x + 10, y, black);
            lastTextY = y;
        }
        lastTextY += textHeight + 5; // Pozycja pod ostatnią linią tekstu

        // Dodaj informacje o liczbie pionków
        std::string playerPieces = "Gracz: " + std::to_string(board.countPieces(false));
        std::string aiPieces = "AI: " + std::to_string(board.countPieces(true));

        drawText(playerPieces, statusRect.x + 10, statusRect.y + 120, black);
        drawText(aiPieces, statusRect.x + 10, statusRect.y + 140, black);

        if (gameOver) {
            drawText("Nacisnij R - restart", statusRect.x + 10, statusRect.y + 160, black);
        }
    }

    // Kolorowy prostokąt pod tekstem końca gry
    if (gameOver) {
        // Sprawdź czy tekst statusu zawiera "Przegrałeś" lub "Wygrałeś"
        bool przegrales = gameStatus.find("Przegrałeś") != std::string::npos;
        bool wygrales = gameStatus.find("Wygrałeś") != std::string::npos;
        if (przegrales || wygrales) {
            SDL_Rect endRect = {statusRect.x + 10, lastTextY, 180, 30};
            if (przegrales) setColor(255, 0, 0); // Czerwony
            if (wygrales) setColor(0, 200, 0);   // Zielony
            SDL_RenderFillRect(renderer, &endRect);
            setColor(0, 0, 0);
            SDL_RenderDrawRect(renderer, &endRect);

            // Wyśrodkuj tekst w prostokącie
            if (font) {
                SDL_Color black = {0, 0, 0, 255};
                std::string msg = przegrales ? "Przegrałeś!" : "Wygrałeś!";
                int w = 0, h = 0;
                TTF_SizeUTF8(font, msg.c_str(), &w, &h);
                int textX = endRect.x + (endRect.w - w) / 2;
                int textY = endRect.y + (endRect.h - h) / 2;
                drawText(msg, textX, textY, black);
            }
        }
    }

    // Wskaźnik tury w prawym dolnym rogu okna komunikatu
    int indicatorSize = 24;
    int margin = 10;
    SDL_Rect turnIndicator = {
        statusRect.x + statusRect.w - indicatorSize - margin,
        statusRect.y + statusRect.h - indicatorSize - margin,
        indicatorSize,
        indicatorSize
    };
    if (playerTurn) {
        setColor(255, 255, 255); // Biały dla gracza
    } else {
        setColor(139, 69, 19); // Brązowy dla AI
    }
    SDL_RenderFillRect(renderer, &turnIndicator);
    setColor(0, 0, 0);
    SDL_RenderDrawRect(renderer, &turnIndicator);

    if (font) {
        SDL_Color black = {0, 0, 0, 255};
        std::string movesText = std::to_string(moveCount);
        int w = 0, h = 0;
        TTF_SizeUTF8(font, movesText.c_str(), &w, &h);
        int textX = turnIndicator.x + (turnIndicator.w - w) / 2;
        int textY = turnIndicator.y + (turnIndicator.h - h) / 2;
        drawText(movesText, textX, textY, black);

        // Dodaj napis "Ruchy:" obok licznika
        drawText("Ruchy:", turnIndicator.x - 55, textY, black);
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
        moveCount++;
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

    Move aiMove = ai.getBestMove(board, currentDifficulty); // użyj wybranej trudności
    
    if (aiMove.srcRow != -1) { // Sprawdź czy AI znalazło ruch
        board.applyMove(aiMove);
        playerTurn = true;
        gameStatus = "Twoja kolej - wybierz pionek";
        moveCount++;
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
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

void GUI::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return; // Bezpiecznie wyjdź jeśli brak czcionki
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Błąd renderowania tekstu: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Błąd tworzenia tekstury: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Funkcja pomocnicza do uzyskania referencji do planszy (potrzebna dla UI)
Board& GUI::getBoardRef() {
    static Board dummyBoard; // To nie jest idealne rozwiązanie, ale potrzebne do kompilacji
    return dummyBoard;
}

void GUI::checkPromotion(std::shared_ptr<Piece> cells[Board::SIZE][Board::SIZE]) {
    // Promocja gracza (wiersz 0)
    for (int col = 0; col < 8; ++col) {
        auto& piece = cells[0][col];
        if (piece && !piece->getIsAI() && !piece->getIsKing()) {
            piece->promote();
            std::cout << "Gracz pionek awansował do damki na (0," << col << ")!" << std::endl;
        }
    }
    // Promocja AI (wiersz 7)
    for (int col = 0; col < 8; ++col) {
        auto& piece = cells[7][col];
        if (piece && piece->getIsAI() && !piece->getIsKing()) {
            piece->promote();
            std::cout << "AI pionek awansował do damki na (7," << col << ")!" << std::endl;
        }
    }
}

void GUI::handleDifficultyClick(int mouseX, int mouseY) {
    for (int i = 0; i < 3; i++) {
        SDL_Rect buttonRect = getButtonRect(i);
        if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
            
            switch(i) {
                case 0: currentDifficulty = Difficulty::EASY; break;
                case 1: currentDifficulty = Difficulty::MEDIUM; break;
                case 2: currentDifficulty = Difficulty::HARD; break;
            }
            break;
        }
    }
}

SDL_Rect GUI::getButtonRect(int buttonIndex) {
    int startX = BOARD_OFFSET_X + BOARD_SIZE + 20;
    int startY = BOARD_OFFSET_Y + 200 + buttonIndex * (BUTTON_HEIGHT + BUTTON_SPACING);
    
    return {startX, startY, BUTTON_WIDTH, BUTTON_HEIGHT};
}

void GUI::drawDifficultyButtons() {
    const char* labels[] = {"Łatwy", "Średni", "Trudny"};
    Difficulty difficulties[] = {Difficulty::EASY, Difficulty::MEDIUM, Difficulty::HARD};
    
    for (int i = 0; i < 3; i++) {
        SDL_Rect buttonRect = getButtonRect(i);
        
        // Kolor przycisku - podświetl aktywny
        if (currentDifficulty == difficulties[i]) {
            setColor(100, 200, 100); // Zielony dla aktywnego
        } else {
            setColor(200, 200, 200); // Szary dla nieaktywnego
        }
        
        SDL_RenderFillRect(renderer, &buttonRect);
        
        // Ramka
        setColor(0, 0, 0);
        SDL_RenderDrawRect(renderer, &buttonRect);
        
        // Tekst
        if (font) {
            SDL_Color black = {0, 0, 0, 255};
            drawText(labels[i], buttonRect.x + 5, buttonRect.y + 5, black);
        }
    }
}