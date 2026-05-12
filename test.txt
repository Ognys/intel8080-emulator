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
			break;
		case 0x01:
			optionsCPU->b = opcode[2];
			optionsCPU->c = opcode[1];
			optionsCPU->pc += 2;
			break;
		case 0x02:{
			uint16_t adr = (optionsCPU->b << 8) | optionsCPU->c;
			optionsCPU->memory[adr] = optionsCPU->a;
			break;}
		case 0x03:{
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			BC++;
			optionsCPU->b = (BC >> 8) & 0xFF;
			optionsCPU->c = BC & 0xFF;
			break;}
		case 0x04:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint8_t temp_b = optionsCPU->b;
			optionsCPU->b += 1;
			optionsCPU->flags->ac = ((temp_b & 0x0F) + 1) > 0x0F;
			FlagOptimizationZSP(optionsCPU->flags,optionsCPU->b);
			break;}
		case 0x05:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint8_t temp_b = optionsCPU->b;
			optionsCPU->b -= 1;
			optionsCPU->flags->ac = (temp_b & 0x0F) < 0x01;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->b);
			break;}
		case 0x06:
			optionsCPU->b = optionsCPU->memory[(optionsCPU->pc)+ 1];
			optionsCPU->pc++;
			break;
		case 0x07:{
			optionsCPU->flags->cy = 0;
			uint8_t temp_byte = optionsCPU->a & 0b10000000;
			optionsCPU->a <<= 1;
			temp_byte >>= 7;
			optionsCPU->a = optionsCPU->a | temp_byte;
			if(temp_byte)
				optionsCPU->flags->cy =  1;
			break;}
		case 0x08:
			break;
		case 0x09:{
			uint16_t HL =(optionsCPU->h << 8) | optionsCPU->l;
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			uint32_t res_hl = HL + BC;

			optionsCPU->h = res_hl >> 8;
			optionsCPU->l = res_hl;

			optionsCPU->flags->cy = res_hl > 0xFFFF;
			break;}
		case 0x0a:{
			uint16_t adr = (optionsCPU->b << 8) | optionsCPU->c;
			optionsCPU->a = optionsCPU->memory[adr];
			break;}
		case 0x0b:{
			uint16_t BC = (optionsCPU->b << 8) | optionsCPU->c;
			BC -= 1;

			optionsCPU->b = BC >> 8;
			optionsCPU->c = BC;
			break;}
		case 0x0c:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->c & 0x0F) + 1) > 0x0F;
			optionsCPU->c++;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->c);
			break;
		case 0x0d:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->c & 0x0F) < 0x01;
			optionsCPU->c--;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->c);
			break;
		case 0x0e:
			optionsCPU->c = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x0f:{
			optionsCPU->flags->cy = optionsCPU->a & 1;
			uint8_t temp_byte = (optionsCPU->a << 7) & 0b10000000;
			optionsCPU->a = (optionsCPU->a >> 1) | temp_byte;
			break;}
		case 0x10:
			break;
		case 0x11:
			optionsCPU->e = optionsCPU->memory[++optionsCPU->pc];
			optionsCPU->d = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x12:{
			uint16_t adr = (optionsCPU->d << 8) | optionsCPU->e;
			optionsCPU->memory[adr] = optionsCPU->a;
			break;}
		case 0x13:{
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			DE++;
			optionsCPU->d = DE >> 8;
			optionsCPU->e = DE;
			break;}
		case 0x14:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->d & 0x0F) + 1) > 0x0F;
			optionsCPU->d++;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->d);
			break;
		case 0x15:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->d & 0x0F) < 0x01;
			optionsCPU->d--;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->d);
			break;
		case 0x16:
			optionsCPU->d = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x17:{
			uint8_t temp_byte = optionsCPU->flags->cy;
			optionsCPU->flags->cy = (optionsCPU->a >> 7) & 0x01;
			optionsCPU->a = (optionsCPU->a << 1) | temp_byte;
			break;}
		case 0x18:
			break;
		case 0x19:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			uint32_t res = HL + DE;

			optionsCPU->h = res >> 8;
			optionsCPU->l = res;

			optionsCPU->flags->cy = res > 0xFFFF;
			break;}
		case 0x1a:{
			uint16_t adr = (optionsCPU->d << 8) | optionsCPU->e;
			optionsCPU->a = optionsCPU->memory[adr];
			break;}
		case 0x1b:{
			uint16_t DE = (optionsCPU->d << 8) | optionsCPU->e;
			DE-=1;
			optionsCPU->d = DE >> 8;
			optionsCPU->e = DE;
			break;}
		case 0x1c:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->e & 0x0F) + 1) > 0x0F;
			optionsCPU->e += 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->e);
			break;
		case 0x1d:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->e & 0x0F) < 0x01;
			optionsCPU->e -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->e);
			break;
		case 0x1e:
			optionsCPU->e = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x1f:{
			uint8_t temp_byte = optionsCPU->flags->cy << 7;
			optionsCPU->flags->cy = optionsCPU->a & 0x01;
			optionsCPU->a = (optionsCPU->a >> 1) | temp_byte;
			break;}
		case 0x20:
			break;
		case 0x21:
			optionsCPU->l = optionsCPU->memory[++optionsCPU->pc];
			optionsCPU->h = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x22:{
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = ((optionsCPU->memory[pc + 2]) << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->memory[adr] = optionsCPU->l;
			optionsCPU->memory[adr + 1] = optionsCPU->h;
			optionsCPU->pc += 2;
			break;}
		case 0x23:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			HL += 1;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			break;}
		case 0x24:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->h & 0x0F) + 1) > 0x0F;
			optionsCPU->h += 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->h);
			break;
		case 0x25:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->h & 0x0F) < 0x01;
			optionsCPU->h -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->h);
			break;
		case 0x26:{
			uint8_t nb = optionsCPU->memory[optionsCPU->pc + 1];
			optionsCPU->h = nb;
			optionsCPU->pc += 1;
			break;}
		case 0x27:{
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
			break;
		case 0x29:{
			uint32_t HL =  (optionsCPU->h << 8) | optionsCPU->l;
			HL += HL;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			optionsCPU->flags->cy = HL > 0xFFFF;
			break;}
		case 0x2a:{
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->l = optionsCPU->memory[adr];
			optionsCPU->h = optionsCPU->memory[adr + 1];
			optionsCPU->pc += 2;
			break;}
		case 0x2b:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			HL -= 1;
			optionsCPU->h = HL >> 8;
			optionsCPU->l = HL;
			break;}
		case 0x2c:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->l & 0x0F) + 1) > 0x0F;
			optionsCPU->l += 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->l);
			break;
		case 0x2d:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->l & 0x0F) < 0x01;
			optionsCPU->l -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->l);
			break;
		case 0x2e:
			optionsCPU->l =optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x2f:
			optionsCPU->a = ~optionsCPU->a;
			break;
		case 0x30:
			break;
		case 0x31:{
			uint8_t low = optionsCPU->memory[++optionsCPU->pc];
			uint8_t high = optionsCPU->memory[++optionsCPU->pc];

			optionsCPU->sp = (high << 8) | low;
			break;}
		case 0x32:{
			uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->memory[adr] = optionsCPU->a;
			optionsCPU->pc += 2;
			break;}
		case 0x33:
			optionsCPU->sp += 1;
			break;
		case 0x34:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->flags->ac = ((optionsCPU->memory[adr] & 0x0F) + 1) > 0x0F;
			optionsCPU->memory[adr] += 1;;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->memory[adr]);
			break;}
		case 0x35:{
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->flags->ac = (optionsCPU->memory[adr] & 0x0F) < 0x01;
			optionsCPU->memory[adr] -= 1;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->memory[adr]);
			break;}
		case 0x36:{
			uint16_t adr = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[adr] = optionsCPU->memory[++optionsCPU->pc];
			break;}
		case 0x37:
			optionsCPU->flags->cy = 1;
			break;
		case 0x38:
			break;
		case 0x39:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint32_t res = HL + optionsCPU->sp;
			optionsCPU->flags->cy = res > 0xFFFF;
			optionsCPU->h = res >> 8;
			optionsCPU->l = res;
			break;}
		case 0x3a:{
			uint16_t pc = optionsCPU->pc;
			uint16_t adr  = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			optionsCPU->a = optionsCPU->memory[adr];
			optionsCPU->pc += 2;
			break;}
		case 0x3b:
			optionsCPU->sp -= 1;
			break;
		case 0x3c:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + 1) > 0x0F;
			optionsCPU->a += 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;
		case 0x3d:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < 0x01;
			optionsCPU->a -= 1;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;
		case 0x3e:
			optionsCPU->a = optionsCPU->memory[++optionsCPU->pc];
			break;
		case 0x3f:
			optionsCPU->flags->cy = !optionsCPU->flags->cy;
			break;
		case 0x40:
			optionsCPU->b = optionsCPU->b;
			break;
		case 0x41:
			optionsCPU->b = optionsCPU->c;
			break;
		case 0x42:
			optionsCPU->b = optionsCPU->d;
			break;
		case 0x43:
			optionsCPU->b = optionsCPU->e;
			break;
		case 0x44:
			optionsCPU->b = optionsCPU->h;
			break;
		case 0x45:
			optionsCPU->b = optionsCPU->l;
			break;
		case 0x46:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->b = optionsCPU->memory[HL];
			break;}
		case 0x47:
			optionsCPU->b = optionsCPU->a;
			break;
		case 0x48:
			optionsCPU->c = optionsCPU->b;
			break;
		case 0x49:
			optionsCPU->c = optionsCPU->c;
			break;
		case 0x4a:
			optionsCPU->c = optionsCPU->d;
			break;
		case 0x4b:
			optionsCPU->c = optionsCPU->e;
			break;
		case 0x4c:
			optionsCPU->c = optionsCPU->h;
			break;
		case 0x4d:
			optionsCPU->c = optionsCPU->l;
			break;
		case 0x4e:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->c = optionsCPU->memory[HL];
			break;}
		case 0x4f:
			optionsCPU->c = optionsCPU->a;
			break;
		case 0x50:
			optionsCPU->d = optionsCPU->b;
			break;
		case 0x51:
			optionsCPU->d = optionsCPU->c;
			break;
		case 0x52:
			optionsCPU->d = optionsCPU->d;
			break;
		case 0x53:
			optionsCPU->d = optionsCPU->e;
			break;
		case 0x54:
			optionsCPU->d = optionsCPU->h;
			break;
		case 0x55:
			optionsCPU->d = optionsCPU->l;
			break;
		case 0x56:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->d = optionsCPU->memory[HL];
			break;}
		case 0x57:
			optionsCPU->d =optionsCPU->a;
			break;
		case 0x58:
			optionsCPU->e = optionsCPU->b;
			break;
		case 0x59:
			optionsCPU->e = optionsCPU->c;
			break;
		case 0x5a:
			optionsCPU->e = optionsCPU->d;
			break;
		case 0x5b:
			optionsCPU->e = optionsCPU->e;
			break;
		case 0x5c:
			optionsCPU->e = optionsCPU->h;
			break;
		case 0x5d:
			optionsCPU->e = optionsCPU->l;
			break;
		case 0x5e:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->e = optionsCPU->memory[HL];
			break;}
		case 0x5f:
			optionsCPU->e = optionsCPU->a;
			break;
		case 0x60:
			optionsCPU->h = optionsCPU->b;
			break;
		case 0x61:
			optionsCPU->h = optionsCPU->c;
			break;
		case 0x62:
			optionsCPU->h = optionsCPU->d;
			break;
		case 0x63:
			optionsCPU->h = optionsCPU->e;
			break;
		case 0x64:
			optionsCPU->h = optionsCPU->h;
			break;
		case 0x65:
			optionsCPU->h = optionsCPU->l;
			break;
		case 0x66:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->h = optionsCPU->memory[HL];
			break;}
		case 0x67:
			optionsCPU->h = optionsCPU->a;
			break;
		case 0x68:
			optionsCPU->l = optionsCPU->b;
			break;
		case 0x69:
			optionsCPU->l = optionsCPU->c;
			break;
		case 0x6a:
			optionsCPU->l = optionsCPU->d;
			break;
		case 0x6b:
			optionsCPU->l = optionsCPU->e;
			break;
		case 0x6c:
			optionsCPU->l = optionsCPU->h;
			break;
		case 0x6d:
			optionsCPU->l = optionsCPU->l;
			break;
		case 0x6e:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->l = optionsCPU->memory[HL];
			break;}
		case 0x6f:
			optionsCPU->l = optionsCPU->a;
			break;
		case 0x70:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->b;
			break;}
		case 0x71:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->c;
			break;}
		case 0x72:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->d;
			break;}
		case 0x73:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->e;
			break;}
		case 0x74:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->h;
			break;}
		case 0x75:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->l;
			break;}
		case 0x76:
			optionsCPU->hal = 1;
			break;
		case 0x77:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->memory[HL] = optionsCPU->a;
			break;}
		case 0x78:
			optionsCPU->a = optionsCPU->b;
			break;
		case 0x79:
			optionsCPU->a = optionsCPU->c;
			break;
		case 0x7a:
			optionsCPU->a = optionsCPU->d;
			break;
		case 0x7b:
			optionsCPU->a = optionsCPU->e;
			break;
		case 0x7c:
			optionsCPU->a = optionsCPU->h;
			break;
		case 0x7d:
			optionsCPU->a = optionsCPU->l;
			break;
		case 0x7e:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a = optionsCPU->memory[HL];
			break;}
		case 0x7f:
			optionsCPU->a = optionsCPU->a;
			break;
		case 0x80:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->b;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->b & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x81:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->c;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->c & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x82:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->d;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->d & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x83:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->e;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->e & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x84:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->h;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->h & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x85:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->l;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->l & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x86:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a + optionsCPU->memory[HL];
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->memory[HL] & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x87:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t res = optionsCPU->a + optionsCPU->a;
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->a & 0x0F)) > 0x0F;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x88:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->b & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->b + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x89:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->c & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->c + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8a:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->d & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->d + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8b:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->e & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->e + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8c:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->h & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->h + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8d:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->l & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->l + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x8e:{
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
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->a & 0x0F) + cy) > 0x0F;
			uint16_t res = optionsCPU->a + optionsCPU->a + cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x90:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->b & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->b;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x91:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->c & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->c;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x92:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->d & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->d;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x93:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->e & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->e;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x94:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->h & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->h;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x95:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->l & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->l;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x96:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->memory[HL] & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL];
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x97:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->a & 0x0F);
			uint16_t res = optionsCPU->a - optionsCPU->a;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x98:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->b & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->b - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x99:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->c & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->c - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9a:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->d & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->d - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9b:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->e & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->e - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9c:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->h & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->h - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9d:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->l & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->l - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9e:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->memory[HL] & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL] - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0x9f:{
			uint8_t cy = optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < ((optionsCPU->a & 0x0F) + cy);
			uint16_t res = optionsCPU->a - optionsCPU->a - cy;
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->a  = res;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			break;}
		case 0xa0:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa1:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa2:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa3:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa4:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa5:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa6:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a &= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xa7:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a &= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 1;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa8:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xa9:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xaa:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xab:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xac:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xad:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xae:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a ^= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xaf:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a ^= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb0:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->b;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb1:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->c;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb2:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->d;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb3:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->e;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb4:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->h;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb5:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->l;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb6:{
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->a |= optionsCPU->memory[HL];
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;}
		case 0xb7:
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->cy = 0;
			optionsCPU->a |= optionsCPU->a;
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->ac = 0;
			optionsCPU->flags->cy = 0;
			break;
		case 0xb8:{
			uint16_t res = optionsCPU->a - optionsCPU->b;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->b & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->b;
			break;}
		case 0xb9:{
			uint16_t res = optionsCPU->a - optionsCPU->c;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->c & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->c;
			break;}
		case 0xba:{
			uint16_t res = optionsCPU->a - optionsCPU->d;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->d & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->d;
			break;}
		case 0xbb:{
			uint16_t res = optionsCPU->a - optionsCPU->e;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->e & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->e;
			break;}
		case 0xbc:{
			uint16_t res = optionsCPU->a - optionsCPU->h;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->h & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->h;
			break;}
		case 0xbd:{
			uint16_t res = optionsCPU->a - optionsCPU->l;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->l & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->l;
			break;}
		case 0xbe:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			uint16_t res = optionsCPU->a - optionsCPU->memory[HL];
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->memory[HL] & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->memory[HL];
			break;}
		case 0xbf:{
			uint16_t res = optionsCPU->a - optionsCPU->a;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (optionsCPU->a & 0x0F);
			optionsCPU->flags->cy = optionsCPU->a < optionsCPU->a;
			break;}
		case 0xc0:{
			if(!optionsCPU->flags->z)
			{
				uint16_t sp = optionsCPU->sp;
				uint8_t high = optionsCPU->memory[sp + 1];
				uint8_t low = optionsCPU->memory[sp];
				optionsCPU->pc = (high << 8) | low;
				optionsCPU->sp +=2;
				return;
			}
			break;}
		case 0xc1:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->b = optionsCPU->memory[sp + 1];
			optionsCPU->c = optionsCPU->memory[sp];
			optionsCPU->sp += 2;
			break;}
		case 0xc2:{
			if(!optionsCPU->flags->z)
			{
				uint16_t adr = (optionsCPU->memory[optionsCPU->pc + 2] << 8) | optionsCPU->memory[optionsCPU->pc + 1];
				optionsCPU->pc = adr;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xc3:{
			uint16_t pc =  optionsCPU->pc;
			optionsCPU->pc = ((optionsCPU->memory[pc + 2]) << 8) | optionsCPU->memory[pc + 1];
			return;
			break;}
		case 0xc4:{
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];

			if(!optionsCPU->flags->z)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr;
				return;
			}
			else
				optionsCPU->pc += 2;
			break;}
		case 0xc5:
			optionsCPU->memory[optionsCPU->sp - 2] = optionsCPU->c;
			optionsCPU->memory[optionsCPU->sp - 1] = optionsCPU->b;
			optionsCPU->sp -= 2;
			break;
		case 0xc6: {
			uint16_t res = optionsCPU->a + optionsCPU->memory[optionsCPU->pc + 1];
			optionsCPU->flags->ac = ((optionsCPU->a & 0x0F) + (optionsCPU->memory[optionsCPU->pc +  1] & 0x0F)) > 0x0F;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res);
			optionsCPU->flags->cy = res > 0xFF;
			optionsCPU->pc += 1;
			break;}
		case 0xc7:{
			uint16_t res = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = res >> 8; 
			optionsCPU->memory[sp - 2] = res;
			optionsCPU->sp-= 2;
			optionsCPU->pc = 0x0000;
			return;
			break;}
		case 0xc8:{
			if(optionsCPU->flags->z)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = ((optionsCPU->memory[sp + 1]) << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xc9:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
			optionsCPU->sp += 2;
			return;
			break;}
		case 0xca:{	
			if(optionsCPU->flags->z)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xcb:
			break;
		case 0xcc:{
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->z)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xcd:{
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
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0008;
			return;
			break;}
		case 0xd0:{
			if(!optionsCPU->flags->cy)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xd1:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->e = optionsCPU->memory[sp];
			optionsCPU->d = optionsCPU->memory[sp + 1];
			optionsCPU->sp += 2;
			break;}
		case 0xd2:{
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
			uint8_t port = optionsCPU->memory[optionsCPU->pc +1];
			optionsCPU->ports[port] = optionsCPU->a;
			optionsCPU->pc += 1;
			break;}
		case 0xd4:{
			if(!optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -= 2;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xd5:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = optionsCPU->d;
			optionsCPU->memory[sp - 2] = optionsCPU->e;
			optionsCPU->sp -= 2;
			break;}
		case 0xd6:{
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a - data;
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (data & 0x0F);
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = res > 0xFF;
			break;}
		case 0xd7:{
			uint16_t sp = optionsCPU->sp;
			uint16_t pc = optionsCPU->pc + 1;
			optionsCPU->memory[sp - 1] = pc >> 8;
			optionsCPU->memory[sp - 2] = pc;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0010;
			return;
			break;}
		case 0xd8:{
			if(optionsCPU->flags->cy)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xd9:
			break;
		case 0xda:{
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
			uint8_t port = optionsCPU->memory[optionsCPU->pc +1];
			optionsCPU->a = optionsCPU->ports[port];
			optionsCPU->pc += 1;
			break;}
		case 0xdc:{
			if(optionsCPU->flags->cy)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -= 2;
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xdd:
			break;
		case 0xde:{
			uint8_t data = optionsCPU->memory[optionsCPU->pc + 1];
			uint16_t res= optionsCPU->a - data - optionsCPU->flags->cy;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			optionsCPU->flags->ac = (optionsCPU->a &  0x0F) < ((data & 0x0F) + optionsCPU->flags->cy);
			optionsCPU->flags->cy = optionsCPU->a < (data + optionsCPU->flags->cy);

			optionsCPU->a = res;

			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->pc += 1;
			break;}
		case 0xdf:{
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0018;
			return;
			break;}
		case 0xe0:{
			if(!optionsCPU->flags->p)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xe1:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->l = optionsCPU->memory[sp];
			optionsCPU->h = optionsCPU->memory[sp + 1];
			optionsCPU->sp += 2;
			break;}
		case 0xe2:{
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
			uint16_t sp = optionsCPU->sp;
			uint8_t temp_sp = optionsCPU->memory[sp];
			uint8_t temp_sp_1 = optionsCPU->memory[sp + 1];

			optionsCPU->memory[sp] = optionsCPU->l;
			optionsCPU->memory[sp + 1] = optionsCPU->h;

			optionsCPU->l = temp_sp;
			optionsCPU->h = temp_sp_1;
			break;}
		case 0xe4:{
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(!optionsCPU->flags->p)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xe5:{
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = optionsCPU->h;
			optionsCPU->memory[sp - 2] = optionsCPU->l;
			optionsCPU->sp -= 2;
			break;}
		case 0xe6:{
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a & data;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			optionsCPU->flags->ac = 1;
			break;}
		case 0xe7:{
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0020;
			return;
			break;}
		case 0xe8:{
			if(optionsCPU->flags->p)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xe9:
			optionsCPU->pc = (optionsCPU->h << 8) | optionsCPU->l;
			return;
			break;
		case 0xea:{
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
			uint8_t temp_h = optionsCPU->h;
			uint8_t temp_l = optionsCPU->l;

			optionsCPU->h = optionsCPU->d;
			optionsCPU->l = optionsCPU->e;

			optionsCPU->d = temp_h;
			optionsCPU->e = temp_l;
			break;}
		case 0xec:{
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->p)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xed:
			break;
		case 0xee:{
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a ^ data;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			optionsCPU->flags->ac = 0;
			break;}
		case 0xef:{
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0028;
			return;
			break;}
		case 0xf0:{
			if(!optionsCPU->flags->s)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xf1:{
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
			if(!optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xf3:
			optionsCPU->ie = 0;
			break;
		case 0xf4:{
			if(!optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				uint16_t ret = optionsCPU->pc + 3;
				uint16_t sp = optionsCPU->sp;
				optionsCPU->memory[sp - 1] = ret >> 8;
				optionsCPU->memory[sp - 2] = ret;
				optionsCPU->sp -=2;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xf5:{
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
			uint8_t data =  optionsCPU->memory[++optionsCPU->pc];
			uint16_t res = optionsCPU->a | data;
			optionsCPU->a = res;
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, optionsCPU->a);
			optionsCPU->flags->cy = 0;
			optionsCPU->flags->ac = 0;
			break;}
		case 0xf7:{
			uint16_t ret = optionsCPU->pc + 1;
			uint16_t sp = optionsCPU->sp;
			optionsCPU->memory[sp - 1] = ret >> 8;
			optionsCPU->memory[sp - 2] = ret;
			optionsCPU->sp -= 2;
			optionsCPU->pc = 0x0030;
			return;
			break;}
		case 0xf8:{
			if(optionsCPU->flags->s)
			{
				uint16_t sp = optionsCPU->sp;
				optionsCPU->pc = (optionsCPU->memory[sp + 1] << 8) | optionsCPU->memory[sp];
				optionsCPU->sp += 2;
				return;
			}
			break;}
		case 0xf9:{
			uint16_t HL = (optionsCPU->h << 8) | optionsCPU->l;
			optionsCPU->sp = HL;
			break;}
		case 0xfa:{
			if(optionsCPU->flags->s)
			{
				uint16_t pc = optionsCPU->pc;
				optionsCPU->pc = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xfb:
			optionsCPU->ie = 1;
			break;
		case 0xfc:{
		 	uint16_t pc = optionsCPU->pc;
			uint16_t adr = (optionsCPU->memory[pc + 2] << 8) | optionsCPU->memory[pc + 1];
			if(optionsCPU->flags->s)
			{
				uint16_t ret = optionsCPU->pc + 3;
				optionsCPU->memory[--optionsCPU->sp] = ret >> 8;
				optionsCPU->memory[--optionsCPU->sp] = ret;
				optionsCPU->pc = adr; 
				return;
			}
			optionsCPU->pc += 2;
			break;}
		case 0xfd:
			break;
		case 0xfe:{
			uint8_t v = optionsCPU->memory[++optionsCPU->pc];
			uint16_t res  = optionsCPU->a - v;
			uint8_t res8  = optionsCPU->a - v;
			optionsCPU->flags->ac = (optionsCPU->a & 0x0F) < (v & 0x0F);
			ResetFlagsToZeroZSP(optionsCPU->flags);
			FlagOptimizationZSP(optionsCPU->flags, res8);
			optionsCPU->flags->cy = optionsCPU->a < v;
			break;}
		case 0xff:{
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
