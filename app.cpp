#include <iostream>
#include <memory>
#include <format>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <cstddef>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "grid.h"
#include "algorithms.h"
#include "app.h"

Pathfinder::Pathfinder() 
: title{"Pathfinder"}, rect{0, 0, cellSize, cellSize}, titleRect{650, 7, 350, 80}, currentMode{Mode::SET_SOURCE},
window{nullptr, SDL_DestroyWindow}, renderer{nullptr, SDL_DestroyRenderer}, font{nullptr, TTF_CloseFont}, titleFont{nullptr, TTF_CloseFont},
titleSurface{nullptr, SDL_FreeSurface}, titleTexture{nullptr, SDL_DestroyTexture},
source{nullptr}, target{nullptr}, fontSize{20}, fontColour{0, 0, 0, 255} {};

void Pathfinder::init() {
    this->window.reset(SDL_CreateWindow(this->title.c_str(),SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, 0));
    if(!this->window) {
        auto error = std::format("error initializing the window: {}", SDL_GetError());
        throw std::runtime_error(error);
    }
    this->renderer.reset(SDL_CreateRenderer(this->window.get(),-1, SDL_RENDERER_ACCELERATED));
    if(!this->renderer) {
        auto error = std::format("error initializing the renderer: {}", SDL_GetError());
        throw std::runtime_error(error);
    }
    this->font.reset(TTF_OpenFont("BebasNeue-Regular.ttf", fontSize));
    if (!this->font) {
        auto error = std::format("error loading font: {}", TTF_GetError());
        throw std::runtime_error(error);
    }
    this->titleFont.reset(TTF_OpenFont("BebasNeue-Regular.ttf", 40));
    if (!this->titleFont) {
        auto error = std::format("error loading title font: {}", TTF_GetError());
        throw std::runtime_error(error);
    }
    this->grid.resize(rows);
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            this->grid[row].emplace_back(col, row);
        }
    }
    buttons.push_back({{10, 12, 60, 30}, ButtonType::SOURCE, "SRC"});
    buttons.push_back({{80, 12, 60, 30}, ButtonType::TARGET, "TRGT"});
    buttons.push_back({{150, 12, 60, 30}, ButtonType::WALL, "WALL"});
    buttons.push_back({{220, 12, 60, 30}, ButtonType::TERRAIN, "TRRN"});
    buttons.push_back({{290, 12, 60, 30}, ButtonType::RUN, "RUN"});
    buttons.push_back({{360, 12, 60, 30}, ButtonType::CLEAR, "CLR"});
    buttons.push_back({{430, 12, 60, 30}, ButtonType::RESET, "RST"});
    buttons.push_back({{500, 12, 60, 30}, ButtonType::SPEED_UP, "SPD +"});
    buttons.push_back({{570, 12, 60, 30}, ButtonType::SPEED_DOWN, "SPD -"});
    buttons.push_back({{10, 57, 60, 30}, ButtonType::BFS, "BFS"});
    buttons.push_back({{80, 57, 60, 30}, ButtonType::DFS, "DFS"});
    buttons.push_back({{150, 57, 60, 30}, ButtonType::DIJKSTRA, "DIJK"});
    buttons.push_back({{220, 57, 60, 30}, ButtonType::ASTAR, "ASTAR"});
    buttons.push_back({{290, 57, 60, 30}, ButtonType::GRASS, "GRASS"});
    buttons.push_back({{360, 57, 60, 30}, ButtonType::MUD, "MUD"});
    buttons.push_back({{430, 57, 60, 30}, ButtonType::WATER, "WATER"});

    for (auto &b: buttons) {

        auto surface = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> (
            TTF_RenderText_Blended(this->font.get(), b.label.c_str(), this->fontColour),
            SDL_FreeSurface
        );
        if (!surface) {
            auto error = std::format("error loading text surface: {}", TTF_GetError());
            throw std::runtime_error(error);
        }
        
        auto texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> (
            SDL_CreateTextureFromSurface(this->renderer.get(), surface.get()),
            SDL_DestroyTexture
        );
        if (!texture) {
            auto error = std::format("error loading text texture: {}", SDL_GetError());
            throw std::runtime_error(error);
        }
        b.labelTexture = std::move(texture);
        b.labelRect.w = surface -> w;
        b.labelRect.h = surface -> h;
        b.labelRect.x = b.rect.x + (b.rect.w - b.labelRect.w) / 2;
        b.labelRect.y = b.rect.y + (b.rect.h - b.labelRect.h) / 2;
    }
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);

    titleSurface.reset(TTF_RenderText_Blended_Wrapped(titleFont.get(), "              Path\n(v1) Finder", this -> fontColour, 200));
    if (!titleSurface) {
        auto error = std::format("error loading title surface: {}", TTF_GetError());
            throw std::runtime_error(error);
    }
    titleTexture.reset(SDL_CreateTextureFromSurface(renderer.get(), titleSurface.get()));
    if (!titleTexture) {
        auto error = std::format("error loading title texture: {}", TTF_GetError());
        throw std::runtime_error(error);    
    }

    titleRect.w = titleSurface->w;
    titleRect.h = titleSurface->h;

    // titleRect.x = width - titleRect.w - 10;
    // titleRect.y = 10;
}

void Pathfinder::run() {
    while (true) {
        while (SDL_PollEvent(&this->event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_1:
                        {
                            clearPath();
                            setMode(Mode::SET_SOURCE);
                            break;
                        }
                        case SDL_SCANCODE_2:
                        {
                            clearPath();
                            setMode(Mode::SET_TARGET);
                            break;
                        }
                        case SDL_SCANCODE_3:
                        {
                            clearPath();
                            setMode(Mode::DRAW_WALL);
                            break;
                        }
                        case SDL_SCANCODE_SPACE:
                        {
                            if (!source || !target) break;
                            startSearch();
                            break;
                        }
                        case SDL_SCANCODE_C:
                        {
                            clearPath();
                            break;
                        }
                        case SDL_SCANCODE_R:
                        {
                            resetGrid();
                            break;
                        }
                        case SDL_SCANCODE_MINUS:
                        {
                            SPEED = std::min(MAX_DELAY, SPEED + 30);
                            std::cout << "Speed: " << SPEED << std::endl;
                            break;
                        }
                        case SDL_SCANCODE_EQUALS:
                        {
                            SDL_Keymod mod = SDL_GetModState();
                            if (mod & KMOD_SHIFT) {
                                SPEED = std::max(MIN_DELAY, SPEED - 30);
                                std::cout << "Speed: " << SPEED << std::endl;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    {
                        int mousex = event.button.x;
                        int mousey = event.button.y;
                        if (mousey < ui_height) {
                            // int col = mousex / cellSize;
                            // int row = mousey / cellSize;

                            if (event.button.button == SDL_BUTTON_LEFT) {
                                for (auto &b: buttons) {
                                    if (isInside(mousex, mousey, b.rect)) {

                                        switch (b.type)  {
                                            case ButtonType::SOURCE:
                                            {
                                                clearPath();
                                                setMode(Mode::SET_SOURCE);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::TARGET:
                                            {
                                                clearPath();
                                                setMode(Mode::SET_TARGET);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::WALL:
                                            {
                                                clearPath();
                                                setMode(Mode::DRAW_WALL);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::TERRAIN:
                                            {
                                                clearPath();
                                                setMode(Mode::DRAW_TERRAIN);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::RUN:
                                            {
                                                if (!source || !target) break;
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                startSearch();
                                                break;
                                            }
                                            case ButtonType::CLEAR:
                                            {
                                                clearPath();
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::RESET:
                                            {
                                                resetGrid();
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::SPEED_DOWN:
                                            {
                                                SPEED = std::min(MAX_DELAY, SPEED + 30);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                std::cout << "Speed: " << SPEED << std::endl;
                                                break;
                                            }
                                            case ButtonType::SPEED_UP:
                                            {
                                                SPEED = std::max(MIN_DELAY, SPEED - 30);
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                std::cout << "Speed: " << SPEED << std::endl;
                                                break;
                                            }
                                            case ButtonType::BFS:
                                            {
                                                currentAlgorithm = Algorithm::BFS;
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }case ButtonType::DFS:
                                            {
                                                clearPath();
                                                currentAlgorithm = Algorithm::DFS;
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }case ButtonType::DIJKSTRA:
                                            {
                                                clearPath();
                                                currentAlgorithm = Algorithm::DIJKSTRA;
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }case ButtonType::ASTAR:
                                            {
                                                clearPath();
                                                currentAlgorithm = Algorithm::ASTAR;
                                                if (b.pressTimer == 0) b.pressTimer = 3;
                                                break;
                                            }
                                            case ButtonType::GRASS:
                                            {
                                                currentTerrain = Terrain::GRASS;
                                                break;
                                            }
                                            case ButtonType::MUD:
                                            {
                                                currentTerrain = Terrain::MUD;
                                                break;
                                            }
                                            case ButtonType::WATER:
                                            {
                                                currentTerrain = Terrain::WATER;
                                                break;
                                            }
                                            default:
                                                break;
                                        }      
                                        break;
                                    }
                                }
                            }
                        }
                        else 
                        {
                            int col = mousex / cellSize;
                            int row = (mousey - ui_height) / cellSize;

                            if (row < 0 || row >= rows || col < 0 || col >= cols) break;
                            
                            if (event.button.button == SDL_BUTTON_LEFT) {
                                switch (currentMode) {
                                    case Mode::SET_SOURCE:
                                    {
                                        if (grid[row][col].isWall) {
                                            break;
                                        }
                                        if (source) {
                                            grid[source -> y][source -> x].isSource = false;
                                        }
                                        source = &grid[row][col];
                                        grid[row][col].isSource = true;
                                        break;
                                    }
                                    case Mode::SET_TARGET:
                                    {
                                        if (grid[row][col].isWall) break;
                                        if (target) {
                                            grid[target -> y][target -> x].isTarget = false;
                                        }
                                        target = &grid[row][col];
                                        grid[row][col].isTarget = true;
                                        break;
                                    }
                                    case Mode::DRAW_WALL:
                                    {
                                        if (!grid[row][col].isSource && !grid[row][col].isTarget) {
                                            grid[row][col].isWall = !(grid[row][col].isWall);
                                        }
                                        break;
                                    }
                                    case Mode::DRAW_TERRAIN:
                                    {
                                        if (!grid[row][col].isSource && !grid[row][col].isTarget) {
                                            grid[row][col].terrain = currentTerrain;
                                        }
                                        break;
                                    }
                                    case Mode::SEARCH:
                                    {
                                        break;
                                    }
                                    default:
                                        break;
                                }
                                std::cout << "Clicked cell: (" << col << ", " << row << ")\n";
                            }
                        }
                        break;
                    }
                case SDL_MOUSEMOTION:
                {
                    int mousex = event.motion.x;
                    int mousey = event.motion.y;
                    Uint32 buttons = event.motion.state;
                    
                    if (mousey >= ui_height) {
                        int col = mousex / cellSize;
                        int row = (mousey - ui_height) / cellSize;

                        if (row < 0 || row >= rows || col < 0 || col >= cols) break;
                        if (!grid[row][col].isSource && !grid[row][col].isTarget) {
                                if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                                    switch (currentMode) {
                                        case Mode::DRAW_WALL:
                                        {
                                            grid[row][col].isWall = true;
                                            break;
                                        }
                                        case Mode::DRAW_TERRAIN:
                                        {
                                            grid[row][col].terrain = currentTerrain;
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                                else if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                                    switch (currentMode) {
                                        case Mode::DRAW_WALL:
                                        {
                                            grid[row][col].isWall = false;
                                            break;
                                        }
                                        case Mode::DRAW_TERRAIN:
                                        {
                                            grid[row][col].terrain = Terrain::GRASS;
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                            }
                    }
                    break;
                }
                default:
                    break;    
            }
        }
        if (isSearching) {
            switch (currentAlgorithm) {
                case Algorithm::BFS:
                    stepBFS();
                    break;
                case Algorithm::DFS:
                    stepDFS();
                    break;
                case Algorithm::DIJKSTRA:
                    stepDijkstra();
                    break;
                case Algorithm::ASTAR:
                    stepAStar();
                    break;
            }
        }

        // 1. clear grid draw title
        SDL_SetRenderDrawColor(this->renderer.get(), 100, 100, 100, 255);
        SDL_RenderClear(this->renderer.get());
        
        SDL_SetRenderDrawColor(this->renderer.get(), 100, 100, 100, 255);
        SDL_RenderFillRect(this->renderer.get(), &titleRect);
        SDL_RenderCopy(renderer.get(), titleTexture.get(), NULL, &titleRect);

        int hoverx, hovery;
        SDL_GetMouseState(&hoverx, &hovery);

        for (auto &b : buttons) {
            if (b.pressTimer > 0) b.pressTimer--;
        }

        // 2. draw buttons and text
        for (auto &b: buttons) {
            if (b.pressTimer > 0) {
                // pressed state
                SDL_SetRenderDrawColor(renderer.get(), 150, 150, 150, 255);
                
            }
            else if (
                (b.type == ButtonType::SOURCE   && currentMode == Mode::SET_SOURCE)         ||
                (b.type == ButtonType::TARGET   && currentMode == Mode::SET_TARGET)         ||
                (b.type == ButtonType::WALL     && currentMode == Mode::DRAW_WALL)          ||
                (b.type == ButtonType::TERRAIN  && currentMode == Mode::DRAW_TERRAIN)       ||
                (b.type == ButtonType::RUN      && currentMode == Mode::SEARCH)             ||
                (b.type == ButtonType::BFS      && currentAlgorithm == Algorithm::BFS)      ||
                (b.type == ButtonType::DFS      && currentAlgorithm == Algorithm::DFS)      ||
                (b.type == ButtonType::DIJKSTRA && currentAlgorithm == Algorithm::DIJKSTRA) ||
                (b.type == ButtonType::ASTAR    && currentAlgorithm == Algorithm::ASTAR)    ||
                (b.type == ButtonType::GRASS    && currentTerrain == Terrain::GRASS)        ||
                (b.type == ButtonType::MUD      && currentTerrain == Terrain::MUD)          ||
                (b.type == ButtonType::WATER    && currentTerrain == Terrain::WATER)    
            )
            {
                // active state
                SDL_SetRenderDrawColor(renderer.get(), 150, 150, 255, 255); 
            }
            else if (hovery < ui_height && isInside(hoverx, hovery, b.rect)) {
                // hovered state
                SDL_SetRenderDrawColor(renderer.get(), 225, 225, 225, 255);
            }
            else {
                // normal state
                SDL_SetRenderDrawColor(renderer.get(), 190, 190, 190, 255); 
            }

            // SDL_SetRenderDrawColor(this->renderer.get(), 220, 220, 220, 255);
            SDL_RenderFillRect(this->renderer.get(), &b.rect);

            SDL_RenderCopy(this->renderer.get(), b.labelTexture.get(), NULL, &b.labelRect);
        }
        
        // 3. draw wall and visited
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                rect.x = col * cellSize;
                rect.y = ui_height + row * cellSize;
                
                // if (!grid[row][col].isWall) {}
                if (grid[row][col].isWall) {
                    SDL_SetRenderDrawColor(this->renderer.get(), 0, 0, 0, 255);
                }
                else {
                    switch (grid[row][col].terrain) {
                        case Terrain::GRASS: SDL_SetRenderDrawColor(this->renderer.get(), 0, 130, 0, 255); break;
                        case Terrain::MUD: SDL_SetRenderDrawColor(this->renderer.get(), 66, 34, 19, 255); break;
                        case Terrain::WATER: SDL_SetRenderDrawColor(this->renderer.get(), 0, 200, 255, 255); break;
                    }
                    // SDL_SetRenderDrawColor(this->renderer.get(), 100, 100, 100, 255);
                }

                SDL_RenderFillRect(this->renderer.get(), &this->rect);

                if (grid[row][col].isVisited) {
                    SDL_SetRenderDrawColor(renderer.get(), 200, 200, 200, 110); // semi-transparent
                    SDL_RenderFillRect(renderer.get(), &rect);
                }
                
            }
            
        }

        // 4. draw path
        if (pathFound) {
            cell* curr = target;

            while (curr && curr != source) {
                // switch (curr -> terrain) {
                //     case Terrain::GRASS: pathCost += 1; break;
                //     case Terrain::MUD: pathCost += 5; break;
                //     case Terrain::WATER: pathCost += 10; break;
                // }
                rect.x = curr -> x * cellSize;
                rect.y = ui_height + curr -> y * cellSize;

                SDL_SetRenderDrawColor(renderer.get(), 200, 200, 0, 170);
                SDL_RenderFillRect(renderer.get(), &rect);
                // std::cout << curr -> x << "," << curr -> y << " " << std::endl;

                curr = curr->parent;
            }
            // pathFound = false;
        }

        // 5. draw source
        if (source) {
            rect.x = source -> x * cellSize;
            rect.y = ui_height + source -> y * cellSize;
            SDL_SetRenderDrawColor(this->renderer.get(), 40, 40, 240, 255);
            SDL_RenderFillRect(this->renderer.get(), &this->rect);
        }
        
        // 6. draw target
        if (target) {
            rect.x = target -> x * cellSize;
            rect.y = ui_height + target -> y * cellSize;
            SDL_SetRenderDrawColor(this->renderer.get(), 220, 50, 50, 255);
            SDL_RenderFillRect(this->renderer.get(), &this->rect);
        }

        // 7. draw grid
        SDL_SetRenderDrawColor(this->renderer.get(), 0, 0, 0, 255);
        makeGrid();

        SDL_RenderPresent(this->renderer.get());

        SDL_Delay(SPEED);

    }
}


void Pathfinder::makeGrid() {
    for (int i = 0 + ui_height; i <= this->height; i += this->cellSize) {
        // horizontal lines
        SDL_RenderDrawLine(this->renderer.get(), 0, i, width, i); // (0, i) -> (500, i)  
    }
    for (int i = 0; i <= this->width; i += this->cellSize) {
         // vertical lines
        SDL_RenderDrawLine(this->renderer.get(), i, 0 + ui_height, i, height); // (i, 0) -> (i, 500) 
    }  
}

void Pathfinder::setMode(Mode m) {
    currentMode = m;
    switch (m) {
        case Mode::SET_SOURCE:   std::cout << "Mode: SET_SOURCE" << std::endl; break;
        case Mode::SET_TARGET:   std::cout << "Mode: SET_TARGET" << std::endl; break;
        case Mode::DRAW_WALL:    std::cout << "Mode: DRAW_WALL" << std::endl; break;
        case Mode::DRAW_TERRAIN: std::cout << "Mode: DRAW_TERRAIN" << std::endl; break; 
        case Mode::SEARCH:       std::cout << "Mode: SEARCH" << std::endl; break;
    }
}

void Pathfinder::startSearch() {

    pathFound = false;
    clearPath();
    switch (currentAlgorithm) {
        case Algorithm::BFS:
            initBFS();
            break;
        case Algorithm::DFS:
            initDFS();
            break;
        case Algorithm::DIJKSTRA:
            initDijkstra();
            break;
        case Algorithm::ASTAR:
            initAStar();
            break;
    }

    isSearching = true;
    pathFound = false;
}

void Pathfinder::resetGrid() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            this->grid[row][col].isVisited = false;
            this->grid[row][col].isWall = false;
            this->grid[row][col].isSource = false;
            this->grid[row][col].isTarget = false;
            this->grid[row][col].parent = nullptr;
            this->grid[row][col].terrain = Terrain::GRASS;
        }
    }
    source = nullptr;
    target = nullptr;
}

void Pathfinder::clearPath() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            this->grid[row][col].isVisited = false;
            this->grid[row][col].parent = nullptr;
        }
    }
}

bool Pathfinder::isInside(int mousex, int mousey, SDL_Rect &rect) {
    return (mousex >= rect.x && mousex < rect.x + rect.w && mousey >= rect.y && mousey < rect.y + rect.h);
}