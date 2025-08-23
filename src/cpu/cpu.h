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
        uint32_t mFlags;     // Registre de drapeaux
        Memory* mMemory;
        bool mRunning;
        bool mDebug;

        // Méthodes privées
        void fetchInstruction(Instruction& instr);
        void decodeInstruction(const Instruction& instr);
        void executeInstruction(const Instruction& instr);

        // Instructions spécifiques
        void executeMov(const Instruction& instr);
        void executeLoad(const Instruction& instr);
        void executeStore(const Instruction& instr);
        void executePush(const Instruction& instr);
        void executePop(const Instruction& instr);
        void executeAdd(const Instruction& instr);
        void executeSub(const Instruction& instr);
        void executeJmp(const Instruction& instr);
        void executeCall(const Instruction& instr);
        void executeRet(const Instruction& instr);
        void executeHlt(const Instruction& instr);

        uint64_t getOperandValue(const Instruction& instr, bool isSecondOperand = false);
        void setOperandValue(const Instruction& instr, uint64_t value, bool isSecondOperand = false);

    public:
        CPU(Memory* mem);
        ~CPU() = default;

        void reset();
        void step();            // Exécute une instruction
        void run();             // Boucle d'exécution
        void halt() { mRunning = false; }
        bool isRunning() const { return mRunning; }

        // Gestion des interruptions
        void handleInterrupt(int num);

        // Gestion des drapeaux
        void setFlag(FlagType flag, bool value);
        bool getFlag(FlagType flag) const;
        void updateFlags(uint64_t result, bool carry = false, bool overflow = false);

        // Accès aux registres
        uint64_t getRegister(uint8_t reg) const;
        void setRegister(uint8_t reg, uint64_t value);
        uint64_t getPC() const { return mPC; }
        void setPC(uint64_t address) { mPC = address; }
        uint64_t getSP() const { return mSP; }
        void setSP(uint64_t address) { mSP = address; }

        // Debug
        void enableDebug(bool enable = true) { mDebug = enable; }
        void printState() const;
    };
}

#endif // VM_CPU_H