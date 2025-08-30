#include "Chip8.h"
#include "Graphics.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include <thread>

Chip8 chip8;

void print_gfx(const unsigned char *array);

void run_sdl2_window() {
    using namespace std::this_thread;
    using namespace std::chrono;
    int frame = 0;

    Chip8Window *screen = new Chip8Window();
    chip8.initialize();
    chip8.load_game("picture.ch8");
    while (screen->is_running()) {
        screen->handle_input();
        chip8.emulate_cycle();
        printf("======================\n", frame);
        printf("Frame     -> %d\n", frame);
        if (chip8.get_draw_flag() == true) {
            printf("      ==> Proceeding to draw screen \n");
            screen->update_screen(chip8.get_gfx());
            chip8.set_draw_flag(false); // draw_flag set back to false
            printf("======================\n", frame);
        }
        frame++;
    }
    delete screen;
}

void print_gfx(const unsigned char *array) {
    for (int i = 0; i < 64 * 32; i++) {
        if (i % 64 == 0 && i != 0) {
            printf("\n");
        }
        printf("%X", array[i]);
    }
}

int main(int argc, char **argv) {
    // run_chip8_emulator();
    run_sdl2_window();
    return 0;
}
