#include "Graphics.h"
#include <iostream>
#include <stdlib.h>

Chip8Window::Chip8Window() {
    // anything to do with the display will be scaled for properly viewing.
    // logically shouldn't affect anything else.
    srand(time(nullptr));

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
            dest_rect = {0, 0, width * scale, height * scale};

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

// creates a simple chessboard pattern for window (debugging)
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
        switch (event.type) {
        case SDL_QUIT:
            flip_game_running();
            break;
        case SDL_KEYDOWN: {
            std::cout << "Key press detected" << std::endl;
            SDL_KeyboardEvent k = event.key;
            printf("Scancode: 0x%02X", k.keysym.scancode);
            printf(", (char)Scancode: %c", k.keysym.sym >= 33 && k.keysym.sym <= 126 ? (char)k.keysym.sym : '*');
            printf(", Name: %s\n", SDL_GetKeyName(k.keysym.sym));
            // Sorry for the implementation, just want to get this working properly
            switch(k.keysym.sym) {
                case '1':
                    break;
                case '2':
                    break;
                case '3':
                    break;
                case '4': 
                    break;
                case 'q': 
                    break;
                case 'w': 
                    break;
                case 'e': 
                    break;
                case 'r': 
                    break;
                case 'a': 
                    break;
                case 's': 
                    break;
                case 'd': 
                    break;
                case 'f': 
                    break;
                case 'z': 
                    break;
                case 'x': 
                    break;
                case 'c': 
                    break;
                case 'v': 
                    break;
                default:
                    printf("Key unrecognized: %c\n", k.keysym.sym);
                    break;
            }
            break;
        }
        case SDL_KEYUP:
            std::cout << "Key release detected" << std::endl;
            break;
        }
    }
}

void Chip8Window::update_screen_with_buffer(unsigned char *gfx) {
    // update pixels from gfx
    for (int i = 0; i < width * height; i++) {
        pixels[i] = gfx[i] == 1 ? color_on : color_off;
    }

    // update texture
    //* NOTE: instead of copying the entire new array from chip-8 gfx into
    //* pixels, could instead just update the texture with pointer to gfx
    // SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

    // clear previous renderer, copy new one from texture, present renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    SDL_RenderPresent(renderer);
}

void Chip8Window::update_screen() {
    // update pixels from gfx
    for (int i = 0; i < width * height; i++) {
        pixels[i] = i % 2 == 0 ? color_on : color_off;
    }

    // update texture
    //* NOTE: instead of copying the entire new array from chip-8 gfx into
    //* pixels, could instead just update the texture with pointer to gfx
    // SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

    // clear previous renderer, copy new one from texture, present renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    SDL_RenderPresent(renderer);
}

bool Chip8Window::is_running() { return running; }

void Chip8Window::flip_game_running() { running = !running; }