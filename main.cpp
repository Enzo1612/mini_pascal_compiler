#include <iostream>
#include <iomanip>
#include <string>
#include "Lexer.h"
#include "SymbolTable.h"
#include "Parser.h"
#include "VM.h"

#include <fstream>

void interpret(VirtualMachine &vm);

std::string decodeInstruction(int code)
{
    switch (code)
    {
    case 0:
        return "ADD";
    case 1:
        return "SUB";
    case 2:
        return "MUL";
    case 3:
        return "DIV";
    case 4:
        return "NEG";
    case 5:
        return "ASSIGN";
    case 6:
        return "READ_IN";
    case 7:
        return "WRITE_NL";
    case 8:
        return "WRITE_INT";
    case 9:
        return "WRITE_STR";
    case 10:
        return "END_STR";
    case 11:
        return "PUSH";
    case 12:
        return "LOAD";
    case 13:
        return "HALT";
    default:
        return std::to_string(code);
    }
}

void generateCodFile(const VirtualMachine &vm, const std::string &sourceFilename)
{
    size_t lastDot = sourceFilename.find_last_of('.');
    std::string outName = (lastDot == std::string::npos)
                              ? sourceFilename + ".COD"
                              : sourceFilename.substr(0, lastDot) + ".COD";

    std::ofstream outFile(outName);
    if (!outFile.is_open())
        return;

    for (int i = 0; i < vm.PC; i++)
    {
        int op = vm.InstructionMemory[i];
        outFile << decodeInstruction(op);

        if (op == 11) // PUSH
        {
            i++;
            outFile << " " << vm.InstructionMemory[i];
        }
        else if (op == 9) // WRITE_STR
        {
            outFile << " '";
            i++;
            int ch = vm.InstructionMemory[i];
            while (ch != 10) // END_STR
            {
                outFile << static_cast<char>(ch);
                i++;
                ch = vm.InstructionMemory[i];
            }
            outFile << "' END_STR";
        }
        outFile << "\n";
    }

    outFile.close();
}

int main()
{
    std::string sourceFile = "test.pas";
    SymbolTable symTab;
    VirtualMachine vm;
    Lexer lexer(sourceFile);
    Parser parser(lexer, symTab, vm);

    std::cout << "--- 1. PARSING & SYNTHESIS ---\n";

    if (!parser.parse())
    {
        std::cerr << "\nFATAL: Compilation aborted due to errors. Virtual Machine will not execute.\n";
        return 1;
    }

    std::cout << "\n--- 2. SYMBOL TABLE STATE ---\n";
    symTab.debugPrint();

    std::cout << "\n--- 3. MACHINE CODE ---\n";
    for (int i = 0; i < vm.PC; i++)
    {
        int op = vm.InstructionMemory[i];
        std::cout << std::setw(4) << i << " | " << decodeInstruction(op);

        if (op == 11) // PUSH
        {
            i++;
            std::cout << " " << vm.InstructionMemory[i];
        }
        else if (op == 9) // WRITE_STR
        {
            std::cout << " '";
            i++;
            int ch = vm.InstructionMemory[i];
            while (ch != 10) // END_STR OpCode 10
            {
                std::cout << static_cast<char>(ch);
                i++;
                ch = vm.InstructionMemory[i];
            }
            std::cout << "' END_STR";
        }
        std::cout << "\n";
    }

    generateCodFile(vm, sourceFile);

    std::cout << "\n--- 4. VM EXECUTION ---\n";
    interpret(vm);
    std::cout << "\n--- EXECUTION COMPLETE ---\n";

    return 0;
}