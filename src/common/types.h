//
// Created by Jean-Michel Frouin on 17/08/2025.
//

#ifndef VM_TYPES_H
#define VM_TYPES_H

#include <cstdint>
#include <array>

namespace vm {
    // Register sizes
    constexpr size_t REGISTER_COUNT = 16;
    constexpr size_t REGISTER_SIZE = 8; // 64 bits

    // CPU flag types
    enum class FlagType : uint8_t {
        ZERO = 0,
        CARRY = 1,
        NEGATIVE = 2,
        OF = 3,
        INTERRUPT = 4
    };

    // Memory access types
    enum class AccessType : uint8_t {
        READ = 1,
        WRITE = 2,
        EXECUTE = 4
    };

    // Instruction opcodes
    enum class Opcode : uint8_t {
        // Data instructions
        MOV = 0x01,
        LOAD = 0x02,
        STORE = 0x03,
        PUSH = 0x04,
        POP = 0x05,
        HLT = 0x06,

        // Arithmetic instructions
        ADD = 0x10,
        SUB = 0x11,
        MUL = 0x12,
        DIV = 0x13,
        MOD = 0x14,
        INC = 0x15,
        DEC = 0x16,
        CMP = 0x17,
        SWAP = 0x18, //Swap 2 registers

        // Logical instructions
        AND = 0x20,
        OR = 0x21,
        XOR = 0x22,
        NOT = 0x23,
        SHL = 0x24,
        SHR = 0x25,

        // Control instructions
        JMP = 0x30,
        JZ = 0x31,
        JNZ = 0x32,
        JEQ = 0x33,    // Jump if Equal (alias for JZ)
        JNE = 0x34,    // Jump if Not Equal (alias for JNZ)
        JC = 0x35,
        JNC = 0x36,
        CALL = 0x37,
        RET = 0x38,
        NOP = 0x39,

        JL   = 0x3A,  // Jump if Less
        JLE  = 0x3B,  // Jump if Less or Equal
        JG   = 0x3C,  // Jump if Greater
        JGE  = 0x3D,  // Jump if Greater or Equal
        LOOP = 0x3E,

        PRINT = 0x44,  // Afficher une valeur

    };

    // Addressing mode
    enum class AddressingMode : uint8_t {
        REGISTER = 0,
        IMMEDIATE = 1,
        MEMORY = 2,
        REGISTER_INDIRECT = 3
    };

    // Instruction structure
    struct Instruction {
        Opcode opcode;
        AddressingMode mode;
        uint8_t reg1;
        uint8_t reg2;
        uint32_t immediate;

        Instruction() : opcode(Opcode::NOP), mode(AddressingMode::REGISTER),
                       reg1(0), reg2(0), immediate(0) {}
    };
}

#endif // VM_TYPES_H