//
// Created by Jean-Michel Frouin on 17/08/2025.
//
// src/cpu/cpu.cpp
#include "cpu.h"
#include <iostream>
#include <iomanip>

namespace vm {
    CPU::CPU(Memory* mem) : memory(mem), running(false), debug_mode(false) {
        reset();
    }

    void CPU::reset() {
        registers.fill(0);
        pc = 0;
        sp = 0xFFFFF0; // Début de la pile en haut de la mémoire
        flags = 0;
        running = false;
    }

    void CPU::step() {
        if (!running) return;

        Instruction instr;
        fetchInstruction(instr);

        if (debug_mode) {
            std::cout << "PC: 0x" << std::hex << pc << " - ";
            std::cout << "Opcode: 0x" << static_cast<int>(instr.opcode) << std::endl;
        }

        executeInstruction(instr);
    }

    void CPU::run() {
        running = true;
        while (running) {
            step();
        }
    }

    void CPU::fetchInstruction(Instruction& instr) {
        // Lecture de l'instruction depuis la mémoire
        uint64_t raw_instr = memory->read64(pc);

        instr.opcode = static_cast<Opcode>((raw_instr >> 56) & 0xFF);
        instr.mode = static_cast<AddressingMode>((raw_instr >> 52) & 0xF);
        instr.reg1 = (raw_instr >> 48) & 0xF;
        instr.reg2 = (raw_instr >> 44) & 0xF;
        instr.immediate = raw_instr & 0xFFFFFFFF;

        pc += 8; // Instruction de 64 bits
    }

    void CPU::executeInstruction(const Instruction& instr) {
        switch (instr.opcode) {
            case Opcode::MOV:   executeMov(instr); break;
            case Opcode::LOAD:  executeLoad(instr); break;
            case Opcode::STORE: executeStore(instr); break;
            case Opcode::PUSH:  executePush(instr); break;
            case Opcode::POP:   executePop(instr); break;
            case Opcode::ADD:   executeAdd(instr); break;
            case Opcode::SUB:   executeSub(instr); break;
            case Opcode::JMP:   executeJmp(instr); break;
            case Opcode::CALL:  executeCall(instr); break;
            case Opcode::RET:   executeRet(instr); break;
            case Opcode::HLT:   executeHlt(instr); break;
            case Opcode::NOP:   break; // Ne fait rien
            default:
                std::cerr << "Instruction non implémentée: 0x"
                         << std::hex << static_cast<int>(instr.opcode) << std::endl;
                halt();
        }
    }

    void CPU::executeMov(const Instruction& instr) {
        uint64_t value = getOperandValue(instr, true); // Source
        setOperandValue(instr, value, false); // Destination
    }

    void CPU::executeLoad(const Instruction& instr) {
        uint64_t address = getOperandValue(instr, true);
        uint64_t value = memory->read64(address);
        registers[instr.reg1] = value;
    }

    void CPU::executeStore(const Instruction& instr) {
        uint64_t address = getOperandValue(instr, false);
        uint64_t value = registers[instr.reg2];
        memory->write64(address, value);
    }

    void CPU::executePush(const Instruction& instr) {
        uint64_t value = getOperandValue(instr);
        sp -= 8;
        memory->write64(sp, value);
    }

    void CPU::executePop(const Instruction& instr) {
        uint64_t value = memory->read64(sp);
        registers[instr.reg1] = value;
        sp += 8;
    }

    void CPU::executeAdd(const Instruction& instr) {
        uint64_t op1 = registers[instr.reg1];
        uint64_t op2 = getOperandValue(instr, true);
        uint64_t result = op1 + op2;

        registers[instr.reg1] = result;
        updateFlags(result, result < op1); // Détection de carry
    }

    void CPU::executeSub(const Instruction& instr) {
        uint64_t op1 = registers[instr.reg1];
        uint64_t op2 = getOperandValue(instr, true);
        uint64_t result = op1 - op2;

        registers[instr.reg1] = result;
        updateFlags(result, op1 < op2); // Détection de borrow
    }

    void CPU::executeJmp(const Instruction& instr) {
        uint64_t address = getOperandValue(instr);
        pc = address;
    }

    void CPU::executeCall(const Instruction& instr) {
        // Sauvegarder l'adresse de retour
        sp -= 8;
        memory->write64(sp, pc);

        // Saut vers la fonction
        uint64_t address = getOperandValue(instr);
        pc = address;
    }

    void CPU::executeRet(const Instruction&) {
        // Restaurer l'adresse de retour
        uint64_t return_address = memory->read64(sp);
        sp += 8;
        pc = return_address;
    }

    void CPU::executeHlt(const Instruction&) {
        running = false;
        if (debug_mode) {
            std::cout << "CPU halted." << std::endl;
        }
    }

    uint64_t CPU::getOperandValue(const Instruction& instr, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                return registers[reg];
            case AddressingMode::IMMEDIATE:
                return instr.immediate;
            case AddressingMode::MEMORY:
                return memory->read64(instr.immediate);
            case AddressingMode::REGISTER_INDIRECT:
                return memory->read64(registers[reg]);
            default:
                return 0;
        }
    }

    void CPU::setOperandValue(const Instruction& instr, uint64_t value, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                registers[reg] = value;
                break;
            case AddressingMode::MEMORY:
                memory->write64(instr.immediate, value);
                break;
            case AddressingMode::REGISTER_INDIRECT:
                memory->write64(registers[reg], value);
                break;
            default:
                // Mode invalide pour écriture
                break;
        }
    }

    void CPU::setFlag(FlagType flag, bool value) {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        if (value) {
            flags |= mask;
        } else {
            flags &= ~mask;
        }
    }

    bool CPU::getFlag(FlagType flag) const {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        return (flags & mask) != 0;
    }

    void CPU::updateFlags(uint64_t result, bool carry, bool overflow) {
        setFlag(FlagType::ZERO, result == 0);
        setFlag(FlagType::CARRY, carry);
        setFlag(FlagType::NEGATIVE, (result & 0x8000000000000000ULL) != 0);
        setFlag(FlagType::OF, overflow);
    }

    uint64_t CPU::getRegister(uint8_t reg) const {
        if (reg < REGISTER_COUNT) {
            return registers[reg];
        }
        return 0;
    }

    void CPU::setRegister(uint8_t reg, uint64_t value) {
        if (reg < REGISTER_COUNT) {
            registers[reg] = value;
        }
    }

    void CPU::printState() const {
        std::cout << "\n=== CPU State ===" << std::endl;
        std::cout << "PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << pc << std::endl;
        std::cout << "SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << sp << std::endl;
        std::cout << "Flags: 0x" << std::hex << std::setfill('0') << std::setw(8) << flags << std::endl;

        std::cout << "Registers:" << std::endl;
        for (size_t i = 0; i < REGISTER_COUNT; ++i) {
            std::cout << "R" << std::dec << i << ": 0x"
                      << std::hex << std::setfill('0') << std::setw(16)
                      << registers[i] << std::endl;
        }
        std::cout << "=================" << std::endl;
    }
}