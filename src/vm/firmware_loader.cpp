//
// Created by Jean-Michel Frouin on 17/08/2025.
//
#include "firmware_loader.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstring>

namespace vm {
    bool FirmwareLoader::SaveFirmware(const std::string& filename,
                                     const std::vector<uint64_t>& instructions,
                                     const std::string& description,
                                     uint64_t entryPoint) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create firmware file: " << filename << std::endl;
            return false;
        }

        try {
            // Préparer l'en-tête
            FirmwareHeader header;
            header.mInstructionCount = static_cast<uint32_t>(instructions.size());
            header.mEntryPoint = entryPoint;
            header.mTimestamp = GetCurrentTimestamp();
            header.mDescriptionSize = static_cast<uint32_t>(description.length());

            // Écrire l'en-tête
            file.write(reinterpret_cast<const char*>(&header), sizeof(FirmwareHeader));

            // Écrire la description
            if (!description.empty()) {
                file.write(description.c_str(), description.length());
            }

            // Écrire les instructions
            for (uint64_t instruction : instructions) {
                file.write(reinterpret_cast<const char*>(&instruction), sizeof(uint64_t));
            }

            file.close();
            
            std::cout << "Firmware saved successfully: " << filename << std::endl;
            std::cout << "Instructions: " << instructions.size() << std::endl;
            std::cout << "Entry point: 0x" << std::hex << entryPoint << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error writing firmware file: " << e.what() << std::endl;
            file.close();
            return false;
        }
    }

    bool FirmwareLoader::LoadFirmware(const std::string& filename,
                                    std::vector<uint64_t>& instructions) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open firmware file: " << filename << std::endl;
            return false;
        }

        try {
            // Lire l'en-tête
            FirmwareHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(FirmwareHeader));
            
            if (!file.good()) {
                std::cerr << "Error: Cannot read firmware header" << std::endl;
                return false;
            }

            // Valider l'en-tête
            if (!ValidateHeader(header)) {
                std::cerr << "Error: Invalid firmware header" << std::endl;
                return false;
            }

            // Lire la description (mais on l'ignore pour le chargement)
            if (header.mDescriptionSize > 0) {
                file.seekg(header.mDescriptionSize, std::ios::cur);
            }

            // Lire les instructions
            instructions.clear();
            instructions.reserve(header.mInstructionCount);

            for (uint32_t i = 0; i < header.mInstructionCount; ++i) {
                uint64_t instruction;
                file.read(reinterpret_cast<char*>(&instruction), sizeof(uint64_t));
                
                if (!file.good()) {
                    std::cerr << "Error: Cannot read instruction " << i << std::endl;
                    return false;
                }
                
                instructions.push_back(instruction);
            }

            file.close();
            
            std::cout << "Firmware loaded successfully: " << filename << std::endl;
            std::cout << "Instructions: " << instructions.size() << std::endl;
            std::cout << "Entry point: 0x" << std::hex << header.mEntryPoint << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error reading firmware file: " << e.what() << std::endl;
            file.close();
            return false;
        }
    }

    void FirmwareLoader::PrintFirmwareInfo(const std::string& filename) {
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
        std::cout << "Signature: " << std::string(header.mVersion, 4) << std::endl;
        std::cout << "Version: " << header.mVersion << std::endl;
        std::cout << "Instructions: " << header.mInstructionCount << std::endl;
        std::cout << "Entry Point: " << header.mEntryPoint << std::endl;
        std::cout << "Description Size: " << header.mDescriptionSize << std::endl;
        
        if (file.gcount() == 1 && separator == '|') {
            std::cout << "Separator: Found ('|')" << std::endl;
        } else {
            std::cout << "Separator: Missing or invalid" << std::endl;
        }

        file.close();
    }

    bool FirmwareLoader::ValidateHeader(const FirmwareHeader& header) {
        // Vérifier la signature magique
        if (std::memcmp(header.mMagic, "VMFW001", 7) != 0) {
            return false;
        }

        // Vérifier la version (on supporte seulement la version 1 pour l'instant)
        if (header.mVersion != 1) {
            return false;
        }

        // Vérifier que le nombre d'instructions est raisonnable
        if (header.mInstructionCount == 0 || header.mInstructionCount > 1000000) {
            return false;
        }

        // Vérifier que la taille de description est raisonnable
        if (header.mDescriptionSize > 10000) {
            return false;
        }

        return true;
    }

    uint64_t FirmwareLoader::GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        return static_cast<uint64_t>(timestamp);
    }
}