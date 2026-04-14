#include "Lexer.h"
#include <cctype> // isdigit, isalpha
#include <iostream>
#include <string>

Lexer::Lexer(const std::string &filename)
{
    m_source.open(filename);
    m_currentLine = 1;
    initKeywords();

    // Prime
    advance();
}

Lexer::~Lexer()
{
    if (m_source.is_open())
    {
        m_source.close();
    }
}

void Lexer::advance()
{
    if (m_source.get(m_currentChar))
    {
        if (m_currentChar == '\n')
        {
            m_currentLine++;
        }
    }
    else
    {
        m_currentChar = '\0';
    }
}

Token Lexer::nextToken()
{
    skipWhiteSpace();

    if (m_currentChar == '\0')
    {
        return {TokenType::END_OF_FILE, "", m_currentLine};
    }

    if (isdigit(m_currentChar))
    {
        return handleInteger();
    }
    if (isalpha(m_currentChar))
    {
        return handleIdentifier();
    }
    if (m_currentChar == '\'')
    {
        return handleString();
    }

    return handleSymbol();
}

void Lexer::skipWhiteSpace()
{
    while (true) // Space
    {
        if (m_currentChar == ' ' || m_currentChar == '\t' || m_currentChar == '\n') // THink about \r
        {
            advance();
        }

        else if (m_currentChar == '{') // Comment
        {
            int count = 1;
            advance();
            while (count > 0 && m_currentChar != '\0')
            {
                if (m_currentChar == '}')
                {
                    count--;
                }
                else if (m_currentChar == '{')
                {
                    count++;
                }
                advance();
            }
        }

        else // Real Token or EOF
        {
            break;
        }
    }
}

Token Lexer::handleInteger()
{
    std::string res = "";
    while (isdigit(m_currentChar))
    {
        res += m_currentChar;
        advance();
    }
    try
    {
        return {TokenType::INTEGER, std::stoi(res), m_currentLine};
    }
    catch (const std::out_of_range &e)
    {
        return {TokenType::ERROR, "Integer overflow", m_currentLine};
    }
    catch (const std::invalid_argument &e)
    {
        return {TokenType::ERROR, "Invalid integer", m_currentLine};
    }
}

Token Lexer::handleString()
{
    std::string res = "";
    advance();
    while (m_currentChar != '\'' && m_currentChar != '\0')
    {
        res += m_currentChar;
        advance();
    }

    if (m_currentChar == '\'')
    {
        advance();
    }
    return {TokenType::STRING, res, m_currentLine};
}

Token Lexer::handleIdentifier()
{
    std::string res = "";
    while (isalpha(m_currentChar) || isdigit(m_currentChar) || m_currentChar == '_')
    {
        res += m_currentChar;
        advance();
    }

    std::string upperRes = res;
    for (auto &c : upperRes)
        c = toupper(c);

    auto it = m_keywords.find(upperRes);
    if (it != m_keywords.end())
    {
        // Keyword
        return {it->second, upperRes, m_currentLine};
    }
    return {TokenType::IDENTIFIER, upperRes, m_currentLine};
}

Token Lexer::handleSymbol()
{
    // Complex
    if (m_currentChar == ':')
    {
        advance();
        if (m_currentChar == '=')
        {
            advance();
            return {TokenType::ASSIGN, ":=", m_currentLine};
        }
        return {TokenType::COLON, ":", m_currentLine};
    }

    if (m_currentChar == '<')
    {
        advance();
        if (m_currentChar == '=')
        {
            advance();
            return {TokenType::LE, "<=", m_currentLine};
        }
        if (m_currentChar == '>')
        {
            advance();
            return {TokenType::NEQ, "<>", m_currentLine};
        }
        return {TokenType::LT, "<", m_currentLine};
    }

    if (m_currentChar == '>')
    {
        advance();
        if (m_currentChar == '=')
        {
            advance();
            return {TokenType::GE, ">=", m_currentLine};
        }
        return {TokenType::GT, ">", m_currentLine};
    }

    // Simple
    char sym = m_currentChar;
    advance();

    switch (sym)
    {
    case '+':
        return {TokenType::PLUS, "+", m_currentLine};
    case '-':
        return {TokenType::MINUS, "-", m_currentLine};
    case '*':
        return {TokenType::MULT, "*", m_currentLine};
    case '/':
        return {TokenType::DIV, "/", m_currentLine};
    case '(':
        return {TokenType::LPAREN, "(", m_currentLine};
    case ')':
        return {TokenType::RPAREN, ")", m_currentLine};
    case '=':
        return {TokenType::EQ, "=", m_currentLine};
    case ',':
        return {TokenType::COMMA, ",", m_currentLine};
    case ';':
        return {TokenType::SEMICOLON, ";", m_currentLine};
    case '.':
        return {TokenType::DOT, ".", m_currentLine};
    }

    return {TokenType::ERROR, std::string("Unexpected character: ") + sym, m_currentLine};
}

void Lexer::initKeywords()
{
    m_keywords["PROGRAM"] = TokenType::PROGRAM;
    m_keywords["BEGIN"] = TokenType::BEGIN;
    m_keywords["END"] = TokenType::END;
    m_keywords["CONST"] = TokenType::CONST;
    m_keywords["VAR"] = TokenType::VAR;
    m_keywords["WRITE"] = TokenType::WRITE;
    m_keywords["READ"] = TokenType::READ;
    m_keywords["IF"] = TokenType::IF;
    m_keywords["THEN"] = TokenType::THEN;
    m_keywords["ELSE"] = TokenType::ELSE;
    m_keywords["WHILE"] = TokenType::WHILE;
    m_keywords["DO"] = TokenType::DO;
}