#include "chip.h"
#include <string.h>

#define FILESIZE 3584

void chip_init(chip_t* chip){
    chip->PC = 512; // 0x200 in the RAM
    memset(chip->RAM, 0, sizeof(chip->RAM));
    memset(chip->V, 0, sizeof(chip->Vx));
    chip->I = 0;
    chip->delay = 0;
    chip->sound = 0;
    chip->stack_pointer = 0;
    memset(chip->stack, 0, sizeof(chip->stack));
    memset(chip->keypad, 0, sizeof(chip->keypad));
    memset(chip->grid, 0, sizeof(chip->grid));
}

bool load_ROM(chip_t* chip, const char* filename){
    if(filename == NULL)
        return false;
    FILE * rom_file = fopen(filename, "rb");
    if(rom_file == NULL)
        return false;
    
    /*In total we have 4KB of ram but we can only use 4096-512 = 3584, because the program starts from 512
    so we need to make sure that the given parameter suits the size*/
    int end = fseek(rom_file,0 ,SEEK_END);
    long x = ftell(rom_file);
    if(x > FILESIZE){
        printf("Input file is too big\n");
        return false;
    }
    rewind(rom_file);
    fread(chip->RAM+512, 1, x, (void*)rom_file);
    fclose(rom_file);
    return true;
}

void emulate_cycle(chip_t* chip){
    //Fetch -> decode -> execute

    //Fetch
    uint16_t opcode;
    uint16_t temp1 = chip->RAM[chip->PC];
    temp1 = temp1 << 8;
    uint16_t temp2 =chip->RAM[chip->PC+1];
    opcode = temp1 | temp2;
    chip->PC = chip->PC+2;

    //Decode
    
    switch (opcode & 0xF000)//extracting the right most Byte
    {
        case 0x0000:{//CLS or RET
            //extracting the 12 right most bits
            uint16_t mask = opcode & 0x0FFF;
            if(mask == 0x00EE){
                chip->stack_pointer--;
                chip->PC = chip->stack[chip->stack_pointer];
            }
            else if (mask == 0x00E0){

            }
            break;
        }

        case 0x1000:{// JUMP
            uint16_t mask = opcode & 0x0FFF;
            chip->PC = mask;
            break;
        }

        case 0x2000:{// CALL
            chip->stack[chip->stack_pointer] = chip->PC;// in this address is the address we should return to after function call
            chip->stack_pointer += 1;
            chip->PC = opcode & 0x0FFF;   
            break;
        }

        case 0x3000:{// skip one instruction(2 bytes)
            uint8_t x_reg = (opcode & 0x0F00) >> 8;
            if(chip->V[x_reg] == (opcode & 0x00FF)){
                chip->PC += 2;
            }
            break;
        }
        
        case 0x4000:{
             uint8_t x_reg = (opcode & 0x0F00) >> 8;
            if(chip->V[x_reg] != (opcode & 0x00FF)){
                chip->PC += 2;
            }
            break;
        }//SE - skip next instruction if something

        case 0x5000:{}//SE - skip next instruction if something

        case 0x6000:{//LD set Vx = 2 right bytes
            uint8_t reg = (opcode & 0x0F00) >> 8;
            uint16_t first_byte = opcode & 0x00F0;
            uint16_t second_byte = opcode & 0x000F;
            chip->V[reg] = first_byte | second_byte;
            break;
        }

        case 0x7000:{//ADD Vx = Vx + kk
            uint8_t reg = (opcode & 0x0F00) >> 8;
            uint16_t first_byte = opcode & 0x00F0;
            uint16_t second_byte = opcode & 0x000F;
            chip->V[reg] += first_byte | second_byte;
            break;
        }

        case 0x8000:{
            uint8_t x_reg = (opcode & 0x0F00) >> 8;
            uint16_t y_reg = (opcode & 0x00F0) >> 4;
            if((opcode & 0x000F) == 0){
                chip->V[x_reg] = chip->V[y_reg];
            }

            else if((opcode & 0x000F) == 1){
                chip->V[x_reg] = chip->V[x_reg] | chip->V[y_reg];
            }
            else if((opcode & 0x000F) == 2){
                chip->V[x_reg] = chip->V[x_reg] & chip->V[y_reg];
            }
            
            else if((opcode & 0x000F) == 3){
                chip->V[x_reg] = chip->V[x_reg] ^ chip->V[y_reg];
            }

            else if((opcode & 0x000F) == 4){
                uint16_t sum = chip->V[x_reg] + chip->V[y_reg];
                if(sum > 255)
                    chip->V[0xf] = 1;
                else
                    chip->V[0xf] = 0;
                chip->V[x_reg] = sum;
            }

            else if ((opcode & 0x000F) == 5){
                if (chip->V[x_reg] >= chip->V[y_reg]){
                    chip->V[0xf] = 1;
                }
                else{
                    chip->V[0xf] = 0;
                }
                chip->V[x_reg] -= chip->V[y_reg];
            }
            
            else if ((opcode & 0x000F) == 6){ // one shift right
                if((chip->V[x_reg] & 0x0001) == 1){
                    chip->V[0xf] = 1;
                }
                else if((chip->V[x_reg] & 0x0001) == 0){
                    chip->V[0xf] = 0;
                }
                chip->V[x_reg] >>= 1;
            }
            else if ((opcode & 0x000F) == 7){
                if(chip->V[x_reg] <= chip->V[y_reg])
                    chip->V[0xf] = 1;
                else
                    chip->V[0xf] = 0;
                chip->V[x_reg] = chip->V[y_reg] - chip->V[x_reg];
            }
            else if ((opcode & 0x000F) == 0xE){
                if((chip->V[x_reg] & 0x0080) == 0x0080){
                    chip->V[0xf] = 1;
                }else
                    chip->V[0xf] = 0;
                chip->V[x_reg] <<= 1;
            }
            break;
        }


           

        case 0x9000://skip

        case 0xA000:

        case 0xB000:

        case 0xC000:

        case 0xD000:

        case 0xE000:

        case 0xF000:



        
        default:
            printf("Error in opcode\n");
            break;
    }
}