//
// Created by Jean-Michel Frouin on 22/08/2025.
//
#include "firmware_loader.h"
#include <fstream>
#include <iostream>
#include <cstring>

namespace vm {
    bool FirmwareLoader::saveFirmware(const std::string& filename,
                                    const std::vector<uint64_t>& instructions,
                                    const std::string& description) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " << filename << " for writing" << std::endl;
            return false;
        }

        FirmwareHeader header;
        header.instructionCount = static_cast<uint32_t>(instructions.size());

        // Copy description safely
        if (!description.empty()) {
            size_t len = std::min(description.length(), sizeof(header.description) - 1);
            std::strncpy(header.description, description.c_str(), len);
            header.description[len] = '\0';
        }

        // Write header
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));

        // Write separator character between header and instructions
        char separator = '|';
        file.write(&separator, 1);

        // Write instructions
        file.write(reinterpret_cast<const char*>(instructions.data()),
                  instructions.size() * sizeof(uint64_t));

        file.close();

        std::cout << "Firmware saved: " << filename << std::endl;
        std::cout << "  Instructions: " << instructions.size() << std::endl;
        std::cout << "  Description: " << (description.empty() ? "None" : description) << std::endl;

        return true;
    }

    bool FirmwareLoader::loadFirmware(const std::string& filename,
                                    std::vector<uint64_t>& instructions) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " << filename << std::endl;
            return false;
        }

        FirmwareHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (file.gcount() != sizeof(header)) {
            std::cerr << "Error: incomplete or corrupted firmware file" << std::endl;
            return false;
        }

        // Check signature
        if (std::strncmp(header.signature, "VMFW", 4) != 0) {
            std::cerr << "Error: invalid firmware signature" << std::endl;
            return false;
        }

        // Check version
        if (header.version != 1) {
            std::cerr << "Error: unsupported firmware version: " << header.version << std::endl;
            return false;
        }

        // Read and verify separator character
        char separator;
        file.read(&separator, 1);
        if (file.gcount() != 1 || separator != '|') {
            std::cerr << "Error: missing or invalid separator character" << std::endl;
            return false;
        }

        instructions.clear();
        instructions.resize(header.instructionCount);

        file.read(reinterpret_cast<char*>(instructions.data()),
                 header.instructionCount * sizeof(uint64_t));

        if (file.gcount() != static_cast<std::streamsize>(header.instructionCount * sizeof(uint64_t))) {
            std::cerr << "Error: unable to read all instructions" << std::endl;
            return false;
        }

        file.close();

        std::cout << "Firmware loaded: " << filename << std::endl;
        std::cout << "  Version: " << header.version << std::endl;
        std::cout << "  Instructions: " << header.instructionCount << std::endl;
        std::cout << "  Description: " << header.description << std::endl;

        return true;
    }

    void FirmwareLoader::printFirmwareInfo(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " << filename << std::endl;
            return;
        }

        FirmwareHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (file.gcount() != sizeof(header)) {
            std::cerr << "Error: incomplete firmware file" << std::endl;
            return;
        }

        // Read separator character for verification
        char separator;
        file.read(&separator, 1);
        
        std::cout << "=== Firmware Information ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "Signature: " << std::string(header.signature, 4) << std::endl;
        std::cout << "Version: " << header.version << std::endl;
        std::cout << "Instructions: " << header.instructionCount << std::endl;
        std::cout << "Entry Point: " << header.entryPoint << std::endl;
        std::cout << "Description: " << header.description << std::endl;
        
        if (file.gcount() == 1 && separator == '|') {
            std::cout << "Separator: Found ('|')" << std::endl;
        } else {
            std::cout << "Separator: Missing or invalid" << std::endl;
        }

        file.close();
    }
}