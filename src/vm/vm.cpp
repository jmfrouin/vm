//
// Created by Jean-Michel Frouin on 17/08/2025.
//

#include "vm.h"
#include <iostream>
#include <iomanip>

namespace vm {
    VirtualMachine::VirtualMachine(size_t memorySize) 
        : mMemory(std::make_unique<Memory>(memorySize))
        , mCPU(std::make_unique<CPU>(mMemory.get()))
        , mDebugMode(false)
        , mRunning(false) {
        InitializeSystem();
    }

    VirtualMachine::~VirtualMachine() {
        Shutdown();
    }

    void VirtualMachine::InitializeSystem() {
        mCPU->Reset();
        mMemory->Clear();
        mRunning = false;
        
        if (mDebugMode) {
            std::cout << "Virtual Machine initialized with " 
                      << mMemory->GetSize() << " bytes of RAM" << std::endl;
        }
    }

    void VirtualMachine::Shutdown() {
        if (mRunning) {
            Stop();
        }
        
        if (mDebugMode) {
            std::cout << "Virtual Machine shutdown complete" << std::endl;
        }
    }

    void VirtualMachine::EnableStepByStep(bool enable) {
        if (mCPU) {
            mCPU->EnableStepByStep(enable);
        }
    }

    bool VirtualMachine::LoadProgram(const std::vector<uint64_t>& program, uint64_t startAddress) {
        if (program.empty()) {
            if (mDebugMode) {
                std::cerr << "Error: Cannot load empty program" << std::endl;
            }
            return false;
        }

        // Vérifier que le programme peut être chargé à l'adresse spécifiée
        uint64_t programSize = program.size() * 8; // 8 bytes per instruction
        if (startAddress + programSize > mMemory->GetSize()) {
            if (mDebugMode) {
                std::cerr << "Error: Program too large for available memory" << std::endl;
            }
            return false;
        }

        try {
            // Charger le programme en mémoire
            for (size_t i = 0; i < program.size(); ++i) {
                uint64_t address = startAddress + (i * 8);
                mMemory->Write64(address, program[i]);
            }

            // Positionner le PC au début du programme
            mCPU->SetPC(startAddress);
            
            if (mDebugMode) {
                std::cout << "Program loaded successfully at address 0x" 
                         << std::hex << startAddress << std::endl;
                std::cout << "Program size: " << std::dec << program.size() 
                         << " instructions (" << programSize << " bytes)" << std::endl;
            }
            
            return true;
        } catch (const std::exception& e) {
            if (mDebugMode) {
                std::cerr << "Error loading program: " << e.what() << std::endl;
            }
            return false;
        }
    }

    void VirtualMachine::Run() {
        if (!mMemory || !mCPU) {
            if (mDebugMode) {
                std::cerr << "Error: System not properly initialized" << std::endl;
            }
            return;
        }

        mRunning = true;
        
        if (mDebugMode) {
            std::cout << "Starting program execution..." << std::endl;
        }

        try {
            mCPU->EnableDebug(mDebugMode);
            mCPU->Run();
        } catch (const std::exception& e) {
            if (mDebugMode) {
                std::cerr << "Runtime error: " << e.what() << std::endl;
            }
            Stop();
        }

        mRunning = mCPU->IsRunning();
        
        if (mDebugMode && !mRunning) {
            std::cout << "Program execution completed" << std::endl;
        }
    }

    void VirtualMachine::Step() {
        if (!mMemory || !mCPU) {
            if (mDebugMode) {
                std::cerr << "Error: System not properly initialized" << std::endl;
            }
            return;
        }

        if (!mRunning) {
            mRunning = true;
        }

        try {
            mCPU->EnableDebug(mDebugMode);
            mCPU->Step();
            mRunning = mCPU->IsRunning();
        } catch (const std::exception& e) {
            if (mDebugMode) {
                std::cerr << "Runtime error: " << e.what() << std::endl;
            }
            Stop();
        }
    }

    void VirtualMachine::Stop() {
        if (mCPU) {
            mCPU->Halt();
        }
        mRunning = false;
        
        if (mDebugMode) {
            std::cout << "Virtual Machine stopped" << std::endl;
        }
    }

    void VirtualMachine::Reset() {
        Stop();
        InitializeSystem();
        
        if (mDebugMode) {
            std::cout << "Virtual Machine reset complete" << std::endl;
        }
    }

    void VirtualMachine::PrintState() const {
        if (!mCPU) {
            std::cout << "CPU not initialized" << std::endl;
            return;
        }

        std::cout << "\n=== Virtual Machine State ===" << std::endl;
        std::cout << "Running: " << (mRunning ? "Yes" : "No") << std::endl;
        std::cout << "Debug Mode: " << (mDebugMode ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Memory Size: " << mMemory->GetSize() << " bytes" << std::endl;
        
        mCPU->PrintState();
        std::cout << "=============================" << std::endl;
    }

    void VirtualMachine::DumpMemory(uint64_t start, uint64_t length) const {
        if (mMemory) {
            mMemory->Dump(start, length);
        }
    }

    void VirtualMachine::DumpRegisters() const {
        if (!mCPU) {
            std::cout << "CPU not initialized" << std::endl;
            return;
        }

        std::cout << "\n=== Register Dump ===" << std::endl;
        for (size_t i = 0; i < REGISTER_COUNT; ++i) {
            std::cout << "R" << std::dec << std::setfill(' ') << std::setw(2) << i 
                      << ": 0x" << std::hex << std::setfill('0') << std::setw(16) 
                      << mCPU->GetRegister(static_cast<uint8_t>(i)) << std::endl;
        }
        std::cout << "PC : 0x" << std::hex << std::setfill('0') << std::setw(16) 
                  << mCPU->GetPC() << std::endl;
        std::cout << "SP : 0x" << std::hex << std::setfill('0') << std::setw(16) 
                  << mCPU->GetSP() << std::endl;
        std::cout << "====================" << std::endl;
    }

    void VirtualMachine::SetBreakpoint(uint64_t address) {
        // Implementation for breakpoints would go here
        if (mDebugMode) {
            std::cout << "Breakpoint set at address 0x" 
                      << std::hex << address << std::endl;
        }
    }

    void VirtualMachine::RemoveBreakpoint(uint64_t address) {
        // Implementation for breakpoint removal would go here
        if (mDebugMode) {
            std::cout << "Breakpoint removed from address 0x" 
                      << std::hex << address << std::endl;
        }
    }
}