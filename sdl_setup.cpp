#include "sdl_setup.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <format>
#include <stdexcept>

void initialize_sdl() {
    int sdl_flags = SDL_INIT_EVERYTHING;

    if (SDL_Init(sdl_flags)) {
        auto error = std::format("error initializing sdl: {}", SDL_GetError());
        throw std::runtime_error(error);
    }
    if (TTF_Init()) {
        auto error = std::format("error initializing ttf: {}", TTF_GetError());
        throw std::runtime_error(error);
    }
}

void close_sdl() {
    SDL_Quit();
    TTF_Quit();
}