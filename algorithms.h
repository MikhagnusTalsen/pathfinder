#pragma once
#include "grid.h"

using Node = std::pair<int, cell *>;

struct cmpAstar {
    bool operator()(const Node& a, const Node& b) const {
        if (a.first == b.first)
            return a.second->heuristicDistance > b.second->heuristicDistance;
        return a.first > b.first;
    }
};