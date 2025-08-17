//
// Created by Jean-Michel Frouin on 17/08/2025.
//

#ifndef VM_MEMORY_H
#define VM_MEMORY_H

#include <common/types.h>
#include <vector>
#include <map>

namespace vm {
    struct MemorySegment {
        uint64_t base;
        uint64_t size;
        AccessType permissions;
        std::string name;

        MemorySegment(uint64_t b, uint64_t s, AccessType p, const std::string& n)
            : base(b), size(s), permissions(p), name(n) {}
    };

    class Memory {
    private:
        std::vector<uint8_t> ram;
        size_t size;
        std::vector<MemorySegment> segments;

        bool isValidAddress(uint64_t addr) const;
        bool checkAccess(uint64_t addr, AccessType type) const;

    public:
        Memory(size_t memSize);
        ~Memory() = default;

        // Lecture/écriture de base
        uint8_t read8(uint64_t addr);
        uint16_t read16(uint64_t addr);
        uint32_t read32(uint64_t addr);
        uint64_t read64(uint64_t addr);

        void write8(uint64_t addr, uint8_t value);
        void write16(uint64_t addr, uint16_t value);
        void write32(uint64_t addr, uint32_t value);
        void write64(uint64_t addr, uint64_t value);

        // Gestion des segments
        void addSegment(const MemorySegment& segment);
        bool checkPermissions(uint64_t addr, AccessType type) const;

        // Utilitaires
        void clear();
        void dump(uint64_t start, uint64_t length) const;
        size_t getSize() const { return size; }
    };
}

#endif // VM_MEMORY_H