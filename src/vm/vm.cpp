//
// Created by Jean-Michel Frouin on 17/08/2025.
//

// src/vm.cpp
#include "vm.h"
#include <iostream>
#include <fstream>

namespace vm {
    VirtualMachine::VirtualMachine(size_t memorySize)
        : memory(memorySize), cpu(&memory), debug_enabled(false) {
    }

    void VirtualMachine::reset() {
        memory.clear();
        cpu.reset();
    }

    void VirtualMachine::run() {
        if (debug_enabled) {
            std::cout << "Starting VM execution..." << std::endl;
        }
        cpu.run();
        if (debug_enabled) {
            std::cout << "VM execution finished." << std::endl;
        }
    }

    void VirtualMachine::step() {
        cpu.step();
    }

    void VirtualMachine::halt() {
        cpu.halt();
    }

    bool VirtualMachine::loadProgram(const std::vector<uint64_t>& program, uint64_t loadAddress) {
        try {
            for (size_t i = 0; i < program.size(); ++i) {
                memory.write64(loadAddress + i * 8, program[i]);
            }
            cpu.setPC(loadAddress);
            if (debug_enabled) {
                std::cout << "Program loaded at address 0x" << std::hex << loadAddress << std::endl;
                std::cout << "Program size: " << std::dec << program.size() << " instructions" << std::endl;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading program: " << e.what() << std::endl;
            return false;
        }
    }

    void VirtualMachine::enableDebugger(bool enable) {
        debug_enabled = enable;
        cpu.enableDebug(enable);
    }

    void VirtualMachine::printState() const {
        cpu.printState();
    }

    void VirtualMachine::dumpMemory(uint64_t start, uint64_t length) const {
        memory.dump(start, length);
    }
}
