#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "disassembler.h"
#include "ProcessorInstructions.h"
#include "cpu.h"
#include "testFunc.h"

int main(){

	//SDL initialization

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(224 * 3, 256 * 3, 0, &window, &renderer);
	SDL_RenderSetScale(renderer,3,3);

	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);

	//initialize processor structure

	flags f = {0};

	CPUstate op = {
		.flags = &f,
		.memory = calloc(65536, sizeof(uint8_t)),
		.ports = calloc(256, sizeof(uint8_t)),
		.sp = 0xF000
	};

	//Reading from roms
	
	FILE *file = fopen("roms/invaders", "rb");

	if(file == NULL)
	{
		printf("File not found\n");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	fread(&op.memory[0], 1, file_size, file);

	fclose(file);

	//

	int cycles = 0;

	SDL_Event event;
	int ine_num = 1;

	while(1){

		uint32_t frame_start = SDL_GetTicks();

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				return 0;

			if(event.type == SDL_KEYDOWN)
				switch(event.key.keysym.sym)
				{
					case SDLK_c:
						op.ports[1] |= 0x01;
						break;
					case SDLK_SPACE:
						op.ports[1] |= 0x04;
						break;
					case SDLK_a:
						op.ports[1] |= 0x20;
						break;
					case SDLK_d:
						op.ports[1] |= 0x40;
						break;
					case SDLK_w:
						op.ports[1] |= 0x10;
						break;
				}

			if(event.type == SDL_KEYUP)
				switch(event.key.keysym.sym)
				{
					case SDLK_c:
						op.ports[1] &= ~0x01;
						break;
					case SDLK_SPACE:
						op.ports[1] &= ~0x04;
						break;
					case SDLK_a:
						op.ports[1] &= ~0x20;
						break;
					case SDLK_d:
						op.ports[1] &= ~0x40;
						break;
					case SDLK_w:
						op.ports[1] &= ~0x10;
						break;
				}
		}

		test8080(&op, 0);

		//disassembler(op.memory,op.pc);
		Instructions(&op);
		cycles += op.cycles;

		if(cycles >= 33333)
		{
			cycles -= 33333;

			if(op.ie)
			{
				op.ie = 0;
				op.sp -= 2;

				op.memory[op.sp] = op.pc & 0xff;
				op.memory[op.sp + 1] = (op.pc >> 8) & 0xff;

				op.pc = ine_num * 8;
			}

			if(ine_num == 1)
				ine_num = 2;
			else
				ine_num = 1;

	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
			for(int y = 0; y < 224; y++)
			{
				for(int x_byte = 0; x_byte < 32; x_byte++)
				{
					uint8_t byte = op.memory[0x2400 + y * 32 + x_byte];
					for(int bit = 0; bit < 8; bit++)
					{
						int x = x_byte * 8 + bit;

						if(byte & (1 << bit))
						{
							int screen_x = y;
							int screen_y = 255 - x;

							SDL_RenderDrawPoint(renderer, screen_x, screen_y);
						}
					}
				}
			}
			SDL_RenderPresent(renderer);

			uint32_t now_frame = SDL_GetTicks() - frame_start;

			if(now_frame < 16)
				SDL_Delay(16 - now_frame);
		}
	}

	return 0;
}
