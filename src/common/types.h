// src/common/types.h
#ifndef VM_TYPES_H
#define VM_TYPES_H

#include <cstdint>
#include <array>

namespace vm {
    // Taille des registres
    constexpr size_t REGISTER_COUNT = 16;
    constexpr size_t REGISTER_SIZE = 8; // 64 bits

    // Types de drapeaux CPU
    enum class FlagType : uint8_t {
        ZERO = 0,
        CARRY = 1,
        NEGATIVE = 2,
        OF = 3,
        INTERRUPT = 4
    };

    // Types d'accès mémoire
    enum class AccessType : uint8_t {
        READ = 1,
        WRITE = 2,
        EXECUTE = 4
    };

    // Opcodes des instructions
    enum class Opcode : uint8_t {
        // Instructions de données
        MOV = 0x01,
        LOAD = 0x02,
        STORE = 0x03,
        PUSH = 0x04,
        POP = 0x05,

        // Instructions arithmétiques
        ADD = 0x10,
        SUB = 0x11,
        MUL = 0x12,
        DIV = 0x13,
        MOD = 0x14,
        INC = 0x15,
        DEC = 0x16,
        CMP = 0x17,

        // Instructions logiques
        AND = 0x20,
        OR = 0x21,
        XOR = 0x22,
        NOT = 0x23,
        SHL = 0x24,
        SHR = 0x25,

        // Instructions de contrôle
        JMP = 0x30,
        JZ = 0x31,
        JNZ = 0x32,
        JC = 0x33,
        JNC = 0x34,
        CALL = 0x35,
        RET = 0x36,
        NOP = 0x37,

        // Instructions système
        INT = 0x40,
        HLT = 0x41,
        IN = 0x42,
        OUT = 0x43
    };

    // Mode d'adressage
    enum class AddressingMode : uint8_t {
        REGISTER = 0,
        IMMEDIATE = 1,
        MEMORY = 2,
        REGISTER_INDIRECT = 3
    };

    // Structure d'une instruction
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