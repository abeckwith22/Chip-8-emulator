#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

class Chip8Window {
private:
    SDL_Window *window = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Rect dest_rect;
    SDL_Event event;
    Uint32 *pixels;
    Uint32 color_on   = 0xc18652;
    Uint32 color_off  = 0xd7c2b0;
    // Uint32 color_on   = 0xFFFFFF;
    // Uint32 color_off  = 0x000000;
    bool running = true;
    int width   = 64;
    int height  = 32;
    int scale   = 15;

    enum key_mappings {
        KEY_PRESS_1,
        KEY_PRESS_2,
        KEY_PRESS_3,
        KEY_PRESS_4,
        KEY_PRESS_Q,
        KEY_PRESS_W,
        KEY_PRESS_E,
        KEY_PRESS_R,
        KEY_PRESS_A,
        KEY_PRESS_S,
        KEY_PRESS_D,
        KEY_PRESS_F,
        KEY_PRESS_Z,
        KEY_PRESS_X,
        KEY_PRESS_C,
        KEY_PRESS_V,
    };

    // helper method that simply a key 
    void input_set_key(int key_num) {
    }

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

    void update_screen_with_buffer(unsigned char *gfx);

    void set_pixels();

    // gets is_running_state
    bool is_running();

    // Switches is_running bool to on or off
    void flip_game_running();

};

#endif