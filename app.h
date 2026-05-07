#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <format>
#include <memory>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <limits>
#include "grid.h"
#include "algorithms.h"


class Pathfinder {

private:
    enum class Algorithm {
        BFS,
        DFS,
        DIJKSTRA,
        ASTAR
    };
    enum class Mode {
        SET_SOURCE,
        SET_TARGET,
        DRAW_WALL,
        DRAW_TERRAIN,
        SEARCH
    };
    enum class ButtonType {
        SOURCE,
        TARGET,
        WALL,
        TERRAIN,
        RUN,
        CLEAR,
        RESET,
        SPEED_UP,
        SPEED_DOWN,
        BFS,
        DFS,
        DIJKSTRA,
        ASTAR,
        GRASS,
        MUD,
        WATER
    };
    struct Button {
        SDL_Rect rect;
        ButtonType type;
        std::string label;

        int pressTimer = 0;

        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> labelTexture;
        SDL_Rect labelRect;

        Button (SDL_Rect r, ButtonType t, std::string s) 
        :   rect{r},
            type {t},
            label{s},
            labelTexture{nullptr, &SDL_DestroyTexture},
            labelRect{0, 0, 0, 0,}
        {}
    };

    int SPEED = 40;
    const int MAX_DELAY = 200;
    const int MIN_DELAY = 5;
    const std:: string title;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect titleRect;
    Mode currentMode;
    Terrain currentTerrain;
    Algorithm currentAlgorithm;
    int pathCost = 0;
    int visitedCount = 0;

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> font;
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> titleFont;
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> titleSurface;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> titleTexture;

    std::vector <std::vector <cell>> grid;
    int rows = (height - ui_height) / cellSize;
    int cols = width / cellSize;
    cell *source;
    cell *target;
    std::queue<cell*> q;
    std::stack<cell*> st;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    std::priority_queue<Node, std::vector<Node>, cmpAstar> pqA;
    bool isSearching = false;
    bool pathFound = false;
    std::vector <Button> buttons;
    int fontSize;
    SDL_Color fontColour;

public:
    Pathfinder();
    void init();
    void run();
    void makeGrid();
    void setMode(Mode);
    void startSearch();
    void clearPath();
    void resetGrid();
    bool isInside(int, int ,SDL_Rect &);
    void stepBFS();
    void stepDFS();
    void stepDijkstra();
    void stepAStar();
    void initBFS();
    void initDFS();
    void initDijkstra();
    void initAStar();

    std::vector <cell*> getNeighbour(cell *);
    static constexpr int cellSize{20};
    static constexpr int width{800};
    static constexpr int height{900};
    static constexpr int ui_height{100};
};
