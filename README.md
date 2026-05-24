# Intel8080 emulator for Space invaders

The emulator is designed to run the arcade game Space invaders on the intel 8080 processor

Programming language: C


![Space invaders demo](assets/demo4.gif)

## Build and Run on Linux
 
To build the executable file, use:
 
```bash
gcc main.c disassembler.c ProcessorInstructions.c testFunc.c -o start $(sdl2-config --cflags --libs)
./start
```

## Controls
 
Controls are implemented only for Player 1.
 
- `C` - Insert coin
- `Space` - Start
- `A` - Move left
- `D` - Move right
- `W` - Shoot

## How to Start
Insert a coin by pressing `C`, then press Start using `Space`.


## Debug
To enable debug mode, set 'isDebug = 1'


## Detailed implementation descripton
The project is divided into several modules: disassembler, ProcessorInstructions, testFunc, cpu, and main. Below is a detailed description of each module:

### disassembler
This module consists of a single function with the same name. It uses a `switch` statement to decode the received opcode into assembly instructions and returns the number of bytes occupied by the instruction. In this project, the disassembler is used for debugging in the `main` function.

### ProcessorInstructions
This module implements the processor instructions. In addition to the main function responsible for executing instructions, it also contains helper functions for setting flags and reporting unimplemented instructions. The main function of this module uses a `switch` statement to execute processor instructions. After each instruction, it also sets the number of CPU cycles taken by that instruction, which is needed for implementing interrupts and screen updates.

### TestFunc
This module contains a function responsible for testing the processor using the corresponding ROMs. The test ROMs are located in `roms/test_roms`.

### cpu
This module contains two structures that describe the processor state.
The `CPUState` structure contains the registers, stack pointer, program counter, and other processor-related data. It also includes the `cycles` field, which stores the number of CPU cycles taken by an instruction after it is executed.
The `Flags` structure contains the processor flags.

### main
This module contains the main program loop and the implementation of graphics and controls using the SDL library.

