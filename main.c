#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "disassembler.h"
#include "ProcessorInstructions.h"
#include "cpu.h"

int main(){

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(224 * 3, 256 * 3, 0, &window, &renderer);
	SDL_RenderSetScale(renderer,3,3);

	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);

	flags f = {0};

	options op = {.flags = &f, .memory = calloc(65536, sizeof(uint8_t)),.ports = calloc(256, sizeof(uint8_t)) ,.sp = 0xF000};

	//Reading from roms
	
	FILE *file = fopen("roms/invaders", "rb");

	if(file == NULL)
	{
		printf("file no");
		exit(1);
	}

	int byteRoms;
	int count = 0;

	while((byteRoms = fgetc(file)) != EOF)
	{
		op.memory[count] = byteRoms;
		count++;
	}

	fclose(file);

	int cycles = 0;

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

		/*
			if(op.pc ==  0x0000)
				break;

		if(op.pc == 0x0005)
		{
			if(op.c == 2)
				putchar(op.e);
			else if(op.c == 9)
			{
				uint16_t adr = op.e | (op.d << 8);

				while(op.memory[adr] != '$')
				{
					putchar(op.memory[adr]);
					adr++;
				}
				printf("\n");
			}

			uint16_t sp = op.sp;
			op.pc = op.memory[sp] | (op.memory[sp + 1] << 8);
			op.sp += 2;
			continue;
		}
		*/

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
			SDL_Delay(5);
		}
	}

	return 0;
}
