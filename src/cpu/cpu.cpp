//
// Created by Jean-Michel Frouin on 17/08/2025.
//
// src/cpu/cpu.cpp

#include "cpu.h"
#include <iostream>
#include <iomanip>

namespace vm {
    CPU::CPU(Memory* mem) : mMemory(mem), mRunning(false), mDebug(false) {
        Reset();
    }

    void CPU::Reset() {
        mRegisters.fill(0);
        mPC = 0;
        mSP = 0xFFFFF0; // Début de la pile en haut de la mémoire
        mFlags = 0;
        mRunning = false;
    }

    void CPU::Step() {
        if (!mRunning) return;

        Instruction instr;
        FetchInstruction(instr);

        if (mDebug) {
            std::cout << "PC: 0x" << std::hex << mPC << " - ";
            std::cout << "Opcode: 0x" << static_cast<int>(instr.opcode) << std::endl;
        }

        ExecuteInstruction(instr);
    }

    void CPU::Run() {
        mRunning = true;
        while (mRunning) {
            Step();
        }
    }

    void CPU::FetchInstruction(Instruction& instr) {
        // Lecture de l'instruction depuis la mémoire
        uint64_t raw_instr = mMemory->Read64(mPC);

        instr.opcode = static_cast<Opcode>((raw_instr >> 56) & 0xFF);
        instr.mode = static_cast<AddressingMode>((raw_instr >> 52) & 0xF);
        instr.reg1 = (raw_instr >> 48) & 0xF;
        instr.reg2 = (raw_instr >> 44) & 0xF;
        instr.immediate = raw_instr & 0xFFFFFFFF;

        mPC += 8; // Instruction de 64 bits
    }

    void CPU::ExecuteInstruction(const Instruction& instr) {
        switch (instr.opcode) {
            case Opcode::MOV:   ExecuteMov(instr); break;
            case Opcode::LOAD:  ExecuteLoad(instr); break;
            case Opcode::STORE: ExecuteStore(instr); break;
            case Opcode::PUSH:  ExecutePush(instr); break;
            case Opcode::POP:   ExecutePop(instr); break;
            case Opcode::ADD:   ExecuteAdd(instr); break;
            case Opcode::SUB:   ExecuteSub(instr); break;
            case Opcode::JMP:   ExecuteJmp(instr); break;
            case Opcode::CALL:  ExecuteCall(instr); break;
            case Opcode::RET:   ExecuteRet(instr); break;
            case Opcode::HLT:   ExecuteHlt(instr); break;
            case Opcode::NOP:   break; // Ne fait rien
            default:
                std::cerr << "Instruction non implémentée: 0x"
                         << std::hex << static_cast<int>(instr.opcode) << std::endl;
                Halt();
        }
    }

    void CPU::ExecuteMov(const Instruction& instr) {
        uint64_t value = GetOperandValue(instr, true); // Source
        SetOperandValue(instr, value, false); // Destination
    }

    void CPU::ExecuteLoad(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr, true);
        uint64_t value = mMemory->Read64(address);
        mRegisters[instr.reg1] = value;
    }

    void CPU::ExecuteStore(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr, false);
        uint64_t value = mRegisters[instr.reg2];
        mMemory->Write64(address, value);
    }

    void CPU::ExecutePush(const Instruction& instr) {
        uint64_t value = GetOperandValue(instr);
        mSP -= 8;
        mMemory->Write64(mSP, value);
    }

    void CPU::ExecutePop(const Instruction& instr) {
        uint64_t value = mMemory->Read64(mSP);
        mRegisters[instr.reg1] = value;
        mSP += 8;
    }

    void CPU::ExecuteAdd(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 + op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, result < op1); // Détection de carry
    }

    void CPU::ExecuteSub(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 - op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, op1 < op2); // Détection de borrow
    }

    void CPU::ExecuteJmp(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr);
        mPC = address;
    }

    void CPU::ExecuteCall(const Instruction& instr) {
        // Sauvegarder l'adresse de retour
        mSP -= 8;
        mMemory->Write64(mSP, mPC);

        // Saut vers la fonction
        uint64_t address = GetOperandValue(instr);
        mPC = address;
    }

    void CPU::ExecuteRet(const Instruction&) {
        // Restaurer l'adresse de retour
        uint64_t return_address = mMemory->Read64(mSP);
        mSP += 8;
        mPC = return_address;
    }

    void CPU::ExecuteHlt(const Instruction&) {
        mRunning = false;
        if (mDebug) {
            std::cout << "CPU halted." << std::endl;
        }
    }

    uint64_t CPU::GetOperandValue(const Instruction& instr, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                return mRegisters[reg];
            case AddressingMode::IMMEDIATE:
                return instr.immediate;
            case AddressingMode::MEMORY:
                return mMemory->Read64(instr.immediate);
            case AddressingMode::REGISTER_INDIRECT:
                return mMemory->Read64(mRegisters[reg]);
            default:
                return 0;
        }
    }

    void CPU::SetOperandValue(const Instruction& instr, uint64_t value, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                mRegisters[reg] = value;
                break;
            case AddressingMode::MEMORY:
                mMemory->Write64(instr.immediate, value);
                break;
            case AddressingMode::REGISTER_INDIRECT:
                mMemory->Write64(mRegisters[reg], value);
                break;
            default:
                // Mode invalide pour écriture
                break;
        }
    }

    void CPU::SetFlag(FlagType flag, bool value) {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        if (value) {
            mFlags |= mask;
        } else {
            mFlags &= ~mask;
        }
    }

    bool CPU::GetFlag(FlagType flag) const {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        return (mFlags & mask) != 0;
    }

    void CPU::UpdateFlags(uint64_t result, bool carry, bool overflow) {
        SetFlag(FlagType::ZERO, result == 0);
        SetFlag(FlagType::CARRY, carry);
        SetFlag(FlagType::NEGATIVE, (result & 0x8000000000000000ULL) != 0);
        SetFlag(FlagType::OF, overflow);
    }

    uint64_t CPU::GetRegister(uint8_t reg) const {
        if (reg < REGISTER_COUNT) {
            return mRegisters[reg];
        }
        return 0;
    }

    void CPU::SetRegister(uint8_t reg, uint64_t value) {
        if (reg < REGISTER_COUNT) {
            mRegisters[reg] = value;
        }
    }

    void CPU::PrintState() const {
        std::cout << "\n=== CPU State ===" << std::endl;
        std::cout << "PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << mPC << std::endl;
        std::cout << "SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << mSP << std::endl;
        std::cout << "Flags: 0x" << std::hex << std::setfill('0') << std::setw(8) << mFlags << std::endl;

        std::cout << "Registers:" << std::endl;
        for (size_t i = 0; i < REGISTER_COUNT; ++i) {
            std::cout << "R" << std::dec << i << ": 0x"
                      << std::hex << std::setfill('0') << std::setw(16)
                      << mRegisters[i] << std::endl;
        }
        std::cout << "=================" << std::endl;
    }

    void CPU::HandleInterrupt(int num) {
        // Implémentation basique des interruptions
        if (mDebug) {
            std::cout << "Interrupt " << num << " triggered" << std::endl;
        }
        
        // Sauvegarder l'état actuel
        mSP -= 8;
        mMemory->Write64(mSP, mPC);
        mSP -= 8;
        mMemory->Write64(mSP, mFlags);
        
        // Saut vers le gestionnaire d'interruption
        // Pour l'instant, on utilise une table simple
        uint64_t handlerAddress = num * 8; // Chaque entrée fait 8 bytes
        mPC = mMemory->Read64(handlerAddress);
        
        // Désactiver les interruptions pendant le traitement
        SetFlag(FlagType::INTERRUPT, false);
    }
}