# Educational Virtual Machine (VM)

A simple virtual machine designed for educational purposes, demonstrating the fundamental concepts of computer architecture, instruction sets, and virtual machine implementation.

## Overview

This project implements a basic virtual machine with:
- Custom instruction set architecture (ISA)
- CPU simulation with registers and stack
- Memory management system
- Firmware loading/saving capabilities
- Debug mode for educational purposes

## Features

- **CPU Simulation**: 16 general-purpose registers (R0-R15) with arithmetic and logic operations
- **Memory Management**: Configurable RAM size with stack support
- **Instruction Set**: Basic operations including MOV, ADD, PUSH, POP, HLT, and more
- **Addressing Modes**: Immediate, register, and memory addressing
- **Firmware Support**: Save and load programs as binary firmware files
- **Debug Mode**: Step-by-step execution with state visualization
- **Cross-platform**: Written in C++ for portability

## Project Structure

```
├── src/  
├── common/ 
│ └── types.h # Common type definitions 
├── cpu/ │ 
│├── cpu.h # CPU class definition
│ └── cpu.cpp # CPU implementation 
├── memory/
├── memory.h # Memory class definition 
│ └── memory.cpp # Memory implementation
│ └── vm/
├── vm.h # Virtual machine class definition 
├── vm.cpp # Virtual machine implementation 
├── firmware_loader.h # Firmware handling definition 
└── firmware_loader.cpp # Firmware handling implementation
├── main.cpp # Main application entry point
├── CMakeLists.txt # CMake build configuration
│└── README.md # This file
```

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)

### Build Instructions

1. Clone the repository: 
```
git clone repository
cd vm
```

2. Create a build directory:
```
mkdir cmake-build-debug 
cd cmake-build-debug
```

3. Configure and build :
````
cmake .. make
````

4. Run the virtual machine:
```
./vm
```
## Usage

The virtual machine supports several operating modes:
```
./vm
```

### Demo Mode (Default)
Runs a built-in test program demonstrating basic arithmetic operations:
```
./vm -d
```

### Firmware Mode
Load and execute a firmware file:

```
./vm -f firmware.vmfw
```


### Test Firmware Generation
Generate a test firmware file for experimentation:

```
./vm -t
```


### Help
Display usage information:
```
./vm -h
```


## Instruction Set Architecture

The virtual machine uses a 64-bit instruction format:

| Bits | Field | Description |
|------|-------|-------------|
| 63-56 | Opcode | Instruction operation code |
| 55-52 | Addressing Mode | How operands are addressed |
| 51-48 | Reg1 | First register operand |
| 47-44 | Reg2 | Second register operand |
| 43-0 | Immediate | Immediate value/address |

### Supported Instructions

- **MOV**: Move data between registers or load immediate values
- **ADD**: Addition operation
- **PUSH**: Push value onto stack
- **POP**: Pop value from stack
- **HLT**: Halt the virtual machine

### Addressing Modes

- **IMMEDIATE**: Use immediate value from instruction
- **REGISTER**: Use register content
- **MEMORY**: Use memory address (future enhancement)

## Example Program

Here's a simple program that adds two numbers:
```
// Load 42 into R0 MOV R0, #42
// Load 10 into R1
MOV R1, #10
// Add R1 to R0 (R0 = R0 + R1) ADD R0, R1
// Save result to stack PUSH R0
// Retrieve result into R2 POP R2
// Stop execution HLT
```


## Firmware Format

Firmware files use a binary format with the following structure:

### Header (80 bytes)
- **Signature** (4 bytes): "VMFW" magic number
- **Version** (4 bytes): Firmware format version
- **Instruction Count** (4 bytes): Number of instructions
- **Entry Point** (4 bytes): Starting address (reserved)
- **Description** (64 bytes): Human-readable description

### Instructions
- Variable-length section containing 64-bit instructions

## Educational Purposes

This virtual machine is designed to teach:

1. **Computer Architecture**: Understanding how CPUs, memory, and instruction sets work together
2. **Assembly Language**: Low-level programming concepts and instruction encoding
3. **Virtual Machine Design**: Implementation of emulation and interpretation
4. **Memory Management**: Stack operations and memory addressing
5. **System Programming**: Binary file formats and data serialization

## Development

### Adding New Instructions

1. Define the opcode in `types.h`
2. Implement instruction logic in `cpu.cpp`
3. Update the instruction decoder
4. Test with sample programs

### Extending Memory Management

The memory system can be extended to support:
- Memory-mapped I/O
- Virtual memory and paging
- Memory protection
- Cache simulation

## Contributing

This is an educational project. Contributions that enhance the learning experience are welcome:

- Additional instruction types
- Improved debugging features
- Better error handling
- Documentation improvements
- Example programs

## License

This project is intended for educational use. Please check the repository for specific license terms.

## Future Enhancements

Planned improvements include:
- [ ] More complex instruction set (multiplication, division, bitwise operations)
- [ ] Conditional jumps and branches
- [ ] Memory-mapped I/O simulation
- [ ] Interrupt handling
- [ ] Assembly language parser/compiler
- [ ] Graphical debugger interface
- [ ] Performance metrics and profiling

---

**Note**: This virtual machine is simplified for educational purposes and does not implement all features of real-world processors. It serves as a foundation for understanding computer architecture concepts.