#include <iostream>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_ttf.h>
// #include <format>
// #include <memory>
// #include <vector>
// #include <queue>
// #include <stack>
// #include <algorithm>
#include <stdexcept>
#include <limits>
#include "app.h"
#include "sdl_setup.h"



int main() {

    int exit_val = EXIT_SUCCESS;

    try {
        initialize_sdl();
        Pathfinder p1;
        p1.init();
        p1.run();
    }
    catch (const std::runtime_error &e) {
        std:: cerr << e.what() << std:: endl;
        exit_val = EXIT_FAILURE;
    }

    close_sdl();

    return exit_val;
}