//
// Created by Jean-Michel Frouin on 17/08/2025.
//

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

        // Data Transfer Instructions
        
        // Move data between registers or load immediate values into register
        void ExecuteMov(const Instruction& instr);
        
        // Load data from memory address into register
        void ExecuteLoad(const Instruction& instr);
        
        // Store register data into memory address
        void ExecuteStore(const Instruction& instr);
        
        // Push register value onto the stack
        void ExecutePush(const Instruction& instr);
        
        // Pop value from stack into register
        void ExecutePop(const Instruction& instr);

        // Arithmetic Instructions
        
        // Add two values and store result in destination register
        void ExecuteAdd(const Instruction& instr);
        
        // Subtract second operand from first and store result in destination register
        void ExecuteSub(const Instruction& instr);
        
        // Multiply two values and store result in destination register (with overflow detection)
        void ExecuteMul(const Instruction& instr);
        
        // Divide first operand by second and store result in destination register
        void ExecuteDiv(const Instruction& instr);
        
        // Calculate modulo (remainder) of division and store in destination register
        void ExecuteMod(const Instruction& instr);
        
        // Increment register value by 1
        void ExecuteInc(const Instruction& instr);
        
        // Decrement register value by 1
        void ExecuteDec(const Instruction& instr);
        
        // Compare two values by subtraction (sets flags but doesn't store result)
        void ExecuteCmp(const Instruction& instr);
        
        // Swap values between two registers
        void ExecuteSwap(const Instruction& instr);

        // Logical Instructions
        
        // Bitwise AND operation between two operands
        void ExecuteAnd(const Instruction& instr);
        
        // Bitwise OR operation between two operands
        void ExecuteOr(const Instruction& instr);
        
        // Bitwise XOR (exclusive OR) operation between two operands
        void ExecuteXor(const Instruction& instr);
        
        // Bitwise NOT operation (one's complement) on register
        void ExecuteNot(const Instruction& instr);
        
        // Shift bits left by specified amount (logical shift)
        void ExecuteShl(const Instruction& instr);
        
        // Shift bits right by specified amount (logical shift)
        void ExecuteShr(const Instruction& instr);

        // Control Flow Instructions
        
        // Unconditional jump to specified address
        void ExecuteJmp(const Instruction& instr);
        
        // Jump if Zero flag is set (result of last operation was zero)
        void ExecuteJz(const Instruction& instr);
        
        // Jump if Zero flag is not set (result of last operation was not zero)
        void ExecuteJnz(const Instruction& instr);
        
        // Jump if Equal (alias for JZ - jump if last comparison showed equality)
        void ExecuteJeq(const Instruction& instr);
        
        // Jump if Not Equal (alias for JNZ - jump if last comparison showed inequality)
        void ExecuteJne(const Instruction& instr);
        
        // Jump if Carry flag is set (unsigned overflow or borrow occurred)
        void ExecuteJc(const Instruction& instr);
        
        // Jump if Carry flag is not set (no unsigned overflow or borrow)
        void ExecuteJnc(const Instruction& instr);
        
        // Jump if Less (signed comparison - first operand < second operand)
        void ExecuteJl(const Instruction& instr);
        
        // Jump if Less or Equal (signed comparison - first operand <= second operand)
        void ExecuteJle(const Instruction& instr);
        
        // Jump if Greater (signed comparison - first operand > second operand)
        void ExecuteJg(const Instruction& instr);
        
        // Jump if Greater or Equal (signed comparison - first operand >= second operand)
        void ExecuteJge(const Instruction& instr);
        
        // Decrement register and jump to address if register is not zero (loop construct)
        void ExecuteLoop(const Instruction& instr);

        // Function Call Instructions
        
        // Call function at address (saves return address on stack)
        void ExecuteCall(const Instruction& instr);
        
        // Return from function (restores return address from stack)
        void ExecuteRet(const Instruction& instr);

        // System Instructions
        
        // Halt CPU execution (stop the virtual machine)
        void ExecuteHlt(const Instruction& instr);
        
        // Print value to console output for debugging purposes
        void ExecutePrint(const Instruction& instr);
        
        // Read input from specified port into register
        void ExecuteIn(const Instruction& instr);
        
        // Write register value to specified output port
        void ExecuteOut(const Instruction& instr);

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