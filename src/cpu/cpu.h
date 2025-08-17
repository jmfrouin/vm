// src/cpu/cpu.h
#ifndef VM_CPU_H
#define VM_CPU_H

#include <common/types.h>
#include <memory/memory.h>
#include <array>

namespace vm {
    class CPU {
    private:
        std::array<uint64_t, REGISTER_COUNT> registers;
        uint64_t pc;        // Program Counter
        uint64_t sp;        // Stack Pointer
        uint32_t flags;     // Registre de drapeaux
        Memory* memory;
        bool running;
        bool debug_mode;

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
        void halt() { running = false; }
        bool isRunning() const { return running; }

        // Gestion des interruptions
        void handleInterrupt(int num);

        // Gestion des drapeaux
        void setFlag(FlagType flag, bool value);
        bool getFlag(FlagType flag) const;
        void updateFlags(uint64_t result, bool carry = false, bool overflow = false);

        // Accès aux registres
        uint64_t getRegister(uint8_t reg) const;
        void setRegister(uint8_t reg, uint64_t value);
        uint64_t getPC() const { return pc; }
        void setPC(uint64_t address) { pc = address; }
        uint64_t getSP() const { return sp; }
        void setSP(uint64_t address) { sp = address; }

        // Debug
        void enableDebug(bool enable = true) { debug_mode = enable; }
        void printState() const;
    };
}

#endif // VM_CPU_H