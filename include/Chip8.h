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

    // Graphics buffer. Chip-8 supports a height of 64 pixels and a width of 32
    // pixels.
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

    // Current number of entries in the stack.
    unsigned int stack_current_size = 0;

    // Stack pointer, increments up and down based on calls and returns from
    // subroutines.
    unsigned short sp;

    // The chip-8 has a HEX based keypad (0x0-0xF), you can use an array
    // to store the current state of the key.

    //* ====================== Key Mapping =================================
    //* |  0: 0x00 |  1: 0x01 |  2: 0x02 |  3: 0x03 | <=> | 1 | 2 | 3 | 4 |
    //* |  4: 0x04 |  5: 0x05 |  6: 0x06 |  7: 0x07 | <=> | Q | W | E | R |
    //* |  8: 0x08 |  9: 0x09 | 10: 0x0a | 11: 0x0b | <=> | A | S | D | F |
    //* | 12: 0x0c | 13: 0x0d | 14: 0x0e | 15: 0x0f | <=> | Z | X | C | V |
    //* ====================================================================

    //* Every frame that Chip8Window checks for input set value to 1 if
    //* associated key is pressed and switch back to 0 if unpressed
    //* Note: At initialization, all values in key should be set to 0.

    //? to avoid confusion, shouldn't key be a list of booleans instead of
    //? unsigned characters?
    //? The values are only going to be either on (pressed) or off (unpressed).
    bool key[16];
    // unsigned char key[16];

    // When gfx is updated, draw_flag is set to true and
    // updates the screen.
    bool draw_flag = false;

    int file_size = 0;

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
    // Gets emulator read to load game.
    void initialize();
    // Read game from filesystem and load into memory array.
    void load_game(const char *exec_path);
    void emulate_cycle();
    // Bitmasks values in gfx to &= 0x00, sets `draw_flag` to true.
    void gfx_clear();
    void gfx_draw_all();
    // Returns gfx array to help update SDL window.
    unsigned char *get_gfx();
    // Draw flags getters/setters
    bool get_draw_flag();
    void set_draw_flag(bool boolean);

    //* Pass in array of keys from Chip8Window class and loop through the 16 keys
    //* and switch their states depending if that key is pressed.
    //* NOTE: Wondering if I need to work out key-mapping or if the opcode key
    //* buffer will be enough by itself.


    void set_key(int address) {
        if (address > 0 && address < 16)
            key[address] = 1;
    }

    // Debugging
    void read_binary_opcodes();
    void read_stack();
};

#endif
