// src/cpu/cpu.h
#ifndef VM_CPU_H
#define VM_CPU_H

#include <common/types.h>
#include <memory/memory.h>
#include <array>

namespace vm {
    class CPU {
    private:
        std::array<uint64_t, REGISTER_COUNT> mRegisters;
        uint64_t mPC;        // Program Counter
        uint64_t mSP;        // Stack Pointer
        uint32_t mFlags;     // Flags register
        Memory* mMemory;
        bool mRunning;
        bool mDebug;
        bool mStepByStep;    // Step-by-step mode

        // Private methods
        void FetchInstruction(Instruction& instr);
        void DecodeInstruction(const Instruction& instr);
        void ExecuteInstruction(const Instruction& instr);
        void WaitForKey() const; // Wait for key press
        void ClearScreen() const; // Clear screen

        // Specific instructions
        void ExecuteMov(const Instruction& instr);
        void ExecuteLoad(const Instruction& instr);
        void ExecuteStore(const Instruction& instr);
        void ExecutePush(const Instruction& instr);
        void ExecutePop(const Instruction& instr);
        void ExecuteAdd(const Instruction& instr);
        void ExecuteSub(const Instruction& instr);
        void ExecuteJmp(const Instruction& instr);
        void ExecuteCall(const Instruction& instr);
        void ExecuteRet(const Instruction& instr);
        void ExecuteHlt(const Instruction& instr);

        uint64_t GetOperandValue(const Instruction& instr, bool isSecondOperand = false);
        void SetOperandValue(const Instruction& instr, uint64_t value, bool isSecondOperand = false);

    public:
        CPU(Memory* mem);
        ~CPU() = default;

        void Reset();
        void Step();            // Execute one instruction
        void Run();             // Execution loop
        void Halt() { mRunning = false; }
        bool IsRunning() const { return mRunning; }

        // Interrupt management
        void HandleInterrupt(int num);

        // Flag management
        void SetFlag(FlagType flag, bool value);
        bool GetFlag(FlagType flag) const;
        void UpdateFlags(uint64_t result, bool carry = false, bool overflow = false);

        // Register access
        uint64_t GetRegister(uint8_t reg) const;
        void SetRegister(uint8_t reg, uint64_t value);
        uint64_t GetPC() const { return mPC; }
        void SetPC(uint64_t address) { mPC = address; }
        uint64_t GetSP() const { return mSP; }
        void SetSP(uint64_t address) { mSP = address; }

        // Debug
        void EnableDebug(bool enable = true) { mDebug = enable; }
        void EnableStepByStep(bool enable = true) { mStepByStep = enable; }
        void PrintState() const;
    };
}

#endif // VM_CPU_H