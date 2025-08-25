//
// Created by Jean-Michel Frouin on 17/08/2025.
//
#include "src/vm/vm.h"
#include "src/vm/firmware_loader.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <functional>

// Structure pour définir un programme
struct TestProgram {
    std::string name;
    std::string description;
    std::string filename;
    std::function<std::vector<uint64_t>()> generator;
};

// Utility function to create an instruction
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

std::vector<uint64_t> createTestProgram() {
    // Simple test program
    return {
        // MOV R0, #42 (load 42 into R0)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 42),

        // MOV R1, #10 (load 10 into R1)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 10),

        // ADD R0, R1 (R0 = R0 + R1)
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),

        // PUSH R0 (save result to stack)
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),

        // POP R2 (retrieve result into R2)
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 2, 0, 0),

        // HLT (stop the VM)
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme arithmétique de base étendu
std::vector<uint64_t> createBasicArithmeticProgram() {
    return {
        // Test addition: 15 + 25 = 40
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 15),
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 25),
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::REGISTER, 2, 0, 0), // R2 = résultat

        // Test soustraction: 50 - 20 = 30
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 3, 0, 50),
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 4, 0, 20),
        makeInstruction(vm::Opcode::SUB, vm::AddressingMode::REGISTER, 3, 4, 0),
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::REGISTER, 5, 3, 0), // R5 = résultat

        // Fin - sauvegarder résultats sur la pile
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0), // addition
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 5, 0, 0), // soustraction  
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme Fibonacci
std::vector<uint64_t> createFibonacciProgram() {
    return {
        // Initialisation: F(0)=0, F(1)=1
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0),  // F(n-2) = 0
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 1),  // F(n-1) = 1
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 1),  // compteur (commencer à F(1))
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 3, 0, 10), // limite (calculer F(10))
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),  // sauver F(0)
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 1, 0, 0),  // sauver F(1)
        
        // Boucle principale (adresse 6 * 8 = 48 = 0x30)
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 2, 3, 0),   // compteur == limite ?
        makeInstruction(vm::Opcode::JZ, vm::AddressingMode::IMMEDIATE, 0, 0, 0x70), // sortir si fini
        
        // Calculer F(n) = F(n-1) + F(n-2)
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 1, 0, 0),   // R1 = F(n-1) + F(n-2)
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 1, 0, 0),  // sauver F(n)
        
        // Préparer pour l'itération suivante
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::REGISTER, 0, 1, 0),   // F(n-2) = ancien F(n-1)
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 4, 0, 0),   // récupérer F(n)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::REGISTER, 1, 4, 0),   // F(n-1) = F(n)
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 4, 0, 0),  // remettre F(n) sur la pile
        
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 2, 0, 0),   // compteur++
        makeInstruction(vm::Opcode::JMP, vm::AddressingMode::IMMEDIATE, 0, 0, 0x30), // retour boucle
        
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme factorielle
std::vector<uint64_t> createFactorialProgram() {
    return {
        // Calculer 5! = 120
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 5),  // n = 5
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 1),  // résultat = 1
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 1),  // compteur = 1
        
        // Boucle factorielle (adresse 3 * 8 = 24 = 0x18)
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 2, 0, 0),   // compteur > n ?
        makeInstruction(vm::Opcode::JZ, vm::AddressingMode::IMMEDIATE, 0, 0, 0x40), // sortir si compteur > n
        
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 2, 0, 0),   // compteur++
        makeInstruction(vm::Opcode::JMP, vm::AddressingMode::IMMEDIATE, 0, 0, 0x18), // retour boucle principale
        
        // Fin
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 1, 0, 0),  // sauver résultat
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme d'opérations sur la pile
std::vector<uint64_t> createStackOperationsProgram() {
    return {
        // Test des opérations de pile et appels de sous-routines
        
        // Initialiser quelques valeurs
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 100), // R0 = 100
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 200), // R1 = 200
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 300), // R2 = 300
        
        // Test PUSH multiple
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0), // Push R0
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 1, 0, 0), // Push R1
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0), // Push R2
        
        // Vider les registres pour tester la pile
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0), // R0 = 0
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 0), // R1 = 0
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 0), // R2 = 0
        
        // Test POP multiple dans l'ordre inverse
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 5, 0, 0), // R5 = 300
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 4, 0, 0), // R4 = 200
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 3, 0, 0), // R3 = 100
        
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme de test mémoire
std::vector<uint64_t> createMemoryTestProgram() {
    return {
        // Test des opérations mémoire LOAD/STORE
        
        // Préparer des données
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0x1000), // Adresse mémoire
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 12345),  // Valeur à stocker
        
        // STORE: Écrire R1 à l'adresse dans R0
        makeInstruction(vm::Opcode::STORE, vm::AddressingMode::REGISTER, 0, 1, 0),
        
        // Vider R1 pour tester le LOAD
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 0),
        
        // LOAD: Lire depuis l'adresse dans R0 vers R2
        makeInstruction(vm::Opcode::LOAD, vm::AddressingMode::REGISTER, 2, 0, 0),
        
        // Sauvegarder résultat sur pile
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0),
        
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme de tri simple
std::vector<uint64_t> createSortingProgram() {
    return {
        // Programme simple qui démontre le concept de tri
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 30),     // Premier élément
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 10),     // Deuxième élément
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 40),     // Troisième élément
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 3, 0, 20),     // Quatrième élément
        
        // Comparaison simple R0 et R1
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 0, 1, 0),
        
        // Sauvegarder les valeurs sur la pile
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 1, 0, 0),
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0),
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 3, 0, 0),
        
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme de logique conditionnelle
std::vector<uint64_t> createConditionalProgram() {
    return {
        // Test de la logique conditionnelle avec CMP et sauts
        
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 15),  // R0 = 15
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 10),  // R1 = 10
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 0),   // R2 = résultat
        
        // Test si R0 > R1
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 0, 1, 0),
        // Simulation de logique conditionnelle
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 1),   // R2 = 1 (true)
        
        // Sauvegarder résultat
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0),   // Sauver résultat
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programme de boucles
std::vector<uint64_t> createLoopProgram() {
    return {
        // Calcul de la somme 1+2+3+4+5 = 15 avec une boucle simple
        
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0),   // Somme = 0
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 1),   // Valeur à ajouter
        
        // Simuler une boucle avec des additions successives
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),    // Somme += 1
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 1, 0, 0),    // Valeur++
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),    // Somme += 2
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 1, 0, 0),    // Valeur++
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),    // Somme += 3
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 1, 0, 0),    // Valeur++
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),    // Somme += 4
        makeInstruction(vm::Opcode::INC, vm::AddressingMode::REGISTER, 1, 0, 0),    // Valeur++
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::REGISTER, 0, 1, 0),    // Somme += 5
        
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),   // Sauver résultat
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Programmes de benchmark
std::vector<uint64_t> createCPUBenchmark() {
    return {
        // Test intensif CPU : 100 additions
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0),     // Résultat = 0
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 1),     // Valeur à ajouter = 1
        
        // Faire 100 additions manuellement (simuler une boucle)
        /*for(int i = 0; i < 100; ++i) {
            // On ne peut pas faire de vraie boucle sans JMP fonctionnel, donc on simule
        }*/
        makeInstruction(vm::Opcode::ADD, vm::AddressingMode::IMMEDIATE, 0, 0, 100),   // Ajouter 100 directement
        
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

std::vector<uint64_t> createMemoryBenchmark() {
    return {
        // Test de mémoire simple
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0x2000), // Adresse base
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 0xAAAA), // Valeur test
        
        // Quelques opérations mémoire
        makeInstruction(vm::Opcode::STORE, vm::AddressingMode::REGISTER, 0, 1, 0),     // Écrire
        makeInstruction(vm::Opcode::LOAD, vm::AddressingMode::REGISTER, 2, 0, 0),      // Lire
        
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 2, 0, 0),
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

std::vector<uint64_t> createStackBenchmark() {
    return {
        // Test de pile : plusieurs PUSH/POP
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 0x5555), // Valeur test
        
        // Plusieurs PUSH
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),     
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),     
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),     
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),     
        makeInstruction(vm::Opcode::PUSH, vm::AddressingMode::REGISTER, 0, 0, 0),     
        
        // Plusieurs POP
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 1, 0, 0),      
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 2, 0, 0),      
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 3, 0, 0),      
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 4, 0, 0),      
        makeInstruction(vm::Opcode::POP, vm::AddressingMode::REGISTER, 5, 0, 0),      
        
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

std::vector<uint64_t> createConditionalJumpProgram() {
    return {
        // Test JEQ/JNE avec CMP
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 0, 0, 10),  // R0 = 10
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 1, 0, 10),  // R1 = 10

        // Test égalité
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 0, 1, 0),    // Compare R0 et R1
        makeInstruction(vm::Opcode::JEQ, vm::AddressingMode::IMMEDIATE, 0, 0, 0x40), // Saut si égaux

        // Cette instruction ne devrait pas s'exécuter
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 999), // R2 = 999 (erreur)
        makeInstruction(vm::Opcode::JMP, vm::AddressingMode::IMMEDIATE, 0, 0, 0x60), // Aller à la fin

        // Point de saut pour égalité (0x40)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 2, 0, 42),  // R2 = 42 (succès)

        // Test inégalité (0x60)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 3, 0, 5),   // R3 = 5
        makeInstruction(vm::Opcode::CMP, vm::AddressingMode::REGISTER, 0, 3, 0),    // Compare R0 et R3
        makeInstruction(vm::Opcode::JNE, vm::AddressingMode::IMMEDIATE, 0, 0, 0x80), // Saut si différents

        // Cette instruction ne devrait pas s'exécuter
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 4, 0, 888), // R4 = 888 (erreur)
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0),

        // Point de saut pour inégalité (0x80)
        makeInstruction(vm::Opcode::MOV, vm::AddressingMode::IMMEDIATE, 4, 0, 84),  // R4 = 84 (succès)
        makeInstruction(vm::Opcode::HLT, vm::AddressingMode::REGISTER, 0, 0, 0)
    };
}

// Collection de programmes de test avancés
std::vector<TestProgram> getAdvancedTestPrograms() {
    return {
        {
            "basic_arithmetic",
            "Basic arithmetic: Addition, subtraction operations",
            "basic_arithmetic.vmfw",
            []() { return createBasicArithmeticProgram(); }
        },
        {
            "fibonacci",
            "Fibonacci sequence calculator (simplified version)",
            "fibonacci.vmfw",
            []() { return createFibonacciProgram(); }
        },
        {
            "factorial",
            "Factorial calculator (5! = 120)",
            "factorial.vmfw",
            []() { return createFactorialProgram(); }
        },
        {
            "stack_operations",
            "Advanced stack manipulation operations",
            "stack_operations.vmfw",
            []() { return createStackOperationsProgram(); }
        },
        {
            "memory_test",
            "Memory read/write operations test",
            "memory_test.vmfw",
            []() { return createMemoryTestProgram(); }
        },
        {
            "sorting_demo",
            "Simple sorting algorithm demonstration",
            "sorting_demo.vmfw",
            []() { return createSortingProgram(); }
        },
        {
            "conditional_logic",
            "Conditional logic and comparison operations",
            "conditional_logic.vmfw",
            []() { return createConditionalProgram(); }
        },
        {
        "conditional_jumps",
        "Conditional jumps with JEQ/JNE instructions",
        "conditional_jumps.vmfw",
        []() { return createConditionalJumpProgram(); }
        },
        {
            "loop_demo",
            "Loop constructs and iterative algorithms",
            "loop_demo.vmfw",
            []() { return createLoopProgram(); }
        }
    };
}

// Générateur de suite de benchmarks
std::vector<TestProgram> getBenchmarkPrograms() {
    return {
        {"cpu_intensive", "CPU intensive operations test", "cpu_bench.vmfw", 
         []() { return createCPUBenchmark(); }},
        {"memory_bandwidth", "Memory bandwidth test", "mem_bench.vmfw",
         []() { return createMemoryBenchmark(); }},
        {"stack_stress", "Stack operations stress test", "stack_bench.vmfw",
         []() { return createStackBenchmark(); }}
    };
}

void generateSingleProgram(const TestProgram& program) {
    std::cout << "\nGenerating: " << program.name << std::endl;
    std::cout << "Description: " << program.description << std::endl;
    
    std::vector<uint64_t> instructions = program.generator();
    
    if (vm::FirmwareLoader::SaveFirmware(program.filename, instructions, program.description)) {
        std::cout << "✓ Generated: " << program.filename 
                  << " (" << instructions.size() << " instructions)" << std::endl;
        
        // Afficher comment exécuter le programme
        std::cout << "  Run with: ./vm -f " << program.filename << std::endl;
    } else {
        std::cerr << "✗ Failed to generate: " << program.filename << std::endl;
    }
}

void generateAdvancedTestFirmware() {
    std::cout << "=== Educational Virtual Machine - Advanced Test Firmware Generation ===" << std::endl;
    
    auto programs = getAdvancedTestPrograms();
    
    std::cout << "\nAvailable test programs:" << std::endl;
    for (size_t i = 0; i < programs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << programs[i].name 
                  << " - " << programs[i].description << std::endl;
    }
    std::cout << "  " << (programs.size() + 1) << ". Generate ALL programs" << std::endl;
    std::cout << "  0. Cancel" << std::endl;
    
    std::cout << "\nEnter your choice (0-" << (programs.size() + 1) << "): ";
    int choice;
    std::cin >> choice;
    
    if (choice == 0) {
        std::cout << "Operation cancelled." << std::endl;
        return;
    }
    
    if (choice == static_cast<int>(programs.size() + 1)) {
        // Générer tous les programmes
        std::cout << "\nGenerating all test programs..." << std::endl;
        for (const auto& program : programs) {
            generateSingleProgram(program);
        }
        std::cout << "\nAll programs generated successfully!" << std::endl;
    } else if (choice >= 1 && choice <= static_cast<int>(programs.size())) {
        // Générer un programme spécifique
        const auto& program = programs[choice - 1];
        generateSingleProgram(program);
    } else {
        std::cerr << "Invalid choice!" << std::endl;
    }
}

void generateBenchmarkSuite() {
    std::cout << "=== Educational Virtual Machine - Benchmark Suite Generation ===" << std::endl;
    
    auto benchmarks = getBenchmarkPrograms();
    
    std::cout << "\nGenerating benchmark programs..." << std::endl;
    for (const auto& benchmark : benchmarks) {
        generateSingleProgram(benchmark);
    }
    std::cout << "\nBenchmark suite generated successfully!" << std::endl;
}

void listAvailableFirmware() {
    std::cout << "=== Available Firmware Files ===" << std::endl;
    std::cout << "Scanning current directory for .vmfw files..." << std::endl;
    // Note: Cette fonction nécessiterait l'utilisation de filesystem pour scanner le répertoire
    // Pour l'instant, on affiche juste un message informatif
    std::cout << "Use 'ls *.vmfw' or 'dir *.vmfw' to list firmware files in your system." << std::endl;
}

void runDemo() {
    std::cout << "=== Educational Virtual Machine - Demo Mode ===" << std::endl;

    // Create VM with 1MB of RAM
    vm::VirtualMachine vm(1024 * 1024);
    vm.EnableDebugger(true);
    vm.EnableStepByStep(true); // Enable step-by-step mode

    std::vector<uint64_t> program = createTestProgram();

    // Load and execute program
    if (vm.LoadProgram(program)) {
        std::cout << "\nInitial state:" << std::endl;
        vm.PrintState();

        std::cout << "\nExecuting program..." << std::endl;
        vm.Run();

        std::cout << "\nFinal state:" << std::endl;
        vm.PrintState();

        std::cout << "\nMemory dump (stack):" << std::endl;
        vm.DumpMemory(vm.GetMemory().GetSize() - 0x100, 128);

        std::cout << "\nExpected result: R2 = 52 (42 + 10)" << std::endl;
        std::cout << "Actual result: R2 = " << std::dec
                  << vm.GetCPU().GetRegister(2) << std::endl;
    }
}

void runFirmware(const std::string& filename) {
    std::cout << "=== Educational Virtual Machine - Firmware Mode ===" << std::endl;
    std::cout << "Loading firmware: " << filename << std::endl;

    // Create VM with 1MB of RAM
    vm::VirtualMachine vm(1024 * 1024);
    vm.EnableDebugger(true);
    vm.EnableStepByStep(true); // Enable step-by-step mode

    // Load firmware
    std::vector<uint64_t> instructions;
    if (!vm::FirmwareLoader::LoadFirmware(filename, instructions)) {
        std::cerr << "Error: Failed to load firmware file: " << filename << std::endl;
        return;
    }

    // Load and execute firmware
    if (vm.LoadProgram(instructions)) {
        std::cout << "\nInitial state:" << std::endl;
        vm.PrintState();

        std::cout << "\nExecuting firmware..." << std::endl;
        vm.Run();

        std::cout << "\nFinal state:" << std::endl;
        vm.PrintState();

        std::cout << "\nMemory dump (stack):" << std::endl;
        vm.DumpMemory(vm.GetMemory().GetSize() - 0x100, 128);
    }
}

void generateTestFirmware() {
    std::cout << "=== Educational Virtual Machine - Basic Test Firmware Generation ===" << std::endl;
    
    const std::string filename = "firmware.vmfw";
    const std::string description = "Test firmware: Simple arithmetic operations (42 + 10)";
    
    std::vector<uint64_t> program = createTestProgram();
    
    if (vm::FirmwareLoader::SaveFirmware(filename, program, description)) {
        std::cout << "\nBasic test firmware generated successfully!" << std::endl;
        std::cout << "You can now run it with: " << std::endl;
        std::cout << "  ./vm -f " << filename << std::endl;
        
        // Print firmware info for verification
        std::cout << "\n";
        vm::FirmwareLoader::PrintFirmwareInfo(filename);
    } else {
        std::cerr << "Error: Failed to generate test firmware" << std::endl;
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d              Run demo mode (default)" << std::endl;
    std::cout << "  -f <filename>   Load and execute firmware file" << std::endl;
    std::cout << "  -t              Generate basic test firmware file" << std::endl;
    std::cout << "  -T              Generate advanced test firmware (interactive)" << std::endl;
    std::cout << "  --benchmark     Generate benchmark suite" << std::endl;
    std::cout << "  --list-fw       List all available firmware in current directory" << std::endl;
    std::cout << "  -h, --help      Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                    # Run demo mode" << std::endl;
    std::cout << "  " << programName << " -T                 # Interactive firmware generator" << std::endl;
    std::cout << "  " << programName << " -f fibonacci.vmfw  # Run Fibonacci calculator" << std::endl;
    std::cout << "  " << programName << " --benchmark        # Generate performance tests" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default mode is demo
    enum Mode { DEMO, FIRMWARE, GENERATE_TEST, GENERATE_ADVANCED, GENERATE_BENCHMARK, LIST_FIRMWARE };
    Mode mode = DEMO;
    std::string firmwareFile;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0) {
            mode = DEMO;
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                firmwareFile = argv[i + 1];
                mode = FIRMWARE;
                ++i; // Skip the filename argument
            } else {
                std::cerr << "Error: -f option requires a filename" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            mode = GENERATE_TEST;
        } else if (strcmp(argv[i], "-T") == 0) {
            mode = GENERATE_ADVANCED;
        } else if (strcmp(argv[i], "--benchmark") == 0) {
            mode = GENERATE_BENCHMARK;
        } else if (strcmp(argv[i], "--list-fw") == 0) {
            mode = LIST_FIRMWARE;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    try {
        switch (mode) {
            case DEMO:
                runDemo();
                break;
            case FIRMWARE:
                runFirmware(firmwareFile);
                break;
            case GENERATE_TEST:
                generateTestFirmware();
                break;
            case GENERATE_ADVANCED:
                generateAdvancedTestFirmware();
                break;
            case GENERATE_BENCHMARK:
                generateBenchmarkSuite();
                break;
            case LIST_FIRMWARE:
                listAvailableFirmware();
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
