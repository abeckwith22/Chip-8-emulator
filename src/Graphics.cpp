#include "Graphics.h"
#include <iostream>
#include <stdlib.h>

Chip8Window::Chip8Window() {
    // anything to do with the display will be scaled for properly viewing.
    // logically shouldn't affect anything else.

    pixels = new Uint32[width * height];
    memset(pixels, 0, width * height);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "ERROR: SDL could not initialize! SDL_Error: "
                  << SDL_GetError() << "\n";
    } else {
        window = SDL_CreateWindow("Chip-8 Emulator",       // Name
                                  SDL_WINDOWPOS_UNDEFINED, // Screen X position
                                  SDL_WINDOWPOS_UNDEFINED, // Screen Y position
                                  width * scale,           // Width
                                  height * scale,          // Height
                                  0                        // Flags
        );
        if (window == nullptr) {
            std::cout << "ERROR: Window could not be created! SDL_Error: "
                      << SDL_GetError() << "\n";
        } else {
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture =
                SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STATIC, width, height);

            set_pixels(); //! DELETE THIS LATER. Just testing if pixel data is
                          //! outputted properly.

            SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));
            SDL_Rect dest_rect = {0, 0, width * scale, height * scale};

            // == update texture ==
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
            SDL_RenderPresent(renderer);
            // SDL_UpdateWindowSurface(window);
        }
    }
}

Chip8Window::~Chip8Window() {
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Chip8Window::set_pixels() {
    bool color = false;
    for (int i = 0; i < width * height; i++) {
        if (!(i % width == 0))
            color = !color;
        pixels[i] = color ? color_on : color_off;
    }
}

void Chip8Window::handle_input() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            flip_game_running();
        }
    }
}

void Chip8Window::update_screen() {
    // Get window surface
}

bool Chip8Window::is_running() { return running; }

void Chip8Window::flip_game_running() { running = !running; }