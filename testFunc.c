#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"

void test8080(CPUstate *cs, _Bool isTest)
{
	if(!isTest)
		return;

	if(cs->pc == 0x0000)
		exit(1);

	if(cs->pc == 0x0005)
	{
		if(cs->c == 2)
			putchar(cs->e);
		else if(cs->c == 9)
		{
			uint16_t adr = cs->e | (cs->d << 8);

			while(cs->memory[adr] != '$')
			{
				putchar(cs->memory[adr]);
				adr++;
			}
			printf("\n");
		}

		uint16_t sp = cs->sp;
		cs->pc = cs->memory[sp] | (cs->memory[sp + 1] << 8);
		cs->sp += 2;
	}
}
