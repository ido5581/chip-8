#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "chip.h"

#define CYCLES_PER_FRAME 60

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    chip_t chip;
    chip_init(&chip);
    const char* rom_name = "snek.ch8";
    //const char* rom_name = "br8kout.ch8";
    //const char* rom_name = "octojam2title.ch8";

    if (!load_ROM(&chip, rom_name)) {
        printf("Failed to load ROM: %s\n", rom_name);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
       
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                bool state = (e.type == SDL_KEYDOWN);
                switch (e.key.keysym.sym) {
                    case SDLK_x: chip.keypad[0x0] = state; break;
                    case SDLK_1: chip.keypad[0x1] = state; break;
                    case SDLK_2: chip.keypad[0x2] = state; break;
                    case SDLK_3: chip.keypad[0x3] = state; break;
                    case SDLK_q: chip.keypad[0x4] = state; break;
                    case SDLK_w: chip.keypad[0x5] = state; break;
                    case SDLK_e: chip.keypad[0x6] = state; break;
                    case SDLK_a: chip.keypad[0x7] = state; break;
                    case SDLK_s: chip.keypad[0x8] = state; break;
                    case SDLK_d: chip.keypad[0x9] = state; break;
                    case SDLK_z: chip.keypad[0xA] = state; break;
                    case SDLK_c: chip.keypad[0xB] = state; break;
                    case SDLK_4: chip.keypad[0xC] = state; break;
                    case SDLK_r: chip.keypad[0xD] = state; break;
                    case SDLK_f: chip.keypad[0xE] = state; break;
                    case SDLK_v: chip.keypad[0xF] = state; break;
                    default: break;
                }
            }
        }

      
        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            emulate_cycle(&chip);
        }

      
        if (chip.delay > 0) {
            chip.delay--;
        }
        if (chip.sound > 0) {
            chip.sound--;
        }

        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                if (chip.grid[y * 64 + x] == 1) {
                    SDL_Rect pixel = { .x = x * 10, .y = y * 10, .w = 10, .h = 10 };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        SDL_RenderPresent(renderer);

        
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}