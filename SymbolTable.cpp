#include "SymbolTable.h"
#include <iostream>

SymbolTable::SymbolTable()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        m_buckets[i] = nullptr;
    }
}

SymbolTable::~SymbolTable()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Symbol *current = m_buckets[i];
        while (current != nullptr)
        {
            Symbol *next = current->next;
            delete current;
            current = next;
        }
        m_buckets[i] = nullptr;
    }
}

// p_{i} = \alpha * p_{i-1} + c_{i}
int SymbolTable::hash(const std::string &name)
{
    int p = 0;
    int a = 31; // Teacher didn't give but apparently it works well (prime)
    for (char c : name)
    {
        p = ((a * p) + (int)c) % SymbolTable::TABLE_SIZE;
    }
    return p;
}

Symbol *SymbolTable::find(const std::string &name)
{
    int hashIndex = hash(name);
    Symbol *current = m_buckets[hashIndex];
    while (current != nullptr)
    {
        if (current->name == name)
        {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

Symbol *SymbolTable::insert(const std::string &name, SymbolType type)
{
    if (find(name) != nullptr)
    {
        return find(name);
    }
    Symbol *new_symbol = new Symbol();
    new_symbol->name = name;
    new_symbol->type = type;

    int hashIndex = hash(new_symbol->name);
    new_symbol->next = m_buckets[hashIndex];
    m_buckets[hashIndex] = new_symbol;

    return new_symbol;
}

void SymbolTable::debugPrint()
{
    for (int i = 0; i < SymbolTable::TABLE_SIZE; i++)
    {
        if (m_buckets[i] != nullptr)
        {

            std::cout << i;
            Symbol *current = m_buckets[i];
            while (current != nullptr)
            {
                std::cout << "-> [Name: " << current->name << " Type: " << (int)current->type << "]";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
}