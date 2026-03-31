#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>

enum class SymbolType
{
    PROGRAM,
    VARIABLE,
    CONSTANT,
    PROCEDURE,
    FUNCTION
};

// T_ENREG_IDENT
// Inside SymbolTable.h
struct Symbol
{
    std::string name;
    SymbolType type;

    int value;
    int address;

    bool isString = false;

    Symbol *next = nullptr;
};

class SymbolTable
{
public:
    SymbolTable();
    ~SymbolTable();

    Symbol *find(const std::string &name);

    Symbol *insert(const std::string &name, SymbolType type);

    void debugPrint();

private:
    int hash(const std::string &name);

    static const int TABLE_SIZE = 211; // Prime number!
    Symbol *m_buckets[TABLE_SIZE];
};

#endif