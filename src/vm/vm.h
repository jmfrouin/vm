// src/vm/vm.h
#ifndef VM_VIRTUAL_MACHINE_H
#define VM_VIRTUAL_MACHINE_H

#include <common/types.h>
#include <memory/memory.h>
#include <cpu/cpu.h>
#include <vector>
#include <memory>

namespace vm {
    class VirtualMachine {
    private:
        std::unique_ptr<Memory> mMemory;
        std::unique_ptr<CPU> mCPU;
        bool mDebugMode;
        bool mRunning;

        // Méthodes privées
        void InitializeSystem();
        void Shutdown();

    public:
        VirtualMachine(size_t memorySize = 1024 * 1024); // 1MB par défaut
        ~VirtualMachine();

        // Gestion du cycle de vie
        bool LoadProgram(const std::vector<uint64_t>& program, uint64_t startAddress = 0);
        void Run();
        void Step();
        void Stop();
        void Reset();

        // Accès aux composants
        Memory& GetMemory() { return *mMemory; }
        const Memory& GetMemory() const { return *mMemory; }
        CPU& GetCPU() { return *mCPU; }
        const CPU& GetCPU() const { return *mCPU; }

        // Debug et monitoring
        void EnableDebugger(bool enable = true) { mDebugMode = enable; }
        bool IsDebugging() const { return mDebugMode; }
        bool IsRunning() const { return mRunning; }
        void PrintState() const;
        void DumpMemory(uint64_t start, uint64_t length) const;

        // Utilitaires
        void DumpRegisters() const;
        void SetBreakpoint(uint64_t address);
        void RemoveBreakpoint(uint64_t address);
    };
}

#endif // VM_VIRTUAL_MACHINE_H