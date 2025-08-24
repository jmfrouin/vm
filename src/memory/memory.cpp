//
// Created by Jean-Michel Frouin on 17/08/2025.
//

// src/memory/memory.cpp
#include "memory.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace vm {
    Memory::Memory(size_t memSize) : mSize(memSize) {
        mRam.resize(memSize, 0);

        // Segments par défaut
        AddSegment(MemorySegment(0x000000, 0x100000,
                   static_cast<AccessType>(static_cast<uint8_t>(AccessType::READ) |
                                         static_cast<uint8_t>(AccessType::WRITE) |
                                         static_cast<uint8_t>(AccessType::EXECUTE)),
                   "CODE"));
        AddSegment(MemorySegment(0x100000, 0x100000,
                   static_cast<AccessType>(static_cast<uint8_t>(AccessType::READ) |
                                         static_cast<uint8_t>(AccessType::WRITE)),
                   "DATA"));
        AddSegment(MemorySegment(0x200000, 0x100000,
                   static_cast<AccessType>(static_cast<uint8_t>(AccessType::READ) |
                                         static_cast<uint8_t>(AccessType::WRITE)),
                   "HEAP"));
        AddSegment(MemorySegment(memSize - 0x100000, 0x100000,
                   static_cast<AccessType>(static_cast<uint8_t>(AccessType::READ) |
                                         static_cast<uint8_t>(AccessType::WRITE)),
                   "STACK"));
    }

    bool Memory::IsValidAddress(uint64_t addr) const {
        return addr < mSize;
    }

    bool Memory::CheckAccess(uint64_t addr, AccessType type) const {
        for (const auto& segment : mSegments) {
            if (addr >= segment.base && addr < segment.base + segment.size) {
                return (static_cast<uint8_t>(segment.permissions) & static_cast<uint8_t>(type)) != 0;
            }
        }
        return false;
    }

    uint8_t Memory::Read8(uint64_t addr) {
        if (!IsValidAddress(addr)) {
            std::stringstream temp;
            temp << "Invalid memory address: 0x" << std::hex << addr;
            throw std::runtime_error(temp.str());
        }
        if (!CheckAccess(addr, AccessType::READ)) {
            throw std::runtime_error("Memory access violation (read) at: 0x" + std::to_string(addr));
        }
        return mRam[addr];
    }

    uint16_t Memory::Read16(uint64_t addr) {
        return static_cast<uint16_t>(Read8(addr)) |
               (static_cast<uint16_t>(Read8(addr + 1)) << 8);
    }

    uint32_t Memory::Read32(uint64_t addr) {
        return static_cast<uint32_t>(Read16(addr)) |
               (static_cast<uint32_t>(Read16(addr + 2)) << 16);
    }

    uint64_t Memory::Read64(uint64_t addr) {
        return static_cast<uint64_t>(Read32(addr)) |
               (static_cast<uint64_t>(Read32(addr + 4)) << 32);
    }

    void Memory::Write8(uint64_t addr, uint8_t value) {
        if (!IsValidAddress(addr)) {
            std::stringstream temp;
            temp << "Invalid memory address: 0x" << std::hex << addr;
            throw std::runtime_error(temp.str());
        }
        if (!CheckAccess(addr, AccessType::WRITE)) {
            throw std::runtime_error("Memory access violation (write) at: 0x" + std::to_string(addr));
        }
        mRam[addr] = value;
    }

    void Memory::Write16(uint64_t addr, uint16_t value) {
        Write8(addr, value & 0xFF);
        Write8(addr + 1, (value >> 8) & 0xFF);
    }

    void Memory::Write32(uint64_t addr, uint32_t value) {
        Write16(addr, value & 0xFFFF);
        Write16(addr + 2, (value >> 16) & 0xFFFF);
    }

    void Memory::Write64(uint64_t addr, uint64_t value) {
        Write32(addr, value & 0xFFFFFFFF);
        Write32(addr + 4, (value >> 32) & 0xFFFFFFFF);
    }

    void Memory::AddSegment(const MemorySegment& segment) {
        mSegments.push_back(segment);
    }

    bool Memory::CheckPermissions(uint64_t addr, AccessType type) const {
        return CheckAccess(addr, type);
    }

    void Memory::Clear() {
        std::fill(mRam.begin(), mRam.end(), 0);
    }

    void Memory::Dump(uint64_t start, uint64_t length) const {
        std::cout << "\n=== Memory Dump ===" << std::endl;
        for (uint64_t i = 0; i < length && (start + i) < mSize; i += 16) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8) << (start + i) << ": ";

            // Affichage hexadécimal
            for (int j = 0; j < 16 && (start + i + j) < mSize; ++j) {
                std::cout << std::hex << std::setfill('0') << std::setw(2)
                         << static_cast<int>(mRam[start + i + j]) << " ";
            }

            // Espacement
            std::cout << " | ";

            // Affichage ASCII
            for (int j = 0; j < 16 && (start + i + j) < mSize; ++j) {
                char c = mRam[start + i + j];
                std::cout << (isprint(c) ? c : '.');
            }
            std::cout << std::endl;
        }
        std::cout << "===================" << std::endl;
    }
}