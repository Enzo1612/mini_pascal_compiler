#include "VM.h"
#include <iostream>
#include <cstdlib>

void interpret(VirtualMachine &vm)
{
    vm.PC = 0;

    while (true)
    {
        OpCode op = static_cast<OpCode>(vm.InstructionMemory[vm.PC]);

        switch (op)
        {
        case OpCode::ADD:
            vm.ExecutionStack[vm.SP - 1] = vm.ExecutionStack[vm.SP - 1] + vm.ExecutionStack[vm.SP];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::SUB:
            vm.ExecutionStack[vm.SP - 1] = vm.ExecutionStack[vm.SP - 1] - vm.ExecutionStack[vm.SP];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::MUL:
            vm.ExecutionStack[vm.SP - 1] = vm.ExecutionStack[vm.SP - 1] * vm.ExecutionStack[vm.SP];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::DIV:
            if (vm.ExecutionStack[vm.SP] == 0)
            {
                std::cerr << "Runtime Error: Division by zero." << std::endl;
                exit(1);
            }
            vm.ExecutionStack[vm.SP - 1] = vm.ExecutionStack[vm.SP - 1] / vm.ExecutionStack[vm.SP];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::NEG:
            vm.ExecutionStack[vm.SP] = -vm.ExecutionStack[vm.SP];
            vm.PC++;
            break;

        case OpCode::ASSIGN:
            vm.GlobalMemory[vm.ExecutionStack[vm.SP - 1]] = vm.ExecutionStack[vm.SP];
            vm.SP -= 2;
            vm.PC++;
            break;

        case OpCode::READ_IN:
            std::cin >> vm.GlobalMemory[vm.ExecutionStack[vm.SP]];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::WRITE_NL:
            std::cout << std::endl;
            vm.PC++;
            break;

        case OpCode::WRITE_INT:
            std::cout << vm.ExecutionStack[vm.SP];
            vm.SP--;
            vm.PC++;
            break;

        case OpCode::WRITE_STR:
        {
            int i = 1;
            char ch = static_cast<char>(vm.InstructionMemory[vm.PC + i]);
            while (ch != static_cast<char>(OpCode::END_STR))
            {
                std::cout << ch;
                i++;
                ch = static_cast<char>(vm.InstructionMemory[vm.PC + i]);
            }
            vm.PC = vm.PC + i + 1;
            break;
        }

        case OpCode::PUSH:
            vm.SP++;
            vm.ExecutionStack[vm.SP] = vm.InstructionMemory[vm.PC + 1];
            vm.PC += 2;
            break;

        case OpCode::LOAD:
            vm.ExecutionStack[vm.SP] = vm.GlobalMemory[vm.ExecutionStack[vm.SP]];
            vm.PC++;
            break;

        case OpCode::JMP:
            vm.PC = vm.InstructionMemory[vm.PC + 1];
            break;

        case OpCode::JMPZ:
            if (vm.ExecutionStack[vm.SP] == 0)
            {
                vm.PC = vm.InstructionMemory[vm.PC + 1];
            }
            else
            {
                vm.PC += 2;
            }
            vm.SP--;
            break;

        case OpCode::HALT:
            return;

        default:
            std::cerr << "Runtime Error: Unknown OpCode " << static_cast<int>(op) << " at PC " << vm.PC << std::endl;
            exit(1);
        }
    }
}