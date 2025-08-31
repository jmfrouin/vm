//
// Created by Jean-Michel Frouin on 17/08/2025.
//

#include "cpu.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

namespace vm {
    // Utility function to convert opcode to string
    const char* OpcodeToString(Opcode opcode) {
        switch (opcode) {
            case Opcode::MOV:   return "MOV";
            case Opcode::LOAD:  return "LOAD";
            case Opcode::STORE: return "STORE";
            case Opcode::PUSH:  return "PUSH";
            case Opcode::POP:   return "POP";
            case Opcode::HLT:   return "HLT";

            case Opcode::ADD:   return "ADD";
            case Opcode::SUB:   return "SUB";
            case Opcode::MUL:   return "MUL";
            case Opcode::DIV:   return "DIV";
            case Opcode::MOD:   return "MOD";
            case Opcode::INC:   return "INC";
            case Opcode::DEC:   return "DEC";
            case Opcode::CMP:   return "CMP";
            case Opcode::SWAP:  return "SWAP";

            case Opcode::AND:   return "AND";
            case Opcode::OR:    return "OR";
            case Opcode::XOR:   return "XOR";
            case Opcode::NOT:   return "NOT";
            case Opcode::SHL:   return "SHL";
            case Opcode::SHR:   return "SHR";

            case Opcode::JMP:   return "JMP";
            case Opcode::JZ:    return "JZ";
            case Opcode::JNZ:   return "JNZ";
            case Opcode::JEQ:   return "JEQ";
            case Opcode::JNE:   return "JNE";
            case Opcode::JC:    return "JC";
            case Opcode::JNC:   return "JNC";
            case Opcode::JL:    return "JL";
            case Opcode::JLE:   return "JLE";
            case Opcode::JG:    return "JG";
            case Opcode::JGE:   return "JGE";
            case Opcode::LOOP:  return "LOOP";
            case Opcode::CALL:  return "CALL";
            case Opcode::RET:   return "RET";
            case Opcode::NOP:   return "NOP";

            case Opcode::PRINT: return "PRINT";
            case Opcode::IN:    return "IN";
            case Opcode::OUT:   return "OUT";

            default:            return "UNKNOWN";
        }
    }

    CPU::CPU(Memory* mem) : mMemory(mem), mRunning(false), mDebug(false), mStepByStep(false) {
        Reset();
    }

    void CPU::Reset() {
        mRegisters.fill(0);
        mPC = 0;
        // Adapter le SP à la taille réelle de la mémoire
        mSP = mMemory->GetSize() - 16; // Laisser 16 octets de marge
        mFlags = 0;
        mRunning = false;
    }

    void CPU::ClearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void CPU::WaitForKey() const {
        std::cout << "\n📝 Press Enter to continue...";
        std::cin.get();
    }

    void CPU::Step() {
        if (!mRunning) return;

        if (mDebug && mStepByStep) {
            ClearScreen(); // Clear screen before each step
        }

        Instruction instr;
        FetchInstruction(instr);

        if (mDebug) {
            std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║                    EXECUTION STEP                          ║" << std::endl;
            std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
            
            std::cout << "📍 PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << (mPC - 8) << " ";
            std::cout << "📦 SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << mSP << std::endl;
            std::cout << "🔧 Instruction: " << OpcodeToString(instr.opcode) 
                      << " (0x" << std::hex << std::setfill('0') << std::setw(2) 
                      << static_cast<int>(instr.opcode) << ")" << std::endl;
            
            // Display state before execution
            std::cout << "\n┌─ State BEFORE execution ─┐" << std::endl;
            PrintState();
        }

        ExecuteInstruction(instr);

        if (mDebug) {
            std::cout << "\n┌─ State AFTER execution ─┐" << std::endl;
            PrintState();
            std::cout << "\n" << std::string(60, '=') << std::endl;
            
            if (mStepByStep && mRunning) {
                WaitForKey();
            }
        }
    }

    void CPU::Run() {
        mRunning = true;
        while (mRunning) {
            Step();
        }
    }

    void CPU::FetchInstruction(Instruction& instr) {
        // Read instruction from memory
        uint64_t raw_instr = mMemory->Read64(mPC);

        instr.opcode = static_cast<Opcode>((raw_instr >> 56) & 0xFF);
        instr.mode = static_cast<AddressingMode>((raw_instr >> 52) & 0xF);
        instr.reg1 = (raw_instr >> 48) & 0xF;
        instr.reg2 = (raw_instr >> 44) & 0xF;
        instr.immediate = raw_instr & 0xFFFFFFFF;

        mPC += 8; // 64-bit instruction
    }

    void CPU::ExecuteInstruction(const Instruction& instr) {
        switch (instr.opcode) {
            case Opcode::MOV:   ExecuteMov(instr); break;
            case Opcode::LOAD:  ExecuteLoad(instr); break;
            case Opcode::STORE: ExecuteStore(instr); break;
            case Opcode::PUSH:  ExecutePush(instr); break;
            case Opcode::POP:   ExecutePop(instr); break;

            case Opcode::ADD:   ExecuteAdd(instr); break;
            case Opcode::SUB:   ExecuteSub(instr); break;
            case Opcode::MUL:   ExecuteMul(instr); break;
            case Opcode::DIV:   ExecuteDiv(instr); break;
            case Opcode::MOD:   ExecuteMod(instr); break;
            case Opcode::INC:   ExecuteInc(instr); break;
            case Opcode::DEC:   ExecuteDec(instr); break;
            case Opcode::CMP:   ExecuteCmp(instr); break;
            case Opcode::SWAP:  ExecuteSwap(instr); break;

            case Opcode::AND:   ExecuteAnd(instr); break;
            case Opcode::OR:    ExecuteOr(instr); break;
            case Opcode::XOR:   ExecuteXor(instr); break;
            case Opcode::NOT:   ExecuteNot(instr); break;
            case Opcode::SHL:   ExecuteShl(instr); break;
            case Opcode::SHR:   ExecuteShr(instr); break;

            case Opcode::JMP:   ExecuteJmp(instr); break;
            case Opcode::JZ:    ExecuteJz(instr); break;
            case Opcode::JNZ:   ExecuteJnz(instr); break;
            case Opcode::JEQ:   ExecuteJeq(instr); break;
            case Opcode::JNE:   ExecuteJne(instr); break;
            case Opcode::JC:    ExecuteJc(instr); break;
            case Opcode::JNC:   ExecuteJnc(instr); break;
            case Opcode::JL:    ExecuteJl(instr); break;
            case Opcode::JLE:   ExecuteJle(instr); break;
            case Opcode::JG:    ExecuteJg(instr); break;
            case Opcode::JGE:   ExecuteJge(instr); break;

            case Opcode::LOOP:  ExecuteLoop(instr); break;
            case Opcode::CALL:  ExecuteCall(instr); break;
            case Opcode::RET:   ExecuteRet(instr); break;

            case Opcode::HLT:   ExecuteHlt(instr); break;
            case Opcode::PRINT: ExecutePrint(instr); break;
            case Opcode::IN:    ExecuteIn(instr); break;
            case Opcode::OUT:   ExecuteOut(instr); break;
            case Opcode::NOP:   break; // Do nothing
            default:
                std::cerr << "❌ Unimplemented instruction: " << OpcodeToString(instr.opcode)
                         << " (0x" << std::hex << static_cast<int>(instr.opcode) << ")" << std::endl;
                Halt();
        }
    }

    void CPU::ExecuteMov(const Instruction& instr) {
        uint64_t value = GetOperandValue(instr, true); // Source
        SetOperandValue(instr, value, false); // Destination
    }

    void CPU::ExecuteLoad(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr, true);
        uint64_t value = mMemory->Read64(address);
        mRegisters[instr.reg1] = value;
    }

    void CPU::ExecuteStore(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr, false);
        uint64_t value = mRegisters[instr.reg2];
        mMemory->Write64(address, value);
    }

    void CPU::ExecutePush(const Instruction& instr) {
        uint64_t value = GetOperandValue(instr);
        mSP -= 8;
        mMemory->Write64(mSP, value);
    }

    void CPU::ExecutePop(const Instruction& instr) {
        uint64_t value = mMemory->Read64(mSP);
        mRegisters[instr.reg1] = value;
        mSP += 8;
    }

    void CPU::ExecuteAdd(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 + op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, result < op1); // Carry detection
    }

    void CPU::ExecuteSub(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 - op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, op1 < op2); // Borrow detection
    }

    void CPU::ExecuteCmp(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 - op2;

        // CMP ne modifie pas les registres, seulement les flags
        UpdateFlags(result, op1 < op2); // Mise à jour des flags selon le résultat de la soustraction
        
        if (mDebug) {
            std::cout << "🔍 CMP R" << static_cast<int>(instr.reg1) 
                      << " (0x" << std::hex << op1 << ") with 0x" << op2
                      << " → flags: Z=" << GetFlag(FlagType::ZERO) 
                      << " C=" << GetFlag(FlagType::CARRY) 
                      << " N=" << GetFlag(FlagType::NEGATIVE) << std::endl;
        }
    }

    void CPU::ExecuteJmp(const Instruction& instr) {
        uint64_t address = GetOperandValue(instr);
        mPC = address;
        
        if (mDebug) {
            std::cout << "🚀 JMP to address 0x" << std::hex << address << std::endl;
        }
    }

    void CPU::ExecuteJz(const Instruction& instr) {
        if (GetFlag(FlagType::ZERO)) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;
            
            if (mDebug) {
                std::cout << "✅ JZ taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JZ not taken (ZERO flag not set)" << std::endl;
            }
        }
    }

    void CPU::ExecuteJnz(const Instruction& instr) {
        if (!GetFlag(FlagType::ZERO)) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;
            
            if (mDebug) {
                std::cout << "✅ JNZ taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JNZ not taken (ZERO flag is set)" << std::endl;
            }
        }
    }

    void CPU::ExecuteJeq(const Instruction& instr) {
        // JEQ est équivalent à JZ (sauter si égal = sauter si zéro)
        ExecuteJz(instr);
        
        if (mDebug) {
            std::cout << "🎯 JEQ = JZ (jump if equal)" << std::endl;
        }
    }

    void CPU::ExecuteJne(const Instruction& instr) {
        // JNE est équivalent à JNZ (sauter si pas égal = sauter si pas zéro)
        ExecuteJnz(instr);
        
        if (mDebug) {
            std::cout << "🎯 JNE = JNZ (jump if not equal)" << std::endl;
        }
    }

    void CPU::ExecuteCall(const Instruction& instr) {
        // Save return address
        mSP -= 8;
        mMemory->Write64(mSP, mPC);

        // Jump to function
        uint64_t address = GetOperandValue(instr);
        mPC = address;
        
        if (mDebug) {
            std::cout << "📞 CALL to address 0x" << std::hex << address << std::endl;
        }
    }

    void CPU::ExecuteRet(const Instruction&) {
        // Restore return address
        uint64_t return_address = mMemory->Read64(mSP);
        mSP += 8;
        mPC = return_address;
        
        if (mDebug) {
            std::cout << "🔙 RET to address 0x" << std::hex << return_address << std::endl;
        }
    }

    void CPU::ExecuteHlt(const Instruction&) {
        mRunning = false;
        if (mDebug) {
            std::cout << "\n🛑 CPU STOPPED (HLT)" << std::endl;
        }
    }

    void CPU::ExecuteInc(const Instruction& instr) {
        uint64_t value = mRegisters[instr.reg1];
        uint64_t result = value + 1;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, result < value); // Détection de carry

        if (mDebug) {
            std::cout << "⬆️ INC R" << static_cast<int>(instr.reg1)
                      << ": 0x" << std::hex << value << " → 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteDec(const Instruction& instr) {
        uint64_t value = mRegisters[instr.reg1];
        uint64_t result = value - 1;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, value == 0); // Détection de borrow

        if (mDebug) {
            std::cout << "⬇️ DEC R" << static_cast<int>(instr.reg1)
                      << ": 0x" << std::hex << value << " → 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteMul(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);

        // Détection d'overflow pour la multiplication
        uint64_t result = op1 * op2;
        bool overflow = (op2 != 0) && (result / op2 != op1);

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, false, overflow);

        if (mDebug) {
            std::cout << "✖️ MUL R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") * 0x" << op2
                      << " = 0x" << result;
            if (overflow) std::cout << " [OVERFLOW!]";
            std::cout << std::endl;
        }
    }

    void CPU::ExecuteDiv(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);

        // Protection contre division par zéro
        if (op2 == 0) {
            if (mDebug) {
                std::cerr << "⚠️ DIV: Division by zero! R" << static_cast<int>(instr.reg1)
                          << " (0x" << std::hex << op1 << ") / 0" << std::endl;
            }
            // Comportement en cas de division par zéro : arrêter la VM
            Halt();
            return;
        }

        uint64_t result = op1 / op2;
        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "➗ DIV R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") / 0x" << op2
                      << " = 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteMod(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);

        // Protection contre modulo par zéro
        if (op2 == 0) {
            if (mDebug) {
                std::cerr << "⚠️ MOD: Modulo by zero! R" << static_cast<int>(instr.reg1)
                          << " (0x" << std::hex << op1 << ") % 0" << std::endl;
            }
            // Comportement en cas de modulo par zéro : arrêter la VM
            Halt();
            return;
        }

        uint64_t result = op1 % op2;
        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "🔢 MOD R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") % 0x" << op2
                      << " = 0x" << result << std::endl;
        }
    }


    uint64_t CPU::GetOperandValue(const Instruction& instr, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                return mRegisters[reg];
            case AddressingMode::IMMEDIATE:
                return instr.immediate;
            case AddressingMode::MEMORY:
                return mMemory->Read64(instr.immediate);
            case AddressingMode::REGISTER_INDIRECT:
                return mMemory->Read64(mRegisters[reg]);
            default:
                return 0;
        }
    }

    void CPU::ExecuteAnd(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 & op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "🔗 AND R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") & 0x" << op2
                      << " = 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteOr(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 | op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "🔀 OR R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") | 0x" << op2
                      << " = 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteXor(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t op2 = GetOperandValue(instr, true);
        uint64_t result = op1 ^ op2;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "⚡ XOR R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") ^ 0x" << op2
                      << " = 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteNot(const Instruction& instr) {
        uint64_t value = mRegisters[instr.reg1];
        uint64_t result = ~value;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result);

        if (mDebug) {
            std::cout << "🚫 NOT R" << static_cast<int>(instr.reg1)
                      << " (~0x" << std::hex << value << ") = 0x" << result << std::endl;
        }
    }

    void CPU::ExecuteShl(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t shift_amount = GetOperandValue(instr, true) & 0x3F; // Limiter à 63
        uint64_t result = op1 << shift_amount;

        // Carry flag = dernier bit décalé
        bool carry = shift_amount > 0 ? (op1 >> (64 - shift_amount)) & 1 : false;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, carry);

        if (mDebug) {
            std::cout << "⬅️ SHL R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") << " << std::dec << shift_amount
                      << " = 0x" << std::hex << result << std::endl;
        }
    }

    void CPU::ExecuteShr(const Instruction& instr) {
        uint64_t op1 = mRegisters[instr.reg1];
        uint64_t shift_amount = GetOperandValue(instr, true) & 0x3F; // Limiter à 63
        uint64_t result = op1 >> shift_amount;

        // Carry flag = dernier bit décalé
        bool carry = shift_amount > 0 ? (op1 >> (shift_amount - 1)) & 1 : false;

        mRegisters[instr.reg1] = result;
        UpdateFlags(result, carry);

        if (mDebug) {
            std::cout << "➡️ SHR R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << op1 << ") >> " << std::dec << shift_amount
                      << " = 0x" << std::hex << result << std::endl;
        }
    }

    void CPU::ExecuteJc(const Instruction& instr) {
        if (GetFlag(FlagType::CARRY)) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JC taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JC not taken (CARRY flag not set)" << std::endl;
            }
        }
    }

    void CPU::ExecuteJnc(const Instruction& instr) {
        if (!GetFlag(FlagType::CARRY)) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JNC taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JNC not taken (CARRY flag is set)" << std::endl;
            }
        }
    }

    void CPU::ExecuteJl(const Instruction& instr) {
        // JL: sauter si moins (signed comparison)
        // Condition: NEGATIVE != OVERFLOW
        bool condition = GetFlag(FlagType::NEGATIVE) != GetFlag(FlagType::OF);

        if (condition) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JL taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JL not taken" << std::endl;
            }
        }
    }

    void CPU::ExecuteJle(const Instruction& instr) {
        // JLE: sauter si moins ou égal (signed comparison)
        // Condition: ZERO || (NEGATIVE != OVERFLOW)
        bool condition = GetFlag(FlagType::ZERO) ||
                        (GetFlag(FlagType::NEGATIVE) != GetFlag(FlagType::OF));

        if (condition) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JLE taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JLE not taken" << std::endl;
            }
        }
    }

    void CPU::ExecuteJg(const Instruction& instr) {
        // JG: sauter si plus grand (signed comparison)
        // Condition: !ZERO && (NEGATIVE == OVERFLOW)
        bool condition = !GetFlag(FlagType::ZERO) &&
                        (GetFlag(FlagType::NEGATIVE) == GetFlag(FlagType::OF));

        if (condition) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JG taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JG not taken" << std::endl;
            }
        }
    }

    void CPU::ExecuteJge(const Instruction& instr) {
        // JGE: sauter si plus grand ou égal (signed comparison)
        // Condition: NEGATIVE == OVERFLOW
        bool condition = GetFlag(FlagType::NEGATIVE) == GetFlag(FlagType::OF);

        if (condition) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "✅ JGE taken to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "❌ JGE not taken" << std::endl;
            }
        }
    }

    void CPU::ExecuteIn(const Instruction& instr) {
        // IN: Lecture depuis un port (simulation simple)
        uint64_t port = GetOperandValue(instr, true);
        uint64_t value = 0;

        // Simulation basique des ports
        switch (port) {
            case 0: // Port clavier (simulation)
                std::cout << "📥 Input from keyboard: ";
                std::cin >> value;
                break;
            case 1: // Port timer (simulation)
                value = static_cast<uint64_t>(std::time(nullptr)) & 0xFFFFFFFF;
                break;
            default:
                value = 0; // Port non supporté
                if (mDebug) {
                    std::cout << "⚠️ Unsupported port: " << port << std::endl;
                }
        }

        mRegisters[instr.reg1] = value;
        UpdateFlags(value);

        if (mDebug) {
            std::cout << "📥 IN from port " << std::dec << port
                      << " → R" << static_cast<int>(instr.reg1)
                      << " = 0x" << std::hex << value << std::endl;
        }
    }

    void CPU::ExecuteOut(const Instruction& instr) {
        // OUT: Écriture vers un port (simulation simple)
        uint64_t port = instr.immediate;
        uint64_t value = mRegisters[instr.reg1];

        // Simulation basique des ports
        switch (port) {
            case 0: // Port écran (simulation)
                std::cout << "📺 Screen output: " << std::dec << value
                          << " (char: '" << static_cast<char>(value & 0xFF) << "')" << std::endl;
                break;
            case 1: // Port série (simulation)
                std::cout << "📡 Serial output: 0x" << std::hex << value << std::endl;
                break;
            default:
                if (mDebug) {
                    std::cout << "⚠️ Unsupported output port: " << std::dec << port << std::endl;
                }
        }

        if (mDebug) {
            std::cout << "📤 OUT R" << static_cast<int>(instr.reg1)
                      << " (0x" << std::hex << value << ") to port "
                      << std::dec << port << std::endl;
        }
    }

    void CPU::ExecuteSwap(const Instruction& instr) {
        uint64_t temp = mRegisters[instr.reg1];
        mRegisters[instr.reg1] = mRegisters[instr.reg2];
        mRegisters[instr.reg2] = temp;

        // Mettre à jour les flags selon la valeur dans reg1
        UpdateFlags(mRegisters[instr.reg1]);

        if (mDebug) {
            std::cout << "🔄 SWAP R" << static_cast<int>(instr.reg1)
                      << " ⇄ R" << static_cast<int>(instr.reg2)
                      << " (R" << static_cast<int>(instr.reg1) << "=0x" << std::hex
                      << mRegisters[instr.reg1] << ", R" << static_cast<int>(instr.reg2)
                      << "=0x" << mRegisters[instr.reg2] << ")" << std::endl;
        }
    }

    void CPU::ExecuteLoop(const Instruction& instr) {
        // LOOP: décrémente le registre et saute si non-zéro
        // Utilise reg1 comme compteur et immediate comme adresse de saut
        uint64_t counter = mRegisters[instr.reg1];
        counter--;
        mRegisters[instr.reg1] = counter;

        if (counter != 0) {
            uint64_t address = GetOperandValue(instr);
            mPC = address;

            if (mDebug) {
                std::cout << "🔁 LOOP taken (counter=" << std::dec << counter
                          << ") to address 0x" << std::hex << address << std::endl;
            }
        } else {
            if (mDebug) {
                std::cout << "🏁 LOOP finished (counter=0)" << std::endl;
            }
        }

        UpdateFlags(counter);
    }

    void CPU::ExecutePrint(const Instruction& instr) {
        uint64_t value = GetOperandValue(instr);

        std::cout << "📟 PRINT: " << std::dec << value
                  << " (0x" << std::hex << value << ")" << std::endl;

        if (mDebug) {
            std::cout << "🖨️ PRINT executed: value=" << std::dec << value << std::endl;
        }
    }

    void CPU::SetOperandValue(const Instruction& instr, uint64_t value, bool isSecondOperand) {
        uint8_t reg = isSecondOperand ? instr.reg2 : instr.reg1;

        switch (instr.mode) {
            case AddressingMode::REGISTER:
                mRegisters[reg] = value;
                break;
            case AddressingMode::MEMORY:
                mMemory->Write64(instr.immediate, value);
                break;
            case AddressingMode::REGISTER_INDIRECT:
                mMemory->Write64(mRegisters[reg], value);
                break;
            case AddressingMode::IMMEDIATE:
                mRegisters[instr.reg1] = value;
                break;
            default:
                // Invalid mode for writing
                break;
        }
    }

    void CPU::SetFlag(FlagType flag, bool value) {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        if (value) {
            mFlags |= mask;
        } else {
            mFlags &= ~mask;
        }
    }

    bool CPU::GetFlag(FlagType flag) const {
        uint32_t mask = 1 << static_cast<uint8_t>(flag);
        return (mFlags & mask) != 0;
    }

    void CPU::UpdateFlags(uint64_t result, bool carry, bool overflow) {
        SetFlag(FlagType::ZERO, result == 0);
        SetFlag(FlagType::CARRY, carry);
        SetFlag(FlagType::NEGATIVE, (result & 0x8000000000000000ULL) != 0);
        SetFlag(FlagType::OF, overflow);
    }

    uint64_t CPU::GetRegister(uint8_t reg) const {
        if (reg < REGISTER_COUNT) {
            return mRegisters[reg];
        }
        return 0;
    }

    void CPU::SetRegister(uint8_t reg, uint64_t value) {
        if (reg < REGISTER_COUNT) {
            mRegisters[reg] = value;
        }
    }

    void CPU::PrintState() const {
        std::cout << "\n┌── CPU State ──┐" << std::endl;
        std::cout << "│ PC: 0x" << std::hex << std::setfill('0') << std::setw(16) << mPC << " │" << std::endl;
        std::cout << "│ SP: 0x" << std::hex << std::setfill('0') << std::setw(16) << mSP << " │" << std::endl;
        std::cout << "│ Flags: 0x" << std::hex << std::setfill('0') << std::setw(8) << mFlags << "     │" << std::endl;
        std::cout << "│ Z:" << (GetFlag(FlagType::ZERO) ? "1" : "0") 
                  << " C:" << (GetFlag(FlagType::CARRY) ? "1" : "0")
                  << " N:" << (GetFlag(FlagType::NEGATIVE) ? "1" : "0")
                  << " O:" << (GetFlag(FlagType::OF) ? "1" : "0") << "           │" << std::endl;
        std::cout << "└─────────────────┘" << std::endl;

        std::cout << "\n📋 Registers:" << std::endl;
        for (size_t i = 0; i < REGISTER_COUNT; i += 4) {
            for (size_t j = 0; j < 4 && (i + j) < REGISTER_COUNT; ++j) {
                size_t reg_idx = i + j;
                std::cout << "R" << std::dec << std::setfill(' ') << std::setw(2) << reg_idx 
                         << ":0x" << std::hex << std::setfill('0') << std::setw(16)
                         << mRegisters[reg_idx];
                if (j < 3 && (i + j + 1) < REGISTER_COUNT) std::cout << " │ ";
            }
            std::cout << std::endl;
        }
    }

    void CPU::HandleInterrupt(int num) {
        // Basic interrupt implementation
        if (mDebug) {
            std::cout << "⚡ Interrupt " << num << " triggered" << std::endl;
        }

        // Save current state
        mSP -= 8;
        mMemory->Write64(mSP, mPC);
        mSP -= 8;
        mMemory->Write64(mSP, mFlags);

        // Jump to interrupt handler
        // For now, use a simple table
        uint64_t handlerAddress = num * 8; // Each entry is 8 bytes
        mPC = mMemory->Read64(handlerAddress);

        // Disable interrupts during handling
        SetFlag(FlagType::INTERRUPT, false);
    }
}