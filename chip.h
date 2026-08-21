#ifndef __CHIP__
#define __CHIP__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct chip_{
    uint8_t RAM[4096];
    uint8_t V[16];//Registers of 8 bit, registers V0 to VF(0 to 15)
    uint16_t I; // used to store memory address, ONLY 12 RIGHTMOST Bytes are being used
    uint8_t delay;
    uint8_t sound;
    uint16_t PC;// program counter
    uint16_t stack[16];//stack of instructions
    uint8_t stack_pointer;

    bool keypad[16];
    uint8_t grid[64 * 32];
}chip_t;

void chip_init(chip_t*);

bool load_ROM(chip_t* chip, const char* filename);
void emulate_cycle(chip_t* chip);

#endif