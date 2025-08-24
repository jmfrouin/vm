// src/vm/firmware_loader.h
#ifndef VM_FIRMWARE_LOADER_H
#define VM_FIRMWARE_LOADER_H

#include <common/types.h>
#include <string>
#include <vector>

namespace vm {
    // Firmware header structure
    struct FirmwareHeader {
        char mMagic[8];           // "VMFW001\0"
        uint32_t mVersion;        // Format version
        uint32_t mInstructionCount; // Number of instructions
        uint64_t mEntryPoint;     // Entry point
        uint64_t mTimestamp;      // Creation timestamp
        uint32_t mDescriptionSize; // Description size
        uint32_t mReserved;       // Reserved for future use
        
        FirmwareHeader() : mVersion(1), mInstructionCount(0), mEntryPoint(0), 
                          mTimestamp(0), mDescriptionSize(0), mReserved(0) {
            std::memcpy(mMagic, "VMFW001", 8);
        }
    };

    class FirmwareLoader {
    public:
        // Save a firmware
        static bool SaveFirmware(const std::string& filename, 
                                const std::vector<uint64_t>& instructions,
                                const std::string& description = "",
                                uint64_t entryPoint = 0);
        
        // Load a firmware
        static bool LoadFirmware(const std::string& filename,
                               std::vector<uint64_t>& instructions);
        
        // Display firmware information
        static void PrintFirmwareInfo(const std::string& filename);
        
        // Check if a file is valid firmware
        static bool IsValidFirmware(const std::string& filename);
        
        // Get firmware version
        static uint32_t GetFirmwareVersion(const std::string& filename);

    private:
        // Private utility methods
        static bool ValidateHeader(const FirmwareHeader& header);
        static std::string FormatTimestamp(uint64_t timestamp);
        static uint64_t GetCurrentTimestamp();
    };
}

#endif // VM_FIRMWARE_LOADER_H