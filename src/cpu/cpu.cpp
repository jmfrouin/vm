//
// Created by Jean-Michel Frouin on 17/08/2025.
//
// src/cpu/cpu.cpp

#include "cpu.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

namespace vm {
    // Utility function to convert opcode to string
    const char* OpcodeToString(Opcode opcode) {
        switch (opcode) {
            case Opcode::MOV:   return "MOV";
            case Opcode::LOAD:  return "LOAD";
            case Opcode::STORE: return "STORE";
            case Opcode::PUSH:  return "PUSH";
            case Opcode::POP:   return "POP";
            case Opcode::ADD:   return "ADD";
            case Opcode::SUB:   return "SUB";
            case Opcode::MUL:   return "MUL";
            case Opcode::DIV:   return "DIV";
            case Opcode::MOD:   return "MOD";
            case Opcode::INC:   return "INC";
            case Opcode::DEC:   return "DEC";
            case Opcode::CMP:   return "CMP";
            case Opcode::AND:   return "AND";
            case Opcode::OR:    return "OR";
            case Opcode::XOR:   return "XOR";
            case Opcode::NOT:   return "NOT";
            case Opcode::SHL:   return "SHL";
            case Opcode::SHR:   return "SHR";
            case Opcode::JMP:   return "JMP";
            case Opcode::JZ:    return "JZ";
            case Opcode::JNZ:   return "JNZ";
            case Opcode::JC:    return "JC";
            case Opcode::JNC:   return "JNC";
            case Opcode::CALL:  return "CALL";
            case Opcode::RET:   return "RET";
            case Opcode::NOP:   return "NOP";
            case Opcode::INT:   return "INT";
            case Opcode::HLT:   return "HLT";
            case Opcode::IN:    return "IN";
            case Opcode::OUT:   return "OUT";
            default:            return "UNKNOWN";
        }
    }

    CPU::CPU(Memory* mem) : mMemory(mem), mRunning(false), mDebug(false), mStepByStep(false) {
        Reset();
    }

    void CPU::Reset() {
        mRegisters.fill(0);
        mPC = 0;
        mSP = 0xFFFFF0; // Stack starts at top of memory
        mFlags = 0;
        mRunning = false;
    }

    void CPU::ClearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void CPU::WaitForKey() const {
        std::cout << "\n📝 Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    void CPU::Step() {
        if (!mRunning) return;

        if (mDebug && mStepByStep) {
            ClearScreen(); // Clear screen before each step
        }

        Instruction instr;
        FetchInstruction(instr);

        if (mDebug) {
            std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                    EXECUTION STEP                          ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
            
            std::cout << "📍 PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << (mPC - 8) << " ";
            std::cout << "📦 SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << mSP << std::endl;
            std::cout << "🔧 Instruction: " << OpcodeToString(instr.opcode) 
                      << " (0x" << std::hex << std::setfill('0') << std::setw(2) 
                      << static_cast<int>(instr.opcode) << ")" << std::endl;
            
            // Display state before execution
            std::cout << "\n┌─ State BEFORE execution ─┐" << std::endl;
            PrintState();
        }

        ExecuteInstruction(instr);

        if (mDebug) {
            std::cout << "\n┌─ State AFTER execution ─┐" << std::endl;
            PrintState();
            std::cout << "\n" << std::string(60, '=') << std::endl;
            
            if (mStepByStep && mRunning) {
                WaitForKey();
            }
        }
    }

    void CPU::Run() {
        mRunning = true;
        while (mRunning) {
            Step();
        }
    }

    void CPU::FetchInstruction(Instruction& instr) {
        // Read instruction from memory
        uint64_t raw_instr = mMemory->Read64(mPC);

        instr.opcode = static_cast<Opcode>((raw_instr >> 56) & 0xFF);
        instr.mode = static_cast<AddressingMode>((raw_instr >> 52) & 0xF);
        instr.reg1 = (raw_instr >> 48) & 0xF;
        instr.reg2 = (raw_instr >> 44) & 0xF;
        instr.immediate = raw_instr & 0xFFFFFFFF;

        mPC += 8; // 64-bit instruction
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
            case Opcode::NOP:   break; // Do nothing
            default:
                std::cerr << "❌ Unimplemented instruction: " << OpcodeToString(instr.opcode)
                         << " (0x" << std::hex << static_cast<int>(instr.opcode) << ")" << std::endl;
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
        UpdateFlags(result, result < op1); // Carry detection
    }

    void CPU::ExecuteSub(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 - op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, op1 < op2); // Borrow detection
    }

    void CPU::ExecuteJmp(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr);
        mPC = address;
    }

    void CPU::ExecuteCall(const Instruction& instr) {
        // Save return address
        mSP -= 8;
        mMemory->Write64(mSP, mPC);

        // Jump to function
        uint64_t address = GetOperandValue(instr);
        mPC = address;
    }

    void CPU::ExecuteRet(const Instruction&) {
        // Restore return address
        uint64_t return_address = mMemory->Read64(mSP);
        mSP += 8;
        mPC = return_address;
    }

    void CPU::ExecuteHlt(const Instruction&) {
        mRunning = false;
        if (mDebug) {
            std::cout << "\n🛑 CPU STOPPED (HLT)" << std::endl;
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
                // Invalid mode for writing
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
        std::cout << "\n┌── CPU State ──┐" << std::endl;
        std::cout << "│ PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << mPC << " │" << std::endl;
        std::cout << "│ SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << mSP << " │" << std::endl;
        std::cout << "│ Flags: 0x" << std::hex << std::setfill('0') << std::setw(8) << mFlags << "     │" << std::endl;
        std::cout << "└─────────────────┘" << std::endl;

        std::cout << "\n📋 Registers:" << std::endl;
        for (size_t i = 0; i < REGISTER_COUNT; i += 4) {
            for (size_t j = 0; j < 4 && (i + j) < REGISTER_COUNT; ++j) {
                size_t reg_idx = i + j;
                std::cout << "R" << std::dec << std::setfill(' ') << std::setw(2) << reg_idx 
                         << ":0x" << std::hex << std::setfill('0') << std::setw(16)
                         << mRegisters[reg_idx];
                if (j < 3 && (i + j + 1) < REGISTER_COUNT) std::cout << " │ ";
            }
            std::cout << std::endl;
        }
    }

    void CPU::HandleInterrupt(int num) {
        // Basic interrupt implementation
        if (mDebug) {
            std::cout << "⚡ Interrupt " << num << " triggered" << std::endl;
        }
        
        // Save current state
        mSP -= 8;
        mMemory->Write64(mSP, mPC);
        mSP -= 8;
        mMemory->Write64(mSP, mFlags);
        
        // Jump to interrupt handler
        // For now, use a simple table
        uint64_t handlerAddress = num * 8; // Each entry is 8 bytes
        mPC = mMemory->Read64(handlerAddress);
        
        // Disable interrupts during handling
        SetFlag(FlagType::INTERRUPT, false);
    }
}