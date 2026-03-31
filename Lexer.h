#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <fstream>
#include <unordered_map>
#include "Token.h"
#include "SymbolTable.h"

class Lexer
{
public:
    Lexer(const std::string &filename);
    ~Lexer();
    Token nextToken();

private:
    // state
    std::ifstream m_source;
    char m_currentChar;
    int m_currentLine;

    std::unordered_map<std::string, TokenType> m_keywords;

    // logic
    void advance();
    void skipWhiteSpace();

    Token handleInteger();
    Token handleString();
    Token handleIdentifier();
    Token handleSymbol();

    void initKeywords();
};

#endif