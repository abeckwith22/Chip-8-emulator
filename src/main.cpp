#include "Chip8.h"
#include "Graphics.h"
#include <chrono>
#include <iostream>
#include <thread>

Chip8 chip8;

void print_gfx(const unsigned char *array);

void run_emulator() {
    using namespace std::this_thread;
    using namespace std::chrono;
    int frame = 0;

    Chip8Window *screen = new Chip8Window();
    chip8.initialize();
    chip8.load_game("Cave.ch8");
    while (screen->is_running()) {
        sleep_for(milliseconds(100));
        screen->handle_input();
        chip8.emulate_cycle();
        printf("======================\n", frame);
        printf("Frame     -> %d\n", frame);
        if (chip8.get_draw_flag() == true) {
            printf("      ==> Proceeding to draw screen \n");
            screen->update_screen_with_buffer(chip8.get_gfx());
            chip8.set_draw_flag(false);
            printf("======================\n", frame);
        }
        frame++;
    }
    delete screen;
}

void run_sdl2_window() {
    using namespace std::this_thread;
    using namespace std::chrono;
    int frame = 0;

    Chip8Window *screen = new Chip8Window();
    while (screen->is_running()) {
        sleep_for(milliseconds(10));
        screen->handle_input();
        screen->update_screen();
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
    // run_emulator();
    run_sdl2_window();

    return 0;
}
