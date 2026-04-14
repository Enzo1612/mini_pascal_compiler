#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "SymbolTable.h"
#include "Token.h"
#include "VM.h"

class Parser
{
public:
    Parser(Lexer &lexer, SymbolTable &symbols, VirtualMachine &vm);
    bool parse();

private:
    Lexer &m_lexer;
    SymbolTable &m_symbols;
    VirtualMachine &m_vm;
    Token m_currentToken;

    void nextToken();
    bool eat(TokenType type);
    void emit(int instruction);

    bool program();
    bool constDecl();
    bool varDecl();
    bool block();
    bool instruction();
    bool ifStatement();
    bool whileStatement();
    bool assignment();
    bool read();
    bool write();
    bool writeArg();
    bool expression();
    bool termSequence();
    bool term();
    bool binaryOp();
};

#endif