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

	CPUstate cs = {
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

	fread(&cs.memory[0], 1, file_size, file);

	fclose(file);

	//cycle parameters

	int cycles = 0;
	int interrupt_cycles = 0;
	uint32_t frame_start = SDL_GetTicks();
	SDL_Event event;
	int ine_num = 1;

	while(1){


		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				return 0;

			if(event.type == SDL_KEYDOWN)
				switch(event.key.keysym.sym)
				{
					case SDLK_c:
						cs.ports[1] |= 0x01;
						break;
					case SDLK_SPACE:
						cs.ports[1] |= 0x04;
						break;
					case SDLK_a:
						cs.ports[1] |= 0x20;
						break;
					case SDLK_d:
						cs.ports[1] |= 0x40;
						break;
					case SDLK_w:
						cs.ports[1] |= 0x10;
						break;
				}

			if(event.type == SDL_KEYUP)
				switch(event.key.keysym.sym)
				{
					case SDLK_c:
						cs.ports[1] &= ~0x01;
						break;
					case SDLK_SPACE:
						cs.ports[1] &= ~0x04;
						break;
					case SDLK_a:
						cs.ports[1] &= ~0x20;
						break;
					case SDLK_d:
						cs.ports[1] &= ~0x40;
						break;
					case SDLK_w:
						cs.ports[1] &= ~0x10;
						break;
				}
		}

		test8080(&cs, 0);

		//disassembler(cs.memory,cs.pc);
		Instructions(&cs);
		cycles += cs.cycles;
		interrupt_cycles += cs.cycles;

		if(interrupt_cycles >= 16666)
		{

			if(cs.ie)
			{
				cs.ie = 0;
				cs.sp -= 2;

				cs.memory[cs.sp] = cs.pc & 0xff;
				cs.memory[cs.sp + 1] = (cs.pc >> 8) & 0xff;

				cs.pc = ine_num * 8;
			}

			interrupt_cycles -= 16666;

			if(ine_num == 1)
				ine_num = 2;
			else
				ine_num = 1;

		}

		if(cycles >= 33333)
		{
			cycles -= 33333;


	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
			for(int y = 0; y < 224; y++)
			{
				for(int x_byte = 0; x_byte < 32; x_byte++)
				{
					uint8_t byte = cs.memory[0x2400 + y * 32 + x_byte];
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

			frame_start = SDL_GetTicks();
		}
	}

	return 0;
}
