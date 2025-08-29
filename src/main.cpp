#include "Chip8.h"
#include "Graphics.h"
#include <GL/glut.h>
#include <iostream>

Chip8 chip8;

#define MAXSIZE 4096

// Opens ch8 file in read-only binary mode and writes to char buffer. Returns
// executable size.
int write_into_buffer(const char exec_path[], unsigned char *buffer);

// Displays memory opcodes to terminal
void read_buffer(unsigned char *buffer, int buffer_size);

void run_sdl2_window() {
    Chip8Window *screen = new Chip8Window();
    while (screen->is_running()) {
        screen->handle_input();
        screen->update_screen();
    }
    delete screen;
}

void run_chip8_emulator() {
    unsigned char buffer[MAXSIZE];
    int size = write_into_buffer("picture.ch8", buffer);
    chip8.initialize(buffer, size);
    chip8.read_binary_opcodes(size);
    for(int i = 0; i < size; i++) {
        chip8.emulate_cycle();
    }
}

int main(int argc, char **argv) {
    run_sdl2_window();
    return 0;
}

int write_into_buffer(const char exec_path[], unsigned char *buffer) {
    FILE *fptr;
    if (!(fptr = fopen(exec_path, "rb"))) {
        std::cerr << "ERROR: Failed to open file. Maybe check your executable "
                     "path...?";
        return -1;
    }
    int c, i;
    for (i = 0; (c = std::fgetc(fptr)) != EOF; i++)
        buffer[i] = c;

    fclose(fptr);
    return i;
}

void read_buffer(unsigned char *buffer, int buffer_size) {
    for (int i = 0; i < buffer_size; i++) {
        if (i % 10 == 0 && i != 0) // formatting
            printf("\n");
        printf("0x%2X ", buffer[i]);
    }
    printf("\n");
}
