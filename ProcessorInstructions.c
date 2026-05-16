#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"

void UnimplementedInstruction(options *optionsCPU){
	printf("Error:  Unimplemented instruction $%02X", optionsCPU->memory[optionsCPU->pc]);
	exit(1);
}

//working with flags(dont forget to move them)

void ResetFlagsToZeroZSP(flags* f){
	f->z = f->s = f->p = 0;
}

int NumberOfUnitsByte(uint8_t number)
{
	uint8_t mask =  0b10000000;
	int sum = 0;
	do{
		if(number & mask)
			sum+=1;
	}while(mask >>= 1);

	return  sum;
}


void FlagOptimizationZSP(flags* f, uint8_t res)
{
	if(!res)
		f->z = 1;
	if(res & 0x80)
		f->s = 1;
	if(NumberOfUnitsByte(res) % 2 == 0)
		f->p = 1;
}


void Instructions(options *optionsCPU){

	uint8_t *opcode = &optionsCPU->memory[optionsCPU->pc];

	switch(*opcode){
		case 0x00:
			optionsCPU->cycles = 4;
			break;
		case 0x01:
			optionsCPU->cycles = 10;
			optionsCPU->b = opcode[2];
			optionsCPU->c = opcode[1];
			optionsCPU->pc += 2;
			break;
		case 0x02:{
			optionsCPU->cycles = 7;
			uint16_t adr = (optionsCPU->b << 8) | optionsCPU->c;
			optionsCPU->memory[adr] = optionsCPU->a;
			break;}
		case 0x03:{
			optionsCPU->cycles = 5;
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			BC++;
			optionsCPU->b = (BC >> 8) & 0xFF;
			optionsCPU->c = BC & 0xFF;
			break;}
		case 0x04:{
			optionsCPU->cycles = 5;
			optionsCPU->b += 1;

			optionsCPU->flags->ac = (optionsCPU->b & 0xf) == 0xf;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags,optionsCPU->b);
			break;}
		case 0x05:{
			optionsCPU->cycles = 5;
			optionsCPU->b -= 1;

			optionsCPU->flags->ac = (optionsCPU->b & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->b);
			break;}
		case 0x06:
			optionsCPU->cycles = 7;
			optionsCPU->b = optionsCPU->memory[(optionsCPU->pc)+ 1];
			optionsCPU->pc++;
			break;
		case 0x07:{
			optionsCPU->cycles = 4;
			optionsCPU->flags->cy = 0;
			uint8_t temp_byte = optionsCPU->a & 0b10000000;
			optionsCPU->a <<= 1;
			temp_byte >>= 7;
			optionsCPU->a = optionsCPU->a | temp_byte;
			if(temp_byte)
				optionsCPU->flags->cy =  1;
			break;}
		case 0x08:
			optionsCPU->cycles = 4;
			break;
		case 0x09:{
			optionsCPU->cycles = 10;
			uint16_t HL =(optionsCPU->h << 8) | optionsCPU->l;
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			uint32_t res_hl = HL + BC;

			optionsCPU->h = res_hl >> 8;
			optionsCPU->l = res_hl;

			optionsCPU->flags->cy = res_hl > 0xFFFF;
			break;}
		case 0x0a:{
			optionsCPU->cycles = 7;
			uint16_t adr = (optionsCPU->b << 8) | optionsCPU->c;
			optionsCPU->a = optionsCPU->memory[adr];
			break;}
		case 0x0b:{
			optionsCPU->cycles = 5;
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			BC -= 1;

			optionsCPU->b = BC >> 8;
			optionsCPU->c = BC;
			break;}
		case 0x0c:
			optionsCPU->cycles = 5;
			optionsCPU->c++;

			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->c & 0xf) == 0xf;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->c);
			break;
		case 0x0d:
			optionsCPU->cycles = 5;
			optionsCPU->c--;

			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->c & 0x0F) == 0x0;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->c);
			break;
		case 0x0e:
			optionsCPU->cycles = 7;
			optionsCPU->c = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x0f:{
			optionsCPU->cycles = 4;
			optionsCPU->flags->cy = (optionsCPU->a & 1) == 1;
			uint8_t temp_byte = (optionsCPU->a << 7) & 0b10000000;
			optionsCPU->a = (optionsCPU->a >> 1) | temp_byte;
			break;}
		case 0x10:
			optionsCPU->cycles = 4;
			break;
		case 0x11:
			optionsCPU->cycles = 10;
			optionsCPU->e = optionsCPU->memory[++optionsCPU->pc];
			optionsCPU->d = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x12:{
			optionsCPU->cycles = 7;
			uint16_t adr = (optionsCPU->d << 8) | optionsCPU->e;
			optionsCPU->memory[adr] = optionsCPU->a;
			break;}
		case 0x13:{
			optionsCPU->cycles = 5;
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			DE++;
			optionsCPU->d = DE >> 8;
			optionsCPU->e = DE;
			break;}
		case 0x14:
			optionsCPU->cycles = 5;
			optionsCPU->d++;

			optionsCPU->flags->ac = (optionsCPU->b & 0xf) == 0xf;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->d);
			break;
		case 0x15:
			optionsCPU->cycles = 5;
			optionsCPU->d--;

			optionsCPU->flags->ac = (optionsCPU->d & 0x0F) == 0x00;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->d);
			break;
		case 0x16:
			optionsCPU->cycles = 7;
			optionsCPU->d = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x17:{
			optionsCPU->cycles = 4;
			uint8_t temp_byte = optionsCPU->flags->cy;
			optionsCPU->flags->cy = (optionsCPU->a >> 7) & 0x01;
			optionsCPU->a = (optionsCPU->a << 1) | temp_byte;
			break;}
		case 0x18:
			optionsCPU->cycles = 4;
			break;
		case 0x19:{
			optionsCPU->cycles = 10;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			uint32_t res = HL + DE;

			optionsCPU->h = res >> 8;
			optionsCPU->l = res;

			optionsCPU->flags->cy = res > 0xFFFF;
			break;}
		case 0x1a:{
			optionsCPU->cycles = 7;
			uint16_t adr = (optionsCPU->d << 8) | optionsCPU->e;
			optionsCPU->a = optionsCPU->memory[adr];
			break;}
		case 0x1b:{
			optionsCPU->cycles = 5;
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			DE-=1;
			optionsCPU->d = DE >> 8;
			optionsCPU->e = DE;
			break;}
		case 0x1c:
			optionsCPU->cycles = 5;
			optionsCPU->e += 1;

			optionsCPU->flags->ac = (optionsCPU->e & 0xf) == 0xf;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->e);
			break;
		case 0x1d:
			optionsCPU->cycles = 5;
			optionsCPU->e -= 1;

			optionsCPU->flags->ac = (optionsCPU->e & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->e);
			break;
		case 0x1e:
			optionsCPU->cycles = 7;
			optionsCPU->e = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x1f:{
			optionsCPU->cycles = 4;
			uint8_t temp_byte = optionsCPU->flags->cy << 7;
			optionsCPU->flags->cy = optionsCPU->a & 0x01;
			optionsCPU->a = (optionsCPU->a >> 1) | temp_byte;
			break;}
		case 0x20:
			optionsCPU->cycles = 4;
			break;
		case 0x21:
			optionsCPU->cycles = 10;
			optionsCPU->l = optionsCPU->memory[++optionsCPU->pc];
			optionsCPU->h = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x22:{
			optionsCPU->cycles = 16;
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = ((optionsCPU->memory[pc + 2]) << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->memory[adr] = optionsCPU->l;
			optionsCPU->memory[adr + 1] = optionsCPU->h;
			optionsCPU->pc += 2;
			break;}
		case 0x23:{
			optionsCPU->cycles = 5;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			HL += 1;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			break;}
		case 0x24:
			optionsCPU->cycles = 5;
			optionsCPU->h += 1;

			optionsCPU->flags->ac = (optionsCPU->h & 0xf) == 0xf;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->h);
			break;
		case 0x25:
			optionsCPU->cycles = 5;
			optionsCPU->h -= 1;
			optionsCPU->flags->ac = (optionsCPU->h & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->h);
			break;
		case 0x26:{
			optionsCPU->cycles = 7;
			uint8_t nb = optionsCPU->memory[optionsCPU->pc + 1];
			optionsCPU->h = nb;
			optionsCPU->pc += 1;
			break;}
		case 0x27:{
			optionsCPU->cycles = 4;
			uint16_t temp_a = optionsCPU->a;
			uint8_t old_a = temp_a;
			uint8_t add = 0;

			if((temp_a & 0x0F) > 9 || optionsCPU->flags->ac)
			{
				add += 0x06;
				temp_a += 0x06;
			}

			if((temp_a >> 4) > 9 ||  optionsCPU->flags->cy)
			{
				temp_a += 0x60;
				optionsCPU->flags->cy = 1;
			}

			optionsCPU->a = temp_a;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((old_a & 0x0F) +(add & 0x0F)) > 0x0F;

			break;}
		case 0x28:
			optionsCPU->cycles = 4;
			break;
		case 0x29:{
			optionsCPU->cycles = 10;
			uint32_t HL =  (optionsCPU->h << 8) | optionsCPU->l;
			HL += HL;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			optionsCPU->flags->cy = HL > 0xFFFF;
			break;}
		case 0x2a:{
			optionsCPU->cycles = 16;
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->l = optionsCPU->memory[adr];
			optionsCPU->h = optionsCPU->memory[adr + 1];
			optionsCPU->pc += 2;
			break;}
		case 0x2b:{
			optionsCPU->cycles = 5;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			HL -= 1;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			break;}
		case 0x2c:
			optionsCPU->cycles = 5;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->l & 0x0F) + 1) > 0x0F;
			optionsCPU->l += 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->l);
			break;
		case 0x2d:
			optionsCPU->cycles = 5;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->l & 0x0F) == 0x00;
			optionsCPU->l -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->l);
			break;
		case 0x2e:
			optionsCPU->cycles = 7;
			optionsCPU->l =optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x2f:
			optionsCPU->cycles = 4;
			optionsCPU->a = ~optionsCPU->a;
			break;
		case 0x30:
			optionsCPU->cycles = 4;
			break;
		case 0x31:{
			optionsCPU->cycles = 10;
			uint8_t low = optionsCPU->memory[++optionsCPU->pc];
			uint8_t high = optionsCPU->memory[++optionsCPU->pc];

			optionsCPU->sp = (high << 8) | low;
			break;}
		case 0x32:{
			optionsCPU->cycles = 13;
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->memory[adr] = optionsCPU->a;
			optionsCPU->pc += 2;
			break;}
		case 0x33:
			optionsCPU->cycles = 5;
			optionsCPU->sp += 1;
			break;
		case 0x34:{
			optionsCPU->cycles = 10;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->flags->ac = ((optionsCPU->memory[adr] & 0x0F) + 1) > 0x0F;
			optionsCPU->memory[adr] += 1;;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->memory[adr]);
			break;}
		case 0x35:{
			optionsCPU->cycles = 10;
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[adr] -= 1;
			optionsCPU->flags->ac = (optionsCPU->memory[adr] & 0x0F) == 0x00;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->memory[adr]);
			break;}
		case 0x36:{
			optionsCPU->cycles = 10;
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[adr] = optionsCPU->memory[++optionsCPU->pc];
			break;}
		case 0x37:
			optionsCPU->cycles = 4;
			optionsCPU->flags->cy = 1;
			break;
		case 0x38:
			optionsCPU->cycles = 4;
			break;
		case 0x39:{
			optionsCPU->cycles = 10;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint32_t res = HL + optionsCPU->sp;
			optionsCPU->flags->cy = res > 0xFFFF;
			optionsCPU->h = res >> 8;
			optionsCPU->l = res;
			break;}
		case 0x3a:{
			optionsCPU->cycles = 13;
			uint16_t pc = optionsCPU->pc;
			uint16_t adr  = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->a = optionsCPU->memory[adr];
			optionsCPU->pc += 2;
			break;}
		case 0x3b:
			optionsCPU->cycles = 5;
			optionsCPU->sp -= 1;
			break;
		case 0x3c:
			optionsCPU->cycles = 5;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a += 1;
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) == 0;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;
		case 0x3d:
			optionsCPU->cycles = 5;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) == 0x00;
			optionsCPU->a -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;
		case 0x3e:
			optionsCPU->cycles = 7;
			optionsCPU->a = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x3f:
			optionsCPU->cycles = 4;
			optionsCPU->flags->cy = !optionsCPU->flags->cy;
			break;
		case 0x40:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->b;
			break;
		case 0x41:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->c;
			break;
		case 0x42:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->d;
			break;
		case 0x43:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->e;
			break;
		case 0x44:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->h;
			break;
		case 0x45:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->l;
			break;
		case 0x46:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->b = optionsCPU->memory[HL];
			break;}
		case 0x47:
			optionsCPU->cycles = 5;
			optionsCPU->b = optionsCPU->a;
			break;
		case 0x48:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->b;
			break;
		case 0x49:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->c;
			break;
		case 0x4a:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->d;
			break;
		case 0x4b:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->e;
			break;
		case 0x4c:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->h;
			break;
		case 0x4d:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->l;
			break;
		case 0x4e:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->c = optionsCPU->memory[HL];
			break;}
		case 0x4f:
			optionsCPU->cycles = 5;
			optionsCPU->c = optionsCPU->a;
			break;
		case 0x50:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->b;
			break;
		case 0x51:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->c;
			break;
		case 0x52:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->d;
			break;
		case 0x53:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->e;
			break;
		case 0x54:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->h;
			break;
		case 0x55:
			optionsCPU->cycles = 5;
			optionsCPU->d = optionsCPU->l;
			break;
		case 0x56:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->d = optionsCPU->memory[HL];
			break;}
		case 0x57:
			optionsCPU->cycles = 5;
			optionsCPU->d =optionsCPU->a;
			break;
		case 0x58:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->b;
			break;
		case 0x59:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->c;
			break;
		case 0x5a:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->d;
			break;
		case 0x5b:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->e;
			break;
		case 0x5c:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->h;
			break;
		case 0x5d:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->l;
			break;
		case 0x5e:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->e = optionsCPU->memory[HL];
			break;}
		case 0x5f:
			optionsCPU->cycles = 5;
			optionsCPU->e = optionsCPU->a;
			break;
		case 0x60:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->b;
			break;
		case 0x61:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->c;
			break;
		case 0x62:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->d;
			break;
		case 0x63:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->e;
			break;
		case 0x64:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->h;
			break;
		case 0x65:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->l;
			break;
		case 0x66:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->h = optionsCPU->memory[HL];
			break;}
		case 0x67:
			optionsCPU->cycles = 5;
			optionsCPU->h = optionsCPU->a;
			break;
		case 0x68:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->b;
			break;
		case 0x69:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->c;
			break;
		case 0x6a:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->d;
			break;
		case 0x6b:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->e;
			break;
		case 0x6c:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->h;
			break;
		case 0x6d:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->l;
			break;
		case 0x6e:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->l = optionsCPU->memory[HL];
			break;}
		case 0x6f:
			optionsCPU->cycles = 5;
			optionsCPU->l = optionsCPU->a;
			break;
		case 0x70:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->b;
			break;}
		case 0x71:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->c;
			break;}
		case 0x72:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->d;
			break;}
		case 0x73:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->e;
			break;}
		case 0x74:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->h;
			break;}
		case 0x75:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->l;
			break;}
		case 0x76:
			optionsCPU->cycles = 7;
			optionsCPU->hal = 1;
			break;
		case 0x77:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->a;
			break;}
		case 0x78:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->b;
			break;
		case 0x79:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->c;
			break;
		case 0x7a:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->d;
			break;
		case 0x7b:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->e;
			break;
		case 0x7c:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->h;
			break;
		case 0x7d:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->l;
			break;
		case 0x7e:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a = optionsCPU->memory[HL];
			break;}
		case 0x7f:
			optionsCPU->cycles = 5;
			optionsCPU->a = optionsCPU->a;
			break;
		case 0x80:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->b;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->b & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x81:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->c;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->c & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x82:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->d;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->d & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x83:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->e;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->e & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x84:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->h;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->h & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x85:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->l;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->l & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x86:{
			optionsCPU->cycles = 7;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a + optionsCPU->memory[HL];
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->memory[HL] & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x87:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->a;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->a & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x88:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->b & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->b + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x89:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->c & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->c + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8a:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->d & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->d + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8b:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->e & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->e + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8c:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->h & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->h + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8d:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->l & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->l + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8e:{
			optionsCPU->cycles = 7;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a + optionsCPU->memory[HL] + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->memory[HL] & 0x0F) + cy) > 0x0F;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8f:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->a & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->a + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x90:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->b;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->b ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res8;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x91:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->c;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->c ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x92:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->d;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->d ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x93:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->e;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->e ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x94:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->h;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->h ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x95:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->l;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->l ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x96:{
			optionsCPU->cycles = 7;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL];
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->memory[HL] ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x97:{
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->a;
			uint8_t res8 =res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->a ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x98:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->b - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->b ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x99:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->c - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->c ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9a:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->d - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->d ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9b:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->e - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->e ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9c:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->h - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->h ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9d:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->l - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->l ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9e:{
			optionsCPU->cycles = 7;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL] - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->memory[HL] ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9f:{
			optionsCPU->cycles = 4;
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a - optionsCPU->a - cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->a ^ cy ^ res8) & 0x10) != 0;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0xa0:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->b) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa1:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->c) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa2:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->d) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa3:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->e) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa4:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->h) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa5:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->l) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa6:{
			optionsCPU->cycles = 7;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a &= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->memory[HL]) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xa7:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->a &= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = ((optionsCPU->a | optionsCPU->a) & 0x08) != 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa8:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa9:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xaa:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xab:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xac:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xad:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xae:{
			optionsCPU->cycles = 7;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a ^= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xaf:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb0:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb1:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb2:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb3:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb4:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb5:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb6:{
			optionsCPU->cycles = 7;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a |= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xb7:
			optionsCPU->cycles = 4;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb8:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->b;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->b ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->b;
			break;}
		case 0xb9:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->c;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->c ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->c;
			break;}
		case 0xba:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->d;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->d ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->d;
			break;}
		case 0xbb:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->e;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->e ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->e;
			break;}
		case 0xbc:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->h;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->h ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->h;
			break;}
		case 0xbd:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->l;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->l ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->l;
			break;}
		case 0xbe:{
			optionsCPU->cycles = 7;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL];
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->memory[HL] ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->memory[HL];
			break;}
		case 0xbf:{
			optionsCPU->cycles = 4;
			uint16_t res = optionsCPU->a - optionsCPU->a;
			uint16_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ optionsCPU->a ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->a;
			break;}
		case 0xc0:{
			optionsCPU->cycles = 5;
			if(!optionsCPU->flags->z)
			{
				uint16_t sp = optionsCPU->sp;
				uint8_t high = optionsCPU->memory[sp + 1];
				uint8_t low = optionsCPU->memory[sp];
				optionsCPU->pc = (high << 8) | low;
				optionsCPU->sp +=2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xc1:{
			optionsCPU->cycles = 10;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->b = optionsCPU->memory[sp + 1];
			optionsCPU->c = optionsCPU->memory[sp];
			optionsCPU->sp += 2;
			break;}
		case 0xc2:{
			optionsCPU->cycles = 10;
			if(!optionsCPU->flags->z)
			{
				uint16_t adr = (optionsCPU->memory[optionsCPU->pc + 2] << 8) | optionsCPU->memory[optionsCPU->pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xc3:{
			optionsCPU->cycles = 10;
			uint16_t pc =  optionsCPU->pc;
			optionsCPU->pc = ((optionsCPU->memory[pc + 2]) << 8) | optionsCPU->memory[pc + 1];
			return;
			break;}
		case 0xc4:{
			optionsCPU->cycles = 11;
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];

			if(!optionsCPU->flags->z)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr;
				optionsCPU->cycles = 17;
				return;
			}
			else
				optionsCPU->pc += 2;
			break;}
		case 0xc5:
			optionsCPU->cycles = 11;
			optionsCPU->memory[optionsCPU->sp - 2] = optionsCPU->c;
			optionsCPU->memory[optionsCPU->sp - 1] = optionsCPU->b;
			optionsCPU->sp -= 2;
			break;
		case 0xc6:
			optionsCPU->cycles = 7; {
			uint16_t res = optionsCPU->a + optionsCPU->memory[optionsCPU->pc + 1];
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->memory[optionsCPU->pc +  1] & 0x0F)) > 0x0F;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->pc += 1;
			break;}
		case 0xc7:{
			optionsCPU->cycles = 11;
			uint16_t res = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = res >> 8; 
			optionsCPU->memory[sp - 2] = res;
			optionsCPU->sp-= 2;
			optionsCPU->pc = 0x0000;
			return;
			break;}
		case 0xc8:{
			optionsCPU->cycles = 5;
			if(optionsCPU->flags->z)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = ((optionsCPU->memory[sp + 1]) << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xc9:{
			optionsCPU->cycles = 10;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
			optionsCPU->sp += 2;
			return;
			break;}
		case 0xca:{
			optionsCPU->cycles = 10;	
			if(optionsCPU->flags->z)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xcb:
			optionsCPU->cycles = 4;
			break;
		case 0xcc:{
			optionsCPU->cycles = 11;
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->z)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xcd:{
			optionsCPU->cycles = 17;
			uint16_t sp = optionsCPU->sp;
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			pc +=  3;
			optionsCPU->memory[sp - 1] = pc >> 8;
			optionsCPU->memory[sp - 2] = pc;
			optionsCPU->sp -=2;
			optionsCPU->pc = adr;
			return;
			break;}
		case 0xce:{
			optionsCPU->cycles = 7;
			uint8_t data = optionsCPU->memory[optionsCPU->pc + 1];
			uint16_t res= optionsCPU->a + data + optionsCPU->flags->cy;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (data & 0x0F) + optionsCPU->flags->cy) > 0x0F;
			optionsCPU->a = res;

			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->pc += 1;
			break;}
		case 0xcf:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0008;
			return;
			break;}
		case 0xd0:{
			optionsCPU->cycles = 5;
			if(!optionsCPU->flags->cy)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xd1:{
			optionsCPU->cycles = 10;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->e = optionsCPU->memory[sp];
			optionsCPU->d = optionsCPU->memory[sp + 1];
			optionsCPU->sp += 2;
			break;}
		case 0xd2:{
			optionsCPU->cycles = 10;
			if(!optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xd3:{
			optionsCPU->cycles = 10;
			uint8_t port = optionsCPU->memory[optionsCPU->pc +1];
			switch(port)
			{
				case 2:
					optionsCPU->so = optionsCPU->a & 0x07;
					break;
				case 4:
					optionsCPU->sr = (optionsCPU->a << 8) | (optionsCPU->sr >> 8);
					break;
			}
			optionsCPU->pc += 1;
			break;}
		case 0xd4:{
			optionsCPU->cycles = 11;
			if(!optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -= 2;
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xd5:{
			optionsCPU->cycles = 11;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = optionsCPU->d;
			optionsCPU->memory[sp - 2] = optionsCPU->e;
			optionsCPU->sp -= 2;
			break;}
		case 0xd6:{
			optionsCPU->cycles = 7;
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a - data;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ data ^ res8) & 0x10) != 0;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = res > 0xFF;
			break;}
		case 0xd7:{
			optionsCPU->cycles = 11;
			uint16_t sp = optionsCPU->sp;
			uint16_t pc = optionsCPU->pc + 1;
			optionsCPU->memory[sp - 1] = pc >> 8;
			optionsCPU->memory[sp - 2] = pc;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0010;
			return;
			break;}
		case 0xd8:{
			optionsCPU->cycles = 5;
			if(optionsCPU->flags->cy)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xd9:
			optionsCPU->cycles = 4;
			break;
		case 0xda:{
			optionsCPU->cycles = 10;
			if(optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xdb:{
			optionsCPU->cycles = 10;
			uint8_t port = optionsCPU->memory[optionsCPU->pc +1];
			switch(port)
			{
				case 3:
					optionsCPU->a = (optionsCPU->sr >> (8 - optionsCPU->so)) & 0xff;
					break;
				default:
					optionsCPU->a = optionsCPU->ports[port];
					break;
			}
			optionsCPU->pc += 1;
			break;}
		case 0xdc:{
			optionsCPU->cycles = 11;
			if(optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -= 2;
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xdd:
			optionsCPU->cycles = 4;
			break;
		case 0xde:{
			optionsCPU->cycles = 7;
			uint8_t data = optionsCPU->memory[optionsCPU->pc + 1];
			uint16_t res= optionsCPU->a - data - optionsCPU->flags->cy;
			uint8_t res8 = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ data ^ res8) & 0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = optionsCPU->a < (data + optionsCPU->flags->cy);

			optionsCPU->a = res;

			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->pc += 1;
			break;}
		case 0xdf:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0018;
			return;
			break;}
		case 0xe0:{
			optionsCPU->cycles = 5;
			if(!optionsCPU->flags->p)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xe1:{
			optionsCPU->cycles = 10;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->l = optionsCPU->memory[sp];
			optionsCPU->h = optionsCPU->memory[sp + 1];
			optionsCPU->sp += 2;
			break;}
		case 0xe2:{
			optionsCPU->cycles = 10;
			if(!optionsCPU->flags->p)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xe3:{
			optionsCPU->cycles = 18;
			uint16_t sp = optionsCPU->sp;
			uint8_t temp_sp = optionsCPU->memory[sp];
			uint8_t temp_sp_1 = optionsCPU->memory[sp + 1];

			optionsCPU->memory[sp] = optionsCPU->l;
			optionsCPU->memory[sp + 1] = optionsCPU->h;

			optionsCPU->l = temp_sp;
			optionsCPU->h = temp_sp_1;
			break;}
		case 0xe4:{
			optionsCPU->cycles = 11;
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(!optionsCPU->flags->p)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xe5:{
			optionsCPU->cycles = 11;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = optionsCPU->h;
			optionsCPU->memory[sp - 2] = optionsCPU->l;
			optionsCPU->sp -= 2;
			break;}
		case 0xe6:{
			optionsCPU->cycles = 7;
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a & data;
			optionsCPU->flags->ac = ((optionsCPU->a | data) & 0x08) != 0;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			break;}
		case 0xe7:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0020;
			return;
			break;}
		case 0xe8:{
			optionsCPU->cycles = 5;
			if(optionsCPU->flags->p)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xe9:
			optionsCPU->cycles = 5;
			optionsCPU->pc = (optionsCPU->h << 8) | optionsCPU->l;
			return;
			break;
		case 0xea:{
			optionsCPU->cycles = 10;
			if(optionsCPU->flags->p)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xeb:{
			optionsCPU->cycles = 4;
			uint8_t temp_h = optionsCPU->h;
			uint8_t temp_l = optionsCPU->l;

			optionsCPU->h = optionsCPU->d;
			optionsCPU->l = optionsCPU->e;

			optionsCPU->d = temp_h;
			optionsCPU->e = temp_l;
			break;}
		case 0xec:{
			optionsCPU->cycles = 11;
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->p)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xed:
			optionsCPU->cycles = 4;
			break;
		case 0xee:{
			optionsCPU->cycles = 7;
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a ^ data;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			optionsCPU->flags->ac = 0;
			break;}
		case 0xef:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0028;
			return;
			break;}
		case 0xf0:{
			optionsCPU->cycles = 5;
			if(!optionsCPU->flags->s)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xf1:{
			optionsCPU->cycles = 10;
			uint16_t sp = optionsCPU->sp;
			uint8_t flags =  optionsCPU->memory[sp];
			optionsCPU->a = optionsCPU->memory[sp + 1];

			optionsCPU->flags->s = flags >> 7 & 0x01;
			optionsCPU->flags->z = flags >> 6 & 0x01;
			optionsCPU->flags->ac = flags >> 4 & 0x01;
			optionsCPU->flags->p = flags >> 2 & 0x01;
			optionsCPU->flags->cy = flags & 0x01;

			optionsCPU->sp += 2;
			break;}
		case 0xf2:{
			optionsCPU->cycles = 10;
			if(!optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xf3:
			optionsCPU->cycles = 4;
			optionsCPU->ie = 0;
			break;
		case 0xf4:{
			optionsCPU->cycles = 11;
			if(!optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -=2;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xf5:{
			optionsCPU->cycles = 11;
			uint8_t flags = 0;

			flags |= optionsCPU->flags->cy ? 0x01 : 0;
			flags |= optionsCPU->flags->p ? 0x04 : 0;
			flags |= optionsCPU->flags->ac ? 0x10 : 0;
			flags |= optionsCPU->flags->z ? 0x40 : 0;
			flags |= optionsCPU->flags->s ? 0x80 : 0;
			flags |= 0x02;

			optionsCPU->memory[--optionsCPU->sp] = optionsCPU->a;
			optionsCPU->memory[--optionsCPU->sp] = flags;
			break;}
		case 0xf6:{
			optionsCPU->cycles = 7;
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a | data;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			optionsCPU->flags->ac = 0;
			break;}
		case 0xf7:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0030;
			return;
			break;}
		case 0xf8:{
			optionsCPU->cycles = 5;
			if(optionsCPU->flags->s)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				optionsCPU->cycles = 11;
				return;
			}
			break;}
		case 0xf9:{
			optionsCPU->cycles = 5;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->sp = HL;
			break;}
		case 0xfa:{
			optionsCPU->cycles = 10;
			if(optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xfb:
			optionsCPU->cycles = 4;
			optionsCPU->ie = 1;
			break;
		case 0xfc:{
			optionsCPU->cycles = 11;
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->s)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				optionsCPU->cycles = 17;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xfd:
			optionsCPU->cycles = 4;
			break;
		case 0xfe:{
			optionsCPU->cycles = 7;
			uint8_t v = optionsCPU->memory[++optionsCPU->pc];
			uint16_t res  = optionsCPU->a - v;
			uint8_t res8  = res & 0xFF;
			optionsCPU->flags->ac = ((optionsCPU->a ^ v ^ res8) & 0x10) != 0;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res8);
			optionsCPU->flags->cy = optionsCPU->a < v;
			break;}
		case 0xff:{
			optionsCPU->cycles = 11;
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0038;
			return;
			break;}
		default:
			UnimplementedInstruction(optionsCPU);
			break;
	};

	optionsCPU->pc++;
}
