#pragma once

enum class Terrain {
    GRASS,
    MUD,
    WATER
};

struct cell {
    int x;
    int y;
    
    bool isVisited = false;
    bool isWall = false;
    bool isSource = false;
    bool isTarget = false;

    cell* parent = nullptr;

    int distance;
    Terrain terrain;
    int heuristicDistance;
    int finalDistance;


    cell(int x_, int y_) : x(x_), y(y_), terrain(Terrain::GRASS) {}
};