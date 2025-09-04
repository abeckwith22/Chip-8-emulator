// =====================================================================================
// Chip-8 Emulator
// Program Author: abeckwith22
// Credits go to: Laurence Muller and their article at
// https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
// to which I have shamelessly stolen code from for educative purposes.
// =====================================================================================
#include "Chip8.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initialize
// Before running the first emulation cycle, you'll you need to prepare the
// systems state. Clearing memory resetting registers to zero.
void Chip8::initialize() {
    prog_counter = 0x200; // Program counter starts at 0x200
    opcode = 0;           // Reset current opcode
    index_register = 0;   // Reset index register
    sp = 0;               // Reset stack pointer
    file_size = 0;        // File size resets to load next game
    // Clear display
    gfx_clear(); // clear graphics before loading next game

    // Clear stack
    for (int i = 0; i < 0xF; i++)
        stack[i] = 0;
    // Clear registers
    for (int i = 0; i < 0xF; i++)
        V[i] = 0;
    // Clear memory
    for (int i = 0; i < 0xFFF; i++)
        memory[i] = 0;
    // reset keys (keys are set to 'unpressed')
    for (int i = 0; i < 0xF; i++)
        key[i] = 0;

    // Set seed
    std::srand(time(NULL));

    // Load fontset
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
    // Reset timers
    delay_timer = 60;
    sound_timer = 60;

    // load program into memory with fopen in binary mode, start filling memory
}

// Loads game into memory, modifies file_size to games size for debugging
// purposes (i.e. reading opcodes)
void Chip8::load_game(const char *executable_path) {
    FILE *fptr;
    if (!(fptr = fopen(executable_path, "rb"))) {
        std::cerr << "ERROR: Failed to open file. Maybe check your executable "
                     "path...?";
        return;
    }
    // read character into memory; limiting steps to 4096 as safeguard
    int c;
    int i;
    for (i = 0; (c = fgetc(fptr)) != EOF && i < 4096; i++) {
        memory[i + 0x200] = c;
    }
    fclose(fptr);

    // Set file size to i
    file_size = i;
}

// TODO: Emulate a timer
// Chip-8 has two timers you will need to implement.
// As mentioned above bother timers (delay and sound) will count down to zero
// if they've been set to any value > 0. Since these timers count down at 60 Hz
// it'd be smart to implment something that slows down your emulation cycle
// (e.g. such as executing only 60 opcodes/second).
void Chip8::emulate_cycle() {
    // Fetch Opcode. Something to note is that opcodes are 16 bits (2 bytes) so
    // we need to fetch the current one at prog_counter and bitshift them 8 bits
    // over and perform an OR on the next 8 bits in memory.
    opcode = memory[prog_counter] << 8 | memory[prog_counter + 1];
    printf("Executing opcode [0x0000] -> %4X\n", opcode);

    // opcode & 0xF000 performs an AND operation which masks the opcode to just
    // displaying the first bit and from there we can write a switch statement
    // for each opcode.
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000: // 0x00E0: clears the screen
            gfx_clear();
            draw_flag = true;
            prog_counter += 2;
            break;
        case 0x000E: // 0x00EE: Returns from subroutine
            printf("== RETURNING FROM SUBROUTINE ==\n");
            // == Pop from stack
            prog_counter = stack[sp];
            stack[sp] = 0; // clears the value from the stack
            // logic for moving sp down
            if (stack_current_size > 1)
                sp--;
            else // No entries in stack
                sp = 0;
            stack_current_size--;
            prog_counter += 2;
            read_stack(); //! Debugging
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%4X\n", opcode);
            break;
        }
        break;
    case 0x1000: //! 1NNN: Jumps to address NNN
        prog_counter = opcode & 0x0FFF;
        break;
    case 0x2000: // 0x2NNN: Calls subroutine at address NNN
        printf("== CALLING SUBROUTINE ==\n");
        if (stack_current_size > 0)
            sp++;
        stack_current_size++;
        stack[sp] = prog_counter;
        prog_counter = opcode & 0x0FFF;
        read_stack(); //! Debugging
        break;
    case 0x3000: // 3XNN: Skips the next instruction if VX == NN (usually the
                 // next instruction is a jump to skip a code block).
        // If the register at 0 is equivalent to the 8 bit constant skip next
        // step
        //* NOTE: Don't forget to shift your bits when searching for
        //* an register address. Otherwise in this case you'll be
        //* accessing memory at (possibly) address 3840!
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            prog_counter += 4;
        else
            prog_counter += 2;
        break;
    case 0x4000: // 4XNN: Skips the next instruction if VX != NN (usually the
                 // next instruction is a jump to skip a code block).
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            prog_counter += 4;
        else
            prog_counter += 2;
        break;
    //? Is 5xy0 comparing registers vs. 3xnn and 4xnn where it compares the
    // memory address (8-bit constant)?
    case 0x5000: // 5XY0: Skips the next instruction if VX == VY (usually the
                 // next instruction is a jump to skip a code block).
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            prog_counter += 4;
        else
            prog_counter += 2;
        break;
    case 0x6000: // 6xnn: Sets VX to NN
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        prog_counter += 2;
        break;
    case 0x7000: // 7XNN: Adds NN to VX (carry flag is not changed)
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        prog_counter += 2;
        break;
    case 0x8000: // 0x8XY0-0x8XYE
        switch (opcode & 0x000F) {
        case 0x0000: // 0x8XY0: Sets VX to the value of VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0)];
            prog_counter += 2;
            break;
        case 0x0001: // 0x8XY1: Sets VX to VX *or*  VY  (bitwise OR operation)
            V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0)];
            prog_counter += 2;
            break;
        case 0x0002: // 0x8XY2: Sets VX to VX *and* VY (bitwise AND operation)
            V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0)];
            prog_counter += 2;
            break;
        case 0x0003: // 0x8XY3: Sets VX to VX *xor* VY (bitwise XOR operation)
            V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0)];
            prog_counter += 2;
            break;
        case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's an
                     // overflow, and to 0 when there is not
            //*================================================================
            //* NOTE: So the expression V[(opcode & 0x00F0) >> 4] and
            //* V[(opcode & 0x0F00) >> 8] is just formatting the
            //* opcode to get the address for the register.
            //*================================================================
            //* Register VY is between 2 bytes (16 bits) (4 hexadecimal) and in
            //* order to get the register
            //* properly I need to mask the opcode to just the VY (word?) and
            //* then bit shift it 4 places
            //* over to get the proper address so if you have an opcode like
            //* 0x8A33 we are:
            //* 1. Masking our opcode to get our VY (0x8A33 & 0x00F0) ==
            //* (0x0030)
            //* 2. Masking our opcode to get our VX (0x8A33 & 0x0F00) ==
            //* (0x0A00)
            //* 3. Bitshifting the values over 4 bits (binary) to retrieve
            //* the correct address.
            //*     3a. For VY, this is (0x0030) >> 4 == (0x0003)
            //*     3b. For VX, this is (0x0A00) >> 8 == (0x000A)
            //* 4. We now compare the two values to see if VY + VX will result
            //* in an overflow which
            //* we account for by setting the 16th register (0xF or our carry
            //* flag register) to 1.
            //*================================================================
            if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1;
            else
                V[0xF] = 0;
            //* 5. Add VY to VX
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0)];
            //* 6. Increment prog_counter by 2
            prog_counter += 2;
            break;
        case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when
                     // there's an underflow, and 1 when there is not. (i.e. VF
                     // set to 1 if VX >= VY and 0 if not)
            if ((V[opcode & 0x0F00] >> 8) >= V[opcode & 0x00F0] >> 4)
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4]; // VX -= VY
            prog_counter += 2;
            break;
        case 0x0006: // 0x8XY6: Shifts VX to the right by 1, then stores the
                     // least significant bit of VX prior to the shift into VF.
            // Store the least significant bit of VX into VF
            V[0xF] = (V[(opcode & 0x0F00) >> 8]) & 0x000F;
            // Shift VX to the right by 1
            V[(opcode & 0x0F00) >> 8] >>= 1;
            prog_counter += 2;
            break;
        case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when
                     // there's an underflow, and 1 when there is not. (i.e. VF
                     // set to 1 if VY >= VX).
            if (V[(opcode & 0x0F00) >> 8] <= V[(opcode & 0x00F0) >> 4])
                V[0xF] = 0; // underflow
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] =
                V[(opcode & 0x00F0) >> 4] -
                V[(opcode & 0X0F00) >> 8]; // VX = VY - VX
            prog_counter += 2;
            break;
        case 0x000E: // 0x8XYE
            //* From wikipedia: 8XYE: Shifts vx to the left by 1, then sets
            //* VF to 1 if the most significant bit of VX prior to that
            //* shift was set, or to 0 if it was unset.
            //* Checks if value at register is greater than or equal to 128
            //* which in binary sets the most significant bit to 1.
            if (V[(opcode & 0x0F00) >> 8] >= 0x80)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] <<= 1; // VX <<= 1
            prog_counter += 2;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%4X\n", opcode);
            break;
        }
        break;
    case 0x9000: // 0x9XY0: Skips the next instruction if VX does not equal VY
        if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            prog_counter += 4;
        else
            prog_counter += 2;
        break;
    case 0xA000: // 0xANNN: Sets I to the address NNN
        index_register = opcode & 0x0FFF;
        prog_counter += 2;
        break;
    case 0xB000: // 0xBNNN: Jumps to the address NNN plus V0.
        prog_counter = V[0x0] + (opcode & 0x0FFF);
        break;
    case 0xC000: {
        // 0xCXNN: Sets VX to the result of a bitwise AND operation on
        // a random number (0-255) to NN.
        // V[(opcode & 0x0F00) >> 8] =
        //     (rand() % 0xFF) & (opcode & 0x00FF); // generates random number
        V[(opcode & 0x0F00) >> 8] =
            rand() & (opcode & 0x00FF); // generates random number
        prog_counter += 2;
    } break;
    case 0xD000: // 0xDXYN: Draws a sprite at coordinate (VX, VY)
    {
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x000F;
        unsigned short pixel;

        V[0xF] = 0; // carry flag, used for collision detection
        for (int y_line = 0; y_line < height; y_line++) {
            pixel = memory[index_register + y_line];
            for (int x_line = 0; x_line < 8; x_line++) {
                //?  What is this doing?
                if ((pixel & (0x80 >> x_line)) != 0) {
                    if (gfx[(x + x_line + ((y + y_line) * 64))] == 1)
                        V[0xF] = 1;
                    gfx[x + x_line + ((y + y_line) * 64)] ^= 1;
                }
            }
        }

        draw_flag = true;
        prog_counter += 2;
    } break;
    case 0xE000: // 0xEX9E + EXA1: Get keys . Not implementing till I
                 // figure out how to get graphics set up.
        switch (opcode & 0x00FF) {
        case 0x009E:
            //! Need to find a way to implement key() properly
            if (key[V[(opcode & 0x0F00) >> 8]] == 1)
                prog_counter += 4;
            else
                prog_counter += 2;
            break;
        case 0x00A1:
            //! Need to find a way to implement key() properly
            if (key[V[(opcode & 0x0F00) >> 8]] != 1)
                prog_counter += 4;
            else
                prog_counter += 2;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%4X\n", opcode);
            break;
        }
        break;
    case 0xF000: // 0xFX07-0xFX65
        switch (opcode & 0x00FF) {
        case 0x0007: // 0xFX07: Sets VX to the value of the delay timer.
            V[(opcode & 0x0F00) >> 8] = delay_timer;
            prog_counter += 2;
            break;
        case 0x000A: // 0xFX0A
            //* From Wikipedia: A key press is awaited, and then stored in VX
            //* (blocking operation,
            //* all instruction halted until next key event, delay and sound
            //* timers should continue
            //* processing).
            prog_counter += 2;
            break;
        case 0x0015: // 0xFX15: Sets the delay timer to vx.
            delay_timer = V[(opcode & 0x0F00) >> 8];
            prog_counter += 2;
            break;
        case 0x0018: // 0xFX18: Sets the sound timer to VX.
            sound_timer = V[(opcode & 0x0F00) >> 8];
            prog_counter += 2;
            break;
        case 0x001E: // 0xFX1E: Adds VX to I. VF is not affected.
            index_register += V[(opcode & 0x0F00)];
            prog_counter += 2;
            break;
        case 0x0029: // 0xFX29: Sets I to the location of the sprite for the
                     // character in VX
            index_register = V[(opcode & 0x0F00) >> 8];
            prog_counter += 2;
            break;
        case 0x0033: // 0xFX33: Stores the binary-coded decimal representation
                     // of VX
            memory[index_register] = V[(opcode & 0x0F00) >> 8] / 100;
            memory[index_register + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[index_register + 2] = (V[(opcode & 0x0F00) >> 8] / 100) % 10;
            prog_counter += 2;
            break;
        case 0x0055: // 0xFX55: Stores from V0 to VX (including VX) in memory,
                     // starting at address I. The offset from I is increased by
                     // 1 for each value written, but I itself is left
                     // unmodified.
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                memory[index_register + i] = V[i];
            }
            prog_counter += 2;
            break;
        case 0x0065: // 0xFX65: reg_load(VX, &index_register);
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                V[i] = memory[index_register + i];
            }
            prog_counter += 2;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%4X\n", opcode);
            break;
        }
        break;

    default:
        printf("Unknown opcode [0x0000]: 0x%4X\n", opcode);
        break;
    }
    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void Chip8::gfx_clear() {
    // Clears all values in GFX to 0
    for (int i = 0; i < 64 * 32; i++)
        gfx[i] = 0;
}

void Chip8::gfx_draw_all() {
    for (int i = 0; i < 64 * 32; i++)
        gfx[i] = 1;
}

void Chip8::read_stack() {
    for (int i = 0; i < 16; i++)
        printf("Memory @ %2d [0x0000] ==> %4X\n", i, stack[i]);
}

unsigned char *Chip8::get_gfx() { return gfx; }

bool Chip8::get_draw_flag() { return draw_flag; }

void Chip8::set_draw_flag(bool boolean) { draw_flag = boolean; }

void Chip8::read_binary_opcodes() {
    std::cout << "== READING OPCODES ==" << std::endl;
    // Save opcode
    unsigned short temp_opcode = opcode;
    // Save prog_counter
    unsigned short temp_pc = prog_counter;
    for (int i = 0; i < file_size; i++) {
        if (i % 10 == 0 && i != 0) // formatting
            printf("\n");
        opcode = memory[prog_counter] << 8 | memory[prog_counter + 1];
        prog_counter += 2;
        printf("0x%4X | ", opcode);
    }
    printf("\n");

    // set opcode and pc back to original values
    opcode = temp_opcode;
    prog_counter = temp_pc;
    std::cout << "== OPCODES END ==" << std::endl;
}
