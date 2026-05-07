#include "app.h"
#include <iostream>
#include <limits>


std::vector <cell*> Pathfinder::getNeighbour(cell *curr) {
    int dR[4] = {1, 0, -1, 0};
    int dC[4] = {0, 1, 0, -1};
    std:: vector <cell*> neighbours;
    for (int i = 0; i < 4; i++) {
        int newRow = curr -> y + dR[i];
        int newCol = curr -> x + dC[i];
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
            neighbours.emplace_back(&grid[newRow][newCol]);
        }
    }
    return neighbours;
}

void Pathfinder::stepBFS() {
    if (q.empty()) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            std::cout << "No path found\n";
    }
    else {  
        cell* curr = q.front();
        q.pop();

        curr -> isVisited = true;
        visitedCount++;

        if (curr == target) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            pathFound = true;

            {
                pathCost = 0;
                cell* curr = target;
                while (curr && curr != source) {
                    switch (curr->terrain) {
                        case Terrain::GRASS: pathCost += 1; break;
                        case Terrain::MUD:   pathCost += 5; break;
                        case Terrain::WATER: pathCost += 10; break;
                    }
                    curr = curr->parent;
                }

                std::cout << "total BFS path cost: " << pathCost << "; cells visited: " << visitedCount << std::endl;
            }
        }
        else {
            for (auto &neigh : getNeighbour(curr)) {
                if (!neigh -> isVisited && !neigh -> isWall) {
                    neigh -> isVisited = true;
                    neigh -> parent = curr;
                    q.push(neigh);
                }
            }
        }
    }
}

void Pathfinder::stepDFS() {
    if (st.empty()) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            std::cout << "No path found\n";
    }
    else {
        cell *curr = st.top();

        if (curr == target) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            pathFound = true;
            {
                pathCost = 0;
                cell* curr = target;
                while (curr && curr != source) {
                    switch (curr->terrain) {
                        case Terrain::GRASS: pathCost += 1; break;
                        case Terrain::MUD:   pathCost += 5; break;
                        case Terrain::WATER: pathCost += 10; break;
                    }
                    curr = curr->parent;
                }

                std::cout << "total DFS path cost: " << pathCost << "; cells visited: " << visitedCount << std::endl;
            }
        }
        else {
            for (auto &neigh : getNeighbour(curr)) {
                if (!neigh -> isVisited && !neigh -> isWall) {
                    neigh -> isVisited = true;
                    visitedCount++;
                    neigh -> parent = curr;
                    st.push(neigh);
                    return;
                }
            }
            st.pop();
            return;
        }
    }
};

void Pathfinder::stepDijkstra() {
    if (pq.empty()) {
        isSearching = false;
        currentMode = Mode::SET_SOURCE;
        std::cout << "No path found\n";
        return;
    }
    else {
        auto [distance, curr] = pq.top();
        pq.pop();

        if (distance > curr -> distance) return;

        curr -> isVisited = true;
        visitedCount++;

        if (curr == target) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            pathFound = true;
            pathCost = curr -> distance;
            std::cout << "total DIJKSTRA path cost: " << pathCost << "; cells visited: " << visitedCount << std::endl;
            return;
        }
        else {
            for (auto &neigh: getNeighbour(curr)) {
                if (neigh -> isWall) continue;

                int edgeWeight = 0;
                switch (neigh -> terrain) {
                    case Terrain::GRASS: edgeWeight = 1; break;
                    case Terrain::MUD: edgeWeight = 5; break;
                    case Terrain::WATER: edgeWeight = 10; break;
                }
                int sumDistance = curr -> distance + edgeWeight;

                if (sumDistance < neigh -> distance) {
                    neigh -> distance = sumDistance;
                    neigh -> parent = curr;
                    pq.emplace(sumDistance, neigh);
                }        
            }
        }
    }
};
// void Pathfinder::stepAStar() {
//     if (pqA.empty()) {
//         isSearching = false;
//         currentMode = Mode::SET_SOURCE;
//         std::cout << "No path found\n";
//         return;
//     }
//     else {
//         auto [distance, curr] = pqA.top();
//         pqA.pop();

//         if (distance > curr -> finalDistance) return;

//         curr -> isVisited = true;
//         visitedCount++;

//         if (curr == target) {
//             isSearching = false;
//             currentMode = Mode::SET_SOURCE;
//             pathFound = true;
//             pathCost = curr -> distance;
//             std::cout << "total A* path cost: " << pathCost << "; cells visited: " << visitedCount << std::endl;
//             return;
//         }
//         else {
//             for (auto &neigh: getNeighbour(curr)) {
//                 if (neigh -> isWall) continue;

//                 int edgeWeight = 0;
//                 switch (neigh -> terrain) {
//                     case Terrain::GRASS: edgeWeight = 1; break;
//                     case Terrain::MUD: edgeWeight = 5; break;
//                     case Terrain::WATER: edgeWeight = 10; break;
//                 }
//                 // neigh_g(n) = curr_g(n) + cost_to_next
//                 int sumDistance = curr -> distance + edgeWeight;

//                 if (sumDistance < neigh -> distance) {
//                     neigh -> distance = sumDistance;
//                     neigh -> finalDistance = neigh -> distance + neigh -> heuristicDistance;
//                     neigh -> parent = curr;
//                     pqA.emplace(neigh -> finalDistance, neigh);
//                 }        
//             }
//         }
//     }
// };

void Pathfinder::stepAStar() {
    while (!pqA.empty()) {
        auto [distance, curr] = pqA.top();
        pqA.pop();

        // Skip stale nodes instantly without dropping the visualization frame!
        if (distance > curr->finalDistance) {
            continue; 
        }

        curr->isVisited = true;
        visitedCount++;

        if (curr == target) {
            isSearching = false;
            currentMode = Mode::SET_SOURCE;
            pathFound = true;
            pathCost = curr->distance;
            std::cout << "total A* path cost: " << pathCost << "; cells visited: " << visitedCount << std::endl;
            return;
        }

        for (auto &neigh: getNeighbour(curr)) {
            if (neigh->isWall) continue;

            int edgeWeight = 0;
            switch (neigh->terrain) {
                case Terrain::GRASS: edgeWeight = 1; break;
                case Terrain::MUD: edgeWeight = 5; break;
                case Terrain::WATER: edgeWeight = 10; break;
            }

            int sumDistance = curr->distance + edgeWeight;

            if (sumDistance < neigh->distance) {
                neigh->distance = sumDistance;
                neigh->finalDistance = neigh->distance + neigh->heuristicDistance;
                neigh->parent = curr;
                pqA.emplace(neigh->finalDistance, neigh);
            }        
        }
        
        // We successfully processed ONE valid node, so exit to render this frame
        return; 
    }

    // If the loop finishes and queue is empty:
    isSearching = false;
    currentMode = Mode::SET_SOURCE;
    std::cout << "No path found\n";
}

void Pathfinder::initBFS() {
    currentMode = Mode::SEARCH;
    std::cout << "Mode: SEARCH\n";
    visitedCount = 0;
    for (auto &row : grid) {
        for (auto &cell : row) {
            cell.isVisited = false;
            cell.parent = nullptr;
        }
    }
    
    while (!q.empty()) q.pop();
    
    q.push(source);
    source -> isVisited = true;
}

void Pathfinder::initDFS() {
    currentMode = Mode::SEARCH;
    std::cout << "Mode: SEARCH\n";
    visitedCount = 0;
    for (auto &row : grid) {
        for (auto &cell : row) {
            cell.isVisited = false;
            cell.parent = nullptr;
        }
    }
    
    while (!st.empty()) st.pop();
    
    st.push(source);
    source -> isVisited = true;
};

void Pathfinder::initDijkstra() {
    currentMode = Mode::SEARCH;
    std::cout << "Mode: SEARCH\n";
    visitedCount = 0;
    for (auto &row : grid) {
        for (auto &cell : row) {
            cell.isVisited = false;
            cell.parent = nullptr;
            cell.distance = std::numeric_limits<int>::max();
        }
    }
    
    while (!pq.empty()) pq.pop();
    
    source -> distance = 0;
    pq.emplace(0, source);
    // source -> isVisited = true;
};
void Pathfinder::initAStar() {
    currentMode = Mode::SEARCH;
    std::cout << "Mode: SEARCH\n";
    visitedCount = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            cell *cell = &grid[row][col];
            cell -> isVisited = false;
            cell -> parent = nullptr;
            cell -> distance = std::numeric_limits<int>::max();
            cell -> heuristicDistance = (abs(target -> x - col) + abs(target -> y - row)) * 1;
            cell -> finalDistance = cell -> distance;
            // abs(target -> x - 0) + abs(target -> y - 0)
        }
    }
    
    while (!pqA.empty()) pqA.pop();
    
    source -> distance = 0;
    source -> finalDistance = source -> distance + source -> heuristicDistance;
    pqA.emplace(source -> finalDistance, source);
};
