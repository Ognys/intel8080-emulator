#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"

void UnimplementedInstruction(CPUstate *cs){
	printf("Error:  Unimplemented instruction $%02X", cs->memory[cs->pc]);
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


void Instructions(CPUstate *cs){

	uint8_t *opcode = &cs->memory[cs->pc];

	switch(*opcode){
		case 0x00:
			cs->cycles = 4;
			break;
		case 0x01:
			cs->cycles = 10;
			cs->b = opcode[2];
			cs->c = opcode[1];
			cs->pc += 2;
			break;
		case 0x02:{
			cs->cycles = 7;
			uint16_t adr = (cs->b << 8) | cs->c;
			cs->memory[adr] = cs->a;
			break;}
		case 0x03:{
			cs->cycles = 5;
			uint16_t BC = (cs->b << 8) | cs->c;
			BC++;
			cs->b = (BC >> 8) & 0xFF;
			cs->c = BC & 0xFF;
			break;}
		case 0x04:{
			cs->cycles = 5;
			cs->b += 1;

			cs->flags->ac = (cs->b & 0xf) == 0xf;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags,cs->b);
			break;}
		case 0x05:{
			cs->cycles = 5;
			cs->b -= 1;

			cs->flags->ac = (cs->b & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->b);
			break;}
		case 0x06:
			cs->cycles = 7;
			cs->b = cs->memory[(cs->pc)+ 1];
			cs->pc++;
			break;
		case 0x07:{
			cs->cycles = 4;
			cs->flags->cy = 0;
			uint8_t temp_byte = cs->a & 0b10000000;
			cs->a <<= 1;
			temp_byte >>= 7;
			cs->a = cs->a | temp_byte;
			if(temp_byte)
				cs->flags->cy =  1;
			break;}
		case 0x08:
			cs->cycles = 4;
			break;
		case 0x09:{
			cs->cycles = 10;
			uint16_t HL =(cs->h << 8) | cs->l;
			uint16_t BC = (cs->b << 8) | cs->c;
			uint32_t res_hl = HL + BC;

			cs->h = res_hl >> 8;
			cs->l = res_hl;

			cs->flags->cy = res_hl > 0xFFFF;
			break;}
		case 0x0a:{
			cs->cycles = 7;
			uint16_t adr = (cs->b << 8) | cs->c;
			cs->a = cs->memory[adr];
			break;}
		case 0x0b:{
			cs->cycles = 5;
			uint16_t BC = (cs->b << 8) | cs->c;
			BC -= 1;

			cs->b = BC >> 8;
			cs->c = BC;
			break;}
		case 0x0c:
			cs->cycles = 5;
			cs->c++;

			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = (cs->c & 0xf) == 0xf;
			FlagOptimizationZSP(cs->flags, cs->c);
			break;
		case 0x0d:
			cs->cycles = 5;
			cs->c--;

			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = (cs->c & 0x0F) == 0x0;
			FlagOptimizationZSP(cs->flags, cs->c);
			break;
		case 0x0e:
			cs->cycles = 7;
			cs->c = cs->memory[++cs->pc];
			break;
		case 0x0f:{
			cs->cycles = 4;
			cs->flags->cy = (cs->a & 1) == 1;
			uint8_t temp_byte = (cs->a << 7) & 0b10000000;
			cs->a = (cs->a >> 1) | temp_byte;
			break;}
		case 0x10:
			cs->cycles = 4;
			break;
		case 0x11:
			cs->cycles = 10;
			cs->e = cs->memory[++cs->pc];
			cs->d = cs->memory[++cs->pc];
			break;
		case 0x12:{
			cs->cycles = 7;
			uint16_t adr = (cs->d << 8) | cs->e;
			cs->memory[adr] = cs->a;
			break;}
		case 0x13:{
			cs->cycles = 5;
			uint16_t DE = (cs->d << 8) | cs->e;
			DE++;
			cs->d = DE >> 8;
			cs->e = DE;
			break;}
		case 0x14:
			cs->cycles = 5;
			cs->d++;

			cs->flags->ac = (cs->b & 0xf) == 0xf;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->d);
			break;
		case 0x15:
			cs->cycles = 5;
			cs->d--;

			cs->flags->ac = (cs->d & 0x0F) == 0x00;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->d);
			break;
		case 0x16:
			cs->cycles = 7;
			cs->d = cs->memory[++cs->pc];
			break;
		case 0x17:{
			cs->cycles = 4;
			uint8_t temp_byte = cs->flags->cy;
			cs->flags->cy = (cs->a >> 7) & 0x01;
			cs->a = (cs->a << 1) | temp_byte;
			break;}
		case 0x18:
			cs->cycles = 4;
			break;
		case 0x19:{
			cs->cycles = 10;
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t DE = (cs->d << 8) | cs->e;
			uint32_t res = HL + DE;

			cs->h = res >> 8;
			cs->l = res;

			cs->flags->cy = res > 0xFFFF;
			break;}
		case 0x1a:{
			cs->cycles = 7;
			uint16_t adr = (cs->d << 8) | cs->e;
			cs->a = cs->memory[adr];
			break;}
		case 0x1b:{
			cs->cycles = 5;
			uint16_t DE = (cs->d << 8) | cs->e;
			DE-=1;
			cs->d = DE >> 8;
			cs->e = DE;
			break;}
		case 0x1c:
			cs->cycles = 5;
			cs->e += 1;

			cs->flags->ac = (cs->e & 0xf) == 0xf;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->e);
			break;
		case 0x1d:
			cs->cycles = 5;
			cs->e -= 1;

			cs->flags->ac = (cs->e & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->e);
			break;
		case 0x1e:
			cs->cycles = 7;
			cs->e = cs->memory[++cs->pc];
			break;
		case 0x1f:{
			cs->cycles = 4;
			uint8_t temp_byte = cs->flags->cy << 7;
			cs->flags->cy = cs->a & 0x01;
			cs->a = (cs->a >> 1) | temp_byte;
			break;}
		case 0x20:
			cs->cycles = 4;
			break;
		case 0x21:
			cs->cycles = 10;
			cs->l = cs->memory[++cs->pc];
			cs->h = cs->memory[++cs->pc];
			break;
		case 0x22:{
			cs->cycles = 16;
			uint16_t pc = cs->pc;
			uint16_t adr = ((cs->memory[pc + 2]) << 8) | cs->memory[pc + 1];
			cs->memory[adr] = cs->l;
			cs->memory[adr + 1] = cs->h;
			cs->pc += 2;
			break;}
		case 0x23:{
			cs->cycles = 5;
			uint16_t HL = (cs->h << 8) | cs->l;
			HL += 1;
			cs->h = HL >> 8;
			cs->l = HL;
			break;}
		case 0x24:
			cs->cycles = 5;
			cs->h += 1;

			cs->flags->ac = (cs->h & 0xf) == 0xf;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->h);
			break;
		case 0x25:
			cs->cycles = 5;
			cs->h -= 1;
			cs->flags->ac = (cs->h & 0x0F) == 0x0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->h);
			break;
		case 0x26:{
			cs->cycles = 7;
			uint8_t nb = cs->memory[cs->pc + 1];
			cs->h = nb;
			cs->pc += 1;
			break;}
		case 0x27:{
			cs->cycles = 4;
			uint16_t temp_a = cs->a;
			uint8_t old_a = temp_a;
			uint8_t add = 0;

			if((temp_a & 0x0F) > 9 || cs->flags->ac)
			{
				add += 0x06;
				temp_a += 0x06;
			}

			if((temp_a >> 4) > 9 ||  cs->flags->cy)
			{
				temp_a += 0x60;
				cs->flags->cy = 1;
			}

			cs->a = temp_a;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((old_a & 0x0F) +(add & 0x0F)) > 0x0F;

			break;}
		case 0x28:
			cs->cycles = 4;
			break;
		case 0x29:{
			cs->cycles = 10;
			uint32_t HL =  (cs->h << 8) | cs->l;
			HL += HL;
			cs->h = HL >> 8;
			cs->l = HL;
			cs->flags->cy = HL > 0xFFFF;
			break;}
		case 0x2a:{
			cs->cycles = 16;
			uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			cs->l = cs->memory[adr];
			cs->h = cs->memory[adr + 1];
			cs->pc += 2;
			break;}
		case 0x2b:{
			cs->cycles = 5;
			uint16_t HL = (cs->h << 8) | cs->l;
			HL -= 1;
			cs->h = HL >> 8;
			cs->l = HL;
			break;}
		case 0x2c:
			cs->cycles = 5;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->l & 0x0F) + 1) > 0x0F;
			cs->l += 1;
			FlagOptimizationZSP(cs->flags, cs->l);
			break;
		case 0x2d:
			cs->cycles = 5;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = (cs->l & 0x0F) == 0x00;
			cs->l -= 1;
			FlagOptimizationZSP(cs->flags, cs->l);
			break;
		case 0x2e:
			cs->cycles = 7;
			cs->l =cs->memory[++cs->pc];
			break;
		case 0x2f:
			cs->cycles = 4;
			cs->a = ~cs->a;
			break;
		case 0x30:
			cs->cycles = 4;
			break;
		case 0x31:{
			cs->cycles = 10;
			uint8_t low = cs->memory[++cs->pc];
			uint8_t high = cs->memory[++cs->pc];

			cs->sp = (high << 8) | low;
			break;}
		case 0x32:{
			cs->cycles = 13;
			uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			cs->memory[adr] = cs->a;
			cs->pc += 2;
			break;}
		case 0x33:
			cs->cycles = 5;
			cs->sp += 1;
			break;
		case 0x34:{
			cs->cycles = 10;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t adr = (cs->h << 8) | cs->l;
			cs->flags->ac = ((cs->memory[adr] & 0x0F) + 1) > 0x0F;
			cs->memory[adr] += 1;;
			FlagOptimizationZSP(cs->flags, cs->memory[adr]);
			break;}
		case 0x35:{
			cs->cycles = 10;
			uint16_t adr = (cs->h << 8) | cs->l;
			cs->memory[adr] -= 1;
			cs->flags->ac = (cs->memory[adr] & 0x0F) == 0x00;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->memory[adr]);
			break;}
		case 0x36:{
			cs->cycles = 10;
			uint16_t adr = (cs->h << 8) | cs->l;
			cs->memory[adr] = cs->memory[++cs->pc];
			break;}
		case 0x37:
			cs->cycles = 4;
			cs->flags->cy = 1;
			break;
		case 0x38:
			cs->cycles = 4;
			break;
		case 0x39:{
			cs->cycles = 10;
			uint16_t HL = (cs->h << 8) | cs->l;
			uint32_t res = HL + cs->sp;
			cs->flags->cy = res > 0xFFFF;
			cs->h = res >> 8;
			cs->l = res;
			break;}
		case 0x3a:{
			cs->cycles = 13;
			uint16_t pc = cs->pc;
			uint16_t adr  = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			cs->a = cs->memory[adr];
			cs->pc += 2;
			break;}
		case 0x3b:
			cs->cycles = 5;
			cs->sp -= 1;
			break;
		case 0x3c:
			cs->cycles = 5;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a += 1;
			cs->flags->ac = (cs->a & 0x0F) == 0;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;
		case 0x3d:
			cs->cycles = 5;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = (cs->a & 0x0F) == 0x00;
			cs->a -= 1;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;
		case 0x3e:
			cs->cycles = 7;
			cs->a = cs->memory[++cs->pc];
			break;
		case 0x3f:
			cs->cycles = 4;
			cs->flags->cy = !cs->flags->cy;
			break;
		case 0x40:
			cs->cycles = 5;
			cs->b = cs->b;
			break;
		case 0x41:
			cs->cycles = 5;
			cs->b = cs->c;
			break;
		case 0x42:
			cs->cycles = 5;
			cs->b = cs->d;
			break;
		case 0x43:
			cs->cycles = 5;
			cs->b = cs->e;
			break;
		case 0x44:
			cs->cycles = 5;
			cs->b = cs->h;
			break;
		case 0x45:
			cs->cycles = 5;
			cs->b = cs->l;
			break;
		case 0x46:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->b = cs->memory[HL];
			break;}
		case 0x47:
			cs->cycles = 5;
			cs->b = cs->a;
			break;
		case 0x48:
			cs->cycles = 5;
			cs->c = cs->b;
			break;
		case 0x49:
			cs->cycles = 5;
			cs->c = cs->c;
			break;
		case 0x4a:
			cs->cycles = 5;
			cs->c = cs->d;
			break;
		case 0x4b:
			cs->cycles = 5;
			cs->c = cs->e;
			break;
		case 0x4c:
			cs->cycles = 5;
			cs->c = cs->h;
			break;
		case 0x4d:
			cs->cycles = 5;
			cs->c = cs->l;
			break;
		case 0x4e:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->c = cs->memory[HL];
			break;}
		case 0x4f:
			cs->cycles = 5;
			cs->c = cs->a;
			break;
		case 0x50:
			cs->cycles = 5;
			cs->d = cs->b;
			break;
		case 0x51:
			cs->cycles = 5;
			cs->d = cs->c;
			break;
		case 0x52:
			cs->cycles = 5;
			cs->d = cs->d;
			break;
		case 0x53:
			cs->cycles = 5;
			cs->d = cs->e;
			break;
		case 0x54:
			cs->cycles = 5;
			cs->d = cs->h;
			break;
		case 0x55:
			cs->cycles = 5;
			cs->d = cs->l;
			break;
		case 0x56:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->d = cs->memory[HL];
			break;}
		case 0x57:
			cs->cycles = 5;
			cs->d =cs->a;
			break;
		case 0x58:
			cs->cycles = 5;
			cs->e = cs->b;
			break;
		case 0x59:
			cs->cycles = 5;
			cs->e = cs->c;
			break;
		case 0x5a:
			cs->cycles = 5;
			cs->e = cs->d;
			break;
		case 0x5b:
			cs->cycles = 5;
			cs->e = cs->e;
			break;
		case 0x5c:
			cs->cycles = 5;
			cs->e = cs->h;
			break;
		case 0x5d:
			cs->cycles = 5;
			cs->e = cs->l;
			break;
		case 0x5e:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->e = cs->memory[HL];
			break;}
		case 0x5f:
			cs->cycles = 5;
			cs->e = cs->a;
			break;
		case 0x60:
			cs->cycles = 5;
			cs->h = cs->b;
			break;
		case 0x61:
			cs->cycles = 5;
			cs->h = cs->c;
			break;
		case 0x62:
			cs->cycles = 5;
			cs->h = cs->d;
			break;
		case 0x63:
			cs->cycles = 5;
			cs->h = cs->e;
			break;
		case 0x64:
			cs->cycles = 5;
			cs->h = cs->h;
			break;
		case 0x65:
			cs->cycles = 5;
			cs->h = cs->l;
			break;
		case 0x66:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->h = cs->memory[HL];
			break;}
		case 0x67:
			cs->cycles = 5;
			cs->h = cs->a;
			break;
		case 0x68:
			cs->cycles = 5;
			cs->l = cs->b;
			break;
		case 0x69:
			cs->cycles = 5;
			cs->l = cs->c;
			break;
		case 0x6a:
			cs->cycles = 5;
			cs->l = cs->d;
			break;
		case 0x6b:
			cs->cycles = 5;
			cs->l = cs->e;
			break;
		case 0x6c:
			cs->cycles = 5;
			cs->l = cs->h;
			break;
		case 0x6d:
			cs->cycles = 5;
			cs->l = cs->l;
			break;
		case 0x6e:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->l = cs->memory[HL];
			break;}
		case 0x6f:
			cs->cycles = 5;
			cs->l = cs->a;
			break;
		case 0x70:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->b;
			break;}
		case 0x71:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->c;
			break;}
		case 0x72:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->d;
			break;}
		case 0x73:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->e;
			break;}
		case 0x74:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->h;
			break;}
		case 0x75:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->l;
			break;}
		case 0x76:
			cs->cycles = 7;
			cs->hal = 1;
			break;
		case 0x77:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->memory[HL] = cs->a;
			break;}
		case 0x78:
			cs->cycles = 5;
			cs->a = cs->b;
			break;
		case 0x79:
			cs->cycles = 5;
			cs->a = cs->c;
			break;
		case 0x7a:
			cs->cycles = 5;
			cs->a = cs->d;
			break;
		case 0x7b:
			cs->cycles = 5;
			cs->a = cs->e;
			break;
		case 0x7c:
			cs->cycles = 5;
			cs->a = cs->h;
			break;
		case 0x7d:
			cs->cycles = 5;
			cs->a = cs->l;
			break;
		case 0x7e:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->a = cs->memory[HL];
			break;}
		case 0x7f:
			cs->cycles = 5;
			cs->a = cs->a;
			break;
		case 0x80:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->b;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->b & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x81:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->c;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->c & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x82:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->d;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->d & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x83:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->e;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->e & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x84:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->h;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->h & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x85:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->l;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->l & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x86:{
			cs->cycles = 7;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t res = cs->a + cs->memory[HL];
			cs->flags->ac = ((cs->a & 0x0F) + (cs->memory[HL] & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x87:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a + cs->a;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->a & 0x0F)) > 0x0F;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x88:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->b & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->b + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x89:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->c & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->c + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8a:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->d & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->d + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8b:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->e & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->e + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8c:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->h & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->h + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8d:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->l & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->l + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8e:{
			cs->cycles = 7;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t res = cs->a + cs->memory[HL] + cy;
			cs->flags->cy = res > 0xFF;
			cs->flags->ac = ((cs->a & 0x0F) + (cs->memory[HL] & 0x0F) + cy) > 0x0F;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x8f:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->ac = ((cs->a & 0x0F) + (cs->a & 0x0F) + cy) > 0x0F;
			uint16_t res = cs->a + cs->a + cy;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x90:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->b;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->b ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res8;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x91:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->c;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->c ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x92:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->d;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->d ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x93:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->e;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->e ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x94:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->h;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->h ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x95:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->l;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->l ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x96:{
			cs->cycles = 7;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t res = cs->a - cs->memory[HL];
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->memory[HL] ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x97:{
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->a;
			uint8_t res8 =res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->a ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x98:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->b - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->b ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x99:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->c - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->c ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9a:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->d - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->d ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9b:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->e - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->e ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9c:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->h - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->h ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9d:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->l - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->l ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9e:{
			cs->cycles = 7;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t res = cs->a - cs->memory[HL] - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->memory[HL] ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0x9f:{
			cs->cycles = 4;
			uint8_t cy = cs->flags->cy;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t res = cs->a - cs->a - cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->a ^ cy ^ res8) & 0x10) != 0;
			cs->flags->cy = res > 0xFF;
			cs->a  = res;
			FlagOptimizationZSP(cs->flags, cs->a);
			break;}
		case 0xa0:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->b;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->b) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa1:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->c;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->c) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa2:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->d;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->d) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa3:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->e;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->e) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa4:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->h;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->h) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa5:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->l;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->l) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa6:{
			cs->cycles = 7;
			ResetFlagsToZeroZSP(cs->flags);
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->a &= cs->memory[HL];
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->memory[HL]) & 0x08) != 0;
			cs->flags->cy = 0;
			break;}
		case 0xa7:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->a &= cs->a;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = ((cs->a | cs->a) & 0x08) != 0;
			cs->flags->cy = 0;
			break;
		case 0xa8:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->b;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xa9:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->c;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xaa:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->d;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xab:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->e;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xac:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->h;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xad:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->l;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xae:{
			cs->cycles = 7;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->a ^= cs->memory[HL];
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;}
		case 0xaf:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a ^= cs->a;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb0:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->b;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb1:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->c;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb2:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->d;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb3:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->e;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb4:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->h;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb5:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->l;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb6:{
			cs->cycles = 7;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->a |= cs->memory[HL];
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;}
		case 0xb7:
			cs->cycles = 4;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = 0;
			cs->a |= cs->a;
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->ac = 0;
			cs->flags->cy = 0;
			break;
		case 0xb8:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->b;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->b ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->b;
			break;}
		case 0xb9:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->c;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->c ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->c;
			break;}
		case 0xba:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->d;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->d ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->d;
			break;}
		case 0xbb:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->e;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->e ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->e;
			break;}
		case 0xbc:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->h;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->h ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->h;
			break;}
		case 0xbd:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->l;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->l ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->l;
			break;}
		case 0xbe:{
			cs->cycles = 7;
			uint16_t HL = (cs->h << 8) | cs->l;
			uint16_t res = cs->a - cs->memory[HL];
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->memory[HL] ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->memory[HL];
			break;}
		case 0xbf:{
			cs->cycles = 4;
			uint16_t res = cs->a - cs->a;
			uint16_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ cs->a ^ res8) &  0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = cs->a < cs->a;
			break;}
		case 0xc0:{
			cs->cycles = 5;
			if(!cs->flags->z)
			{
				uint16_t sp = cs->sp;
				uint8_t high = cs->memory[sp + 1];
				uint8_t low = cs->memory[sp];
				cs->pc = (high << 8) | low;
				cs->sp +=2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xc1:{
			cs->cycles = 10;
			uint16_t sp = cs->sp;
			cs->b = cs->memory[sp + 1];
			cs->c = cs->memory[sp];
			cs->sp += 2;
			break;}
		case 0xc2:{
			cs->cycles = 10;
			if(!cs->flags->z)
			{
				uint16_t adr = (cs->memory[cs->pc + 2] << 8) | cs->memory[cs->pc + 1];
				cs->pc = adr;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xc3:{
			cs->cycles = 10;
			uint16_t pc =  cs->pc;
			cs->pc = ((cs->memory[pc + 2]) << 8) | cs->memory[pc + 1];
			return;
			break;}
		case 0xc4:{
			cs->cycles = 11;
		 	uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];

			if(!cs->flags->z)
			{
				uint16_t ret = cs->pc + 3;
				cs->memory[--cs->sp] = ret >> 8;
				cs->memory[--cs->sp] = ret;
				cs->pc = adr;
				cs->cycles = 17;
				return;
			}
			else
				cs->pc += 2;
			break;}
		case 0xc5:
			cs->cycles = 11;
			cs->memory[cs->sp - 2] = cs->c;
			cs->memory[cs->sp - 1] = cs->b;
			cs->sp -= 2;
			break;
		case 0xc6:
			cs->cycles = 7; {
			uint16_t res = cs->a + cs->memory[cs->pc + 1];
			cs->flags->ac = ((cs->a & 0x0F) + (cs->memory[cs->pc +  1] & 0x0F)) > 0x0F;
			cs->a = res;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res);
			cs->flags->cy = res > 0xFF;
			cs->pc += 1;
			break;}
		case 0xc7:{
			cs->cycles = 11;
			uint16_t res = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = res >> 8; 
			cs->memory[sp - 2] = res;
			cs->sp-= 2;
			cs->pc = 0x0000;
			return;
			break;}
		case 0xc8:{
			cs->cycles = 5;
			if(cs->flags->z)
			{
				uint16_t sp = cs->sp;
				cs->pc = ((cs->memory[sp + 1]) << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xc9:{
			cs->cycles = 10;
			uint16_t sp = cs->sp;
			cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
			cs->sp += 2;
			return;
			break;}
		case 0xca:{
			cs->cycles = 10;	
			if(cs->flags->z)
			{
				uint16_t pc = cs->pc;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				return;
			}
			cs->pc += 2;
			break;}
		case 0xcb:
			cs->cycles = 4;
			break;
		case 0xcc:{
			cs->cycles = 11;
		 	uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			if(cs->flags->z)
			{
				uint16_t ret = cs->pc + 3;
				cs->memory[--cs->sp] = ret >> 8;
				cs->memory[--cs->sp] = ret;
				cs->pc = adr; 
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xcd:{
			cs->cycles = 17;
			uint16_t sp = cs->sp;
			uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			pc +=  3;
			cs->memory[sp - 1] = pc >> 8;
			cs->memory[sp - 2] = pc;
			cs->sp -=2;
			cs->pc = adr;
			return;
			break;}
		case 0xce:{
			cs->cycles = 7;
			uint8_t data = cs->memory[cs->pc + 1];
			uint16_t res= cs->a + data + cs->flags->cy;
			cs->flags->ac = ((cs->a & 0x0F) + (data & 0x0F) + cs->flags->cy) > 0x0F;
			cs->a = res;

			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->cy = res > 0xFF;
			cs->pc += 1;
			break;}
		case 0xcf:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0008;
			return;
			break;}
		case 0xd0:{
			cs->cycles = 5;
			if(!cs->flags->cy)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xd1:{
			cs->cycles = 10;
			uint16_t sp = cs->sp;
			cs->e = cs->memory[sp];
			cs->d = cs->memory[sp + 1];
			cs->sp += 2;
			break;}
		case 0xd2:{
			cs->cycles = 10;
			if(!cs->flags->cy)
			{
				uint16_t pc = cs->pc;
				uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->pc = adr;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xd3:{
			cs->cycles = 10;
			uint8_t port = cs->memory[cs->pc +1];
			switch(port)
			{
				case 2:
					cs->so = cs->a & 0x07;
					break;
				case 4:
					cs->sr = (cs->a << 8) | (cs->sr >> 8);
					break;
			}
			cs->pc += 1;
			break;}
		case 0xd4:{
			cs->cycles = 11;
			if(!cs->flags->cy)
			{
				uint16_t pc = cs->pc;
				uint16_t ret = cs->pc + 3;
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->memory[sp - 1] = ret >> 8;
				cs->memory[sp - 2] = ret;
				cs->sp -= 2;
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xd5:{
			cs->cycles = 11;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = cs->d;
			cs->memory[sp - 2] = cs->e;
			cs->sp -= 2;
			break;}
		case 0xd6:{
			cs->cycles = 7;
			uint8_t data =  cs->memory[++cs->pc];
			uint16_t res = cs->a - data;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ data ^ res8) & 0x10) != 0;
			cs->a = res;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->cy = res > 0xFF;
			break;}
		case 0xd7:{
			cs->cycles = 11;
			uint16_t sp = cs->sp;
			uint16_t pc = cs->pc + 1;
			cs->memory[sp - 1] = pc >> 8;
			cs->memory[sp - 2] = pc;
			cs->sp -= 2;
			cs->pc = 0x0010;
			return;
			break;}
		case 0xd8:{
			cs->cycles = 5;
			if(cs->flags->cy)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xd9:
			cs->cycles = 4;
			break;
		case 0xda:{
			cs->cycles = 10;
			if(cs->flags->cy)
			{
				uint16_t pc = cs->pc;
				uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->pc = adr;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xdb:{
			cs->cycles = 10;
			uint8_t port = cs->memory[cs->pc +1];
			switch(port)
			{
				case 3:
					cs->a = (cs->sr >> (8 - cs->so)) & 0xff;
					break;
				default:
					cs->a = cs->ports[port];
					break;
			}
			cs->pc += 1;
			break;}
		case 0xdc:{
			cs->cycles = 11;
			if(cs->flags->cy)
			{
				uint16_t pc = cs->pc;
				uint16_t ret = cs->pc + 3;
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->memory[sp - 1] = ret >> 8;
				cs->memory[sp - 2] = ret;
				cs->sp -= 2;
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xdd:
			cs->cycles = 4;
			break;
		case 0xde:{
			cs->cycles = 7;
			uint8_t data = cs->memory[cs->pc + 1];
			uint16_t res= cs->a - data - cs->flags->cy;
			uint8_t res8 = res & 0xFF;
			cs->flags->ac = ((cs->a ^ data ^ res8) & 0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			cs->flags->cy = cs->a < (data + cs->flags->cy);

			cs->a = res;

			FlagOptimizationZSP(cs->flags, cs->a);
			cs->pc += 1;
			break;}
		case 0xdf:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0018;
			return;
			break;}
		case 0xe0:{
			cs->cycles = 5;
			if(!cs->flags->p)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xe1:{
			cs->cycles = 10;
			uint16_t sp = cs->sp;
			cs->l = cs->memory[sp];
			cs->h = cs->memory[sp + 1];
			cs->sp += 2;
			break;}
		case 0xe2:{
			cs->cycles = 10;
			if(!cs->flags->p)
			{
				uint16_t pc = cs->pc;
				uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->pc = adr;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xe3:{
			cs->cycles = 18;
			uint16_t sp = cs->sp;
			uint8_t temp_sp = cs->memory[sp];
			uint8_t temp_sp_1 = cs->memory[sp + 1];

			cs->memory[sp] = cs->l;
			cs->memory[sp + 1] = cs->h;

			cs->l = temp_sp;
			cs->h = temp_sp_1;
			break;}
		case 0xe4:{
			cs->cycles = 11;
		 	uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			if(!cs->flags->p)
			{
				uint16_t ret = cs->pc + 3;
				cs->memory[--cs->sp] = ret >> 8;
				cs->memory[--cs->sp] = ret;
				cs->pc = adr; 
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xe5:{
			cs->cycles = 11;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = cs->h;
			cs->memory[sp - 2] = cs->l;
			cs->sp -= 2;
			break;}
		case 0xe6:{
			cs->cycles = 7;
			uint8_t data =  cs->memory[++cs->pc];
			uint16_t res = cs->a & data;
			cs->flags->ac = ((cs->a | data) & 0x08) != 0;
			cs->a = res;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->cy = 0;
			break;}
		case 0xe7:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0020;
			return;
			break;}
		case 0xe8:{
			cs->cycles = 5;
			if(cs->flags->p)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xe9:
			cs->cycles = 5;
			cs->pc = (cs->h << 8) | cs->l;
			return;
			break;
		case 0xea:{
			cs->cycles = 10;
			if(cs->flags->p)
			{
				uint16_t pc = cs->pc;
				uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->pc = adr;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xeb:{
			cs->cycles = 4;
			uint8_t temp_h = cs->h;
			uint8_t temp_l = cs->l;

			cs->h = cs->d;
			cs->l = cs->e;

			cs->d = temp_h;
			cs->e = temp_l;
			break;}
		case 0xec:{
			cs->cycles = 11;
		 	uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			if(cs->flags->p)
			{
				uint16_t ret = cs->pc + 3;
				cs->memory[--cs->sp] = ret >> 8;
				cs->memory[--cs->sp] = ret;
				cs->pc = adr; 
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xed:
			cs->cycles = 4;
			break;
		case 0xee:{
			cs->cycles = 7;
			uint8_t data =  cs->memory[++cs->pc];
			uint16_t res = cs->a ^ data;
			cs->a = res;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->cy = 0;
			cs->flags->ac = 0;
			break;}
		case 0xef:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0028;
			return;
			break;}
		case 0xf0:{
			cs->cycles = 5;
			if(!cs->flags->s)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xf1:{
			cs->cycles = 10;
			uint16_t sp = cs->sp;
			uint8_t flags =  cs->memory[sp];
			cs->a = cs->memory[sp + 1];

			cs->flags->s = flags >> 7 & 0x01;
			cs->flags->z = flags >> 6 & 0x01;
			cs->flags->ac = flags >> 4 & 0x01;
			cs->flags->p = flags >> 2 & 0x01;
			cs->flags->cy = flags & 0x01;

			cs->sp += 2;
			break;}
		case 0xf2:{
			cs->cycles = 10;
			if(!cs->flags->s)
			{
				uint16_t pc = cs->pc;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				return;
			}
			cs->pc += 2;
			break;}
		case 0xf3:
			cs->cycles = 4;
			cs->interrupt_enabled = 0;
			break;
		case 0xf4:{
			cs->cycles = 11;
			if(!cs->flags->s)
			{
				uint16_t pc = cs->pc;
				uint16_t ret = cs->pc + 3;
				uint16_t sp = cs->sp;
				cs->memory[sp - 1] = ret >> 8;
				cs->memory[sp - 2] = ret;
				cs->sp -=2;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xf5:{
			cs->cycles = 11;
			uint8_t flags = 0;

			flags |= cs->flags->cy ? 0x01 : 0;
			flags |= cs->flags->p ? 0x04 : 0;
			flags |= cs->flags->ac ? 0x10 : 0;
			flags |= cs->flags->z ? 0x40 : 0;
			flags |= cs->flags->s ? 0x80 : 0;
			flags |= 0x02;

			cs->memory[--cs->sp] = cs->a;
			cs->memory[--cs->sp] = flags;
			break;}
		case 0xf6:{
			cs->cycles = 7;
			uint8_t data =  cs->memory[++cs->pc];
			uint16_t res = cs->a | data;
			cs->a = res;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, cs->a);
			cs->flags->cy = 0;
			cs->flags->ac = 0;
			break;}
		case 0xf7:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0030;
			return;
			break;}
		case 0xf8:{
			cs->cycles = 5;
			if(cs->flags->s)
			{
				uint16_t sp = cs->sp;
				cs->pc = (cs->memory[sp + 1] << 8) | cs->memory[sp];
				cs->sp += 2;
				cs->cycles = 11;
				return;
			}
			break;}
		case 0xf9:{
			cs->cycles = 5;
			uint16_t HL = (cs->h << 8) | cs->l;
			cs->sp = HL;
			break;}
		case 0xfa:{
			cs->cycles = 10;
			if(cs->flags->s)
			{
				uint16_t pc = cs->pc;
				cs->pc = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
				return;
			}
			cs->pc += 2;
			break;}
		case 0xfb:
			cs->cycles = 4;
			cs->interrupt_enabled = 1;
			break;
		case 0xfc:{
			cs->cycles = 11;
		 	uint16_t pc = cs->pc;
			uint16_t adr = (cs->memory[pc + 2] << 8) | cs->memory[pc + 1];
			if(cs->flags->s)
			{
				uint16_t ret = cs->pc + 3;
				cs->memory[--cs->sp] = ret >> 8;
				cs->memory[--cs->sp] = ret;
				cs->pc = adr; 
				cs->cycles = 17;
				return;
			}
			cs->pc += 2;
			break;}
		case 0xfd:
			cs->cycles = 4;
			break;
		case 0xfe:{
			cs->cycles = 7;
			uint8_t v = cs->memory[++cs->pc];
			uint16_t res  = cs->a - v;
			uint8_t res8  = res & 0xFF;
			cs->flags->ac = ((cs->a ^ v ^ res8) & 0x10) != 0;
			ResetFlagsToZeroZSP(cs->flags);
			FlagOptimizationZSP(cs->flags, res8);
			cs->flags->cy = cs->a < v;
			break;}
		case 0xff:{
			cs->cycles = 11;
			uint16_t ret = cs->pc + 1;
			uint16_t sp = cs->sp;
			cs->memory[sp - 1] = ret >> 8;
			cs->memory[sp - 2] = ret;
			cs->sp -= 2;
			cs->pc = 0x0038;
			return;
			break;}
		default:
			UnimplementedInstruction(cs);
			break;
	};

	cs->pc++;
}
