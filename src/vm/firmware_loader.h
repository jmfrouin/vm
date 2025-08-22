
//
// Created by Jean-Michel Frouin on 22/08/2025.
//
#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace vm {
    struct FirmwareHeader {
        char signature[4] = {'V', 'M', 'F', 'W'}; // "VMFW"
        uint32_t version = 1;
        uint32_t instructionCount = 0;
        uint32_t entryPoint = 0;
        char description[64] = {};
    };

    class FirmwareLoader {
    public:
        static bool saveFirmware(const std::string& filename, 
                               const std::vector<uint64_t>& instructions,
                               const std::string& description = "");
        
        static bool loadFirmware(const std::string& filename, 
                               std::vector<uint64_t>& instructions);
        
        static void printFirmwareInfo(const std::string& filename);
    };
}
