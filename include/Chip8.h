#ifndef CHIP8_H
#define CHIP8_H

class Chip8 {
private:
    // each opcode is 2 bytes which is represented with an unsigned short.
    unsigned short opcode;

    // Chip-8 has 4K of memory in total.
    unsigned char memory[4096];

    // CPU registers, from V0-VE with the 16th register being the 'carry flag'.
    // Eight bits is one byte so we can use an unsigned char.
    unsigned char V[16];

    // Index register and a program counter which can have a value from
    // 0x000-0xFFF.
    unsigned short index_register;
    // prog_counter (pc) moves to next opcodes in memory.
    unsigned short prog_counter;

    // Graphics buffer. Chip-8 supports a height of 64 pixels and a width of 32 pixels.
    unsigned char gfx[64 * 32];

    unsigned char sound_timer;
    unsigned char delay_timer;

    // The Chip-8 instruction set has opcodes that allow the program to jump
    // to a certain address. While the chip-8 specification doesn't mention a
    // stack you'll have to implement one to allow for jumps to other addresses
    // in your program
    unsigned short stack[16];
    // moves through stack and increments either up or down when opcode
    // calls a subroutine.
    unsigned short sp;

    // Lastly, the chip-8 has a HEX based keypad (0x0-0xF), you can use an array
    // to store the current state of the key.
    unsigned char key[16];

    // when gfx is updated, draw_flag is set to true and
    // updates the screen.
    bool draw_flag = false;

    const unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

public:
    // Loads game into memory, points prog_counter to start of code
    void initialize(unsigned char b[], int b_size);
    void emulate_cycle();

    // Returns gfx array to help update SDL window.
    unsigned char *get_gfx();

	bool get_draw_flag();

    // Helpful for debugging
    void read_binary_opcodes(int b_size);
};

#endif
