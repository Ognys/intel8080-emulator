#ifndef CPU_H
#define CPU_H

#include <stdint.h>

	struct flags{
		uint8_t z:1;
		uint8_t s:1;
		uint8_t p:1;
		uint8_t cy:1;
		uint8_t ac:1;
		uint8_t pad:3;
	};

	struct options{
		uint8_t b;
		uint8_t c;
		uint8_t d;
		uint8_t e;
		uint8_t h;
		uint8_t l;
		uint8_t m;
		uint8_t a;
		uint16_t pc;
		uint16_t sp;
		uint8_t *memory;
		uint8_t  *ports;
		struct flags *flags;
		uint8_t ie;
		uint8_t  hal;
		int cycles;
		uint16_t so;
		uint16_t sr;
	};

	typedef struct options options;
	typedef struct flags flags;

#endif
