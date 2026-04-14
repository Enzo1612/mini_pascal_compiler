#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>
#include <iostream>

using TokenValue = std::variant<int, std::string>;

enum class TokenType
{
    // KEYWORDS
    PROGRAM,
    BEGIN,
    END,
    CONST,
    VAR,
    WRITE,
    READ,
    // CONTROL
    IF,
    THEN,
    ELSE,
    WHILE,
    DO,
    // DATA
    STRING,
    INTEGER,
    IDENTIFIER,
    // SYMBOLS
    PLUS,
    MINUS,
    MULT,
    DIV,
    ASSIGN,
    LPAREN,
    RPAREN,
    SEMICOLON,
    DOT,
    COMMA,
    COLON,
    EQ,
    NEQ,
    LT,
    LE,
    GT,
    GE,
    // CONTROLS
    END_OF_FILE,
    ERROR
};

struct Token
{
    TokenType type;
    TokenValue value;
    int line;
};

#endif