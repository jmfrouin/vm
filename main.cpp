//
// Created by Jean-Michel Frouin on 17/08/2025.
//
#include "src/vm.h"
#include <iostream>
#include <vector>

// Fonction utilitaire pour créer une instruction
uint64_t makeInstruction(vm::Opcode opcode, vm::AddressingMode mode,
                        uint8_t reg1, uint8_t reg2, uint32_t immediate) {
    uint64_t instr = 0;
    instr |= (static_cast<uint64_t>(opcode) << 56);
    instr |= (static_cast<uint64_t>(mode) << 52);
    instr |= (static_cast<uint64_t>(reg1) << 48);
    instr |= (static_cast<uint64_t>(reg2) << 44);
    instr |= static_cast<uint64_t>(immediate);
    return instr;
}

int main() {
    std::cout << "=== Machine Virtuelle Éducative ===" << std::endl;

    // Création de la VM avec 1MB de RAM
    vm::VirtualMachine vm(1024 * 1024);
    vm.enableDebugger(true);

    // Programme de test simple
    std::vector<uint64_t> program = {
        // MOV R0, #42 (charger 42 dans R0)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 42),

        // MOV R1, #10 (charger 10 dans R1)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 10),

        // ADD R0, R1 (R0 = R0 + R1)
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),

        // PUSH R0 (sauvegarder le résultat sur la pile)
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),

        // POP R2 (récupérer le résultat dans R2)
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 2, 0, 0),

        // HLT (arrêter la VM)
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };

    // Chargement et exécution du programme
    if (vm.loadProgram(program)) {
        std::cout << "\nÉtat initial:" << std::endl;
        vm.printState();

        std::cout << "\nExécution du programme..." << std::endl;
        vm.run();

        std::cout << "\nÉtat final:" << std::endl;
        vm.printState();

        std::cout << "\nDump mémoire (pile):" << std::endl;
        vm.dumpMemory(vm.getMemory().getSize() - 0x100, 64);

        std::cout << "\nRésultat attendu: R2 = 52 (42 + 10)" << std::endl;
        std::cout << "Résultat obtenu: R2 = " << std::dec
                  << vm.getCPU().getRegister(2) << std::endl;
    }

    return 0;
}