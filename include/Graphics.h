#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

class Chip8Window {
private:
    SDL_Window *window = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Event event;
    Uint32 *pixels;
    Uint32 color_on   = 0xc18652;
    Uint32 color_off  = 0xd7c2b0;
    bool running = true;
    int width   = 64;
    int height  = 32;
    int scale   = 15;

    enum key_press_surfaces {
        KEY_PRESS_SURFACE_DEFAULT,
        KEY_PRESS_SURFACE_UP,
        KEY_PRESS_SURFACE_DOWN,
        KEY_PRESS_SURFACE_LEFT,
        KEY_PRESS_SURFACE_RIGHT,
        KEY_PRESS_SURFACE_TOTAL
    };

public:
    /**
     * @brief Construct a new Chip 8 Window:: Chip 8 Window object.
     * 
     * - Intializes screen, renderer, and texture. 
     *
     * - Scales SDL_window properly to avoid having to view a (literal) 64x32 pixel display.
     *
     * - Pixels are mutable through the `update_gfx()` method.
     */
    Chip8Window();

    /**
     * @brief Destroy the Chip 8 Window object
     *
     */
    ~Chip8Window();

    /**
     * @brief Handles standard window events (keyboard events, quitting, etc)
     *
     */
    void handle_input();

    void update_screen();

    void set_pixels();

    // gets is_running_state
    bool is_running();

    // Switches is_running bool to on or off
    void flip_game_running();
};

#endif