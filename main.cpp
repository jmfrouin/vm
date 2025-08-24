//
// Created by Jean-Michel Frouin on 17/08/2025.
//
#include "src/vm/vm.h"
#include "src/vm/firmware_loader.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

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
    std::cout << "=== Educational Virtual Machine - Test Firmware Generation ===" << std::endl;
    
    const std::string filename = "firmware.vmfw";
    const std::string description = "Test firmware: Simple arithmetic operations (42 + 10)";
    
    std::vector<uint64_t> program = createTestProgram();
    
    if (vm::FirmwareLoader::SaveFirmware(filename, program, description)) {
        std::cout << "\nTest firmware generated successfully!" << std::endl;
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
    std::cout << "  -t              Generate test firmware file (firmware.vmfw)" << std::endl;
    std::cout << "  -h, --help      Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                    # Run demo mode" << std::endl;
    std::cout << "  " << programName << " -t                 # Generate test firmware" << std::endl;
    std::cout << "  " << programName << " -f firmware.vmfw   # Run firmware file" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default mode is demo
    enum Mode { DEMO, FIRMWARE, GENERATE_TEST };
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
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}