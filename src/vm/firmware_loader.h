// src/vm/firmware_loader.h
#ifndef VM_FIRMWARE_LOADER_H
#define VM_FIRMWARE_LOADER_H

#include <common/types.h>
#include <string>
#include <vector>

namespace vm {
    // Structure d'en-tête du firmware
    struct FirmwareHeader {
        char mMagic[8];           // "VMFW001\0"
        uint32_t mVersion;        // Version du format
        uint32_t mInstructionCount; // Nombre d'instructions
        uint64_t mEntryPoint;     // Point d'entrée
        uint64_t mTimestamp;      // Timestamp de création
        uint32_t mDescriptionSize; // Taille de la description
        uint32_t mReserved;       // Réservé pour usage futur
        
        FirmwareHeader() : mVersion(1), mInstructionCount(0), mEntryPoint(0), 
                          mTimestamp(0), mDescriptionSize(0), mReserved(0) {
            std::memcpy(mMagic, "VMFW001", 8);
        }
    };

    class FirmwareLoader {
    public:
        // Sauvegarde un firmware
        static bool SaveFirmware(const std::string& filename, 
                                const std::vector<uint64_t>& instructions,
                                const std::string& description = "",
                                uint64_t entryPoint = 0);
        
        // Charge un firmware
        static bool LoadFirmware(const std::string& filename,
                               std::vector<uint64_t>& instructions);
        
        // Affiche les informations d'un firmware
        static void PrintFirmwareInfo(const std::string& filename);
        
        // Vérifie si un fichier est un firmware valide
        static bool IsValidFirmware(const std::string& filename);
        
        // Obtient la version d'un firmware
        static uint32_t GetFirmwareVersion(const std::string& filename);

    private:
        // Méthodes utilitaires privées
        static bool ValidateHeader(const FirmwareHeader& header);
        static std::string FormatTimestamp(uint64_t timestamp);
        static uint64_t GetCurrentTimestamp();
    };
}

#endif // VM