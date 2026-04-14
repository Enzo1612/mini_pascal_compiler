#ifndef VM_H
#define VM_H

enum class OpCode
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    NEG = 4,
    ASSIGN = 5,
    READ_IN = 6,
    WRITE_NL = 7,
    WRITE_INT = 8,
    WRITE_STR = 9,
    END_STR = 10,
    PUSH = 11,
    LOAD = 12,
    HALT = 13,
    JMP = 14,
    JMPZ = 15
};

struct VirtualMachine
{
    static const int MAX_MEM_SIZE = 10000;

    int GlobalMemory[MAX_MEM_SIZE] = {0};
    int InstructionMemory[MAX_MEM_SIZE] = {0};
    int ExecutionStack[MAX_MEM_SIZE] = {0};

    int PC = 0;
    int SP = -1;
};

#endif