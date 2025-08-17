//
// Created by Jean-Michel Frouin on 17/08/2025.
//

// src/vm.h
#ifndef VM_H
#define VM_H

#include "cpu/cpu.h"
#include "memory/memory.h"
#include <string>
#include <vector>

namespace vm {
    class VirtualMachine {
    private:
        Memory memory;
        CPU cpu;
        bool debug_enabled;

    public:
        VirtualMachine(size_t memorySize = 1024 * 1024); // 1MB par défaut
        ~VirtualMachine() = default;

        // Contrôle de la VM
        void reset();
        void run();
        void step();
        void halt();

        // Chargement de programmes
        bool loadProgram(const std::vector<uint64_t>& program, uint64_t loadAddress = 0);
        bool loadProgramFromFile(const std::string& filename);

        // Debug et introspection
        void enableDebugger(bool enable = true);
        void printState() const;
        void dumpMemory(uint64_t start, uint64_t length) const;

        // Accès aux composants
        CPU& getCPU() { return cpu; }
        Memory& getMemory() { return memory; }
        const CPU& getCPU() const { return cpu; }
        const Memory& getMemory() const { return memory; }
    };
}

#endif // VM_H
