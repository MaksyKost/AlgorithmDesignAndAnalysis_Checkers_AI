#include "gui.h"
#include <iostream>

GUI::GUI() : window(nullptr), renderer(nullptr), quit(false) {}

GUI::~GUI() {
    clean();
}

bool GUI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Błąd SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("Warcaby AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Błąd SDL_CreateWindow: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Błąd SDL_CreateRenderer: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void GUI::render(Board &boardObj) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int tileSize = 80; // 640/8
    // Rysowanie planszy i pionków
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            SDL_Rect rect = { j * tileSize, i * tileSize, tileSize, tileSize };
            // Kolor pól (naprzemiennie jasne i ciemne)
            if ((i + j) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            else
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
            SDL_RenderFillRect(renderer, &rect);
            
            // Rysowanie pionków – dla uproszczenia symbolizujemy to prostokątem
            PieceType piece = boardObj.getPieceAt(i, j);
            if (piece != EMPTY) {
                if (piece == PLAYER || piece == PLAYER_KING)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                
                SDL_Rect pieceRect = { j * tileSize + tileSize / 4, i * tileSize + tileSize / 4, tileSize / 2, tileSize / 2 };
                SDL_RenderFillRect(renderer, &pieceRect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void GUI::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        // Tu możesz dodać obsługę kliknięć myszą lub klawiatury
    }
}

bool GUI::shouldQuit() {
    return quit;
}

void GUI::clean() {
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