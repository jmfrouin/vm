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
        std::vector<uint8_t> mRam;
        size_t mSize;
        std::vector<MemorySegment> mSegments;

        bool IsValidAddress(uint64_t addr) const;
        bool CheckAccess(uint64_t addr, AccessType type) const;

    public:
        Memory(size_t memSize);
        ~Memory() = default;

        // Lecture/écriture de base
        uint8_t Read8(uint64_t addr);
        uint16_t Read16(uint64_t addr);
        uint32_t Read32(uint64_t addr);
        uint64_t Read64(uint64_t addr);

        void Write8(uint64_t addr, uint8_t value);
        void Write16(uint64_t addr, uint16_t value);
        void Write32(uint64_t addr, uint32_t value);
        void Write64(uint64_t addr, uint64_t value);

        // Gestion des segments
        void AddSegment(const MemorySegment& segment);
        bool CheckPermissions(uint64_t addr, AccessType type) const;

        // Utilitaires
        void Clear();
        void Dump(uint64_t start, uint64_t length) const;
        size_t GetSize() const { return mSize; }
    };
}

#endif // VM_MEMORY_H