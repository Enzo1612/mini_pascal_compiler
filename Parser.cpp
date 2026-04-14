#include "Parser.h"
#include <iostream>

Parser::Parser(Lexer &lexer, SymbolTable &symbols, VirtualMachine &vm)
    : m_lexer(lexer), m_symbols(symbols), m_vm(vm)
{
    nextToken();
}

void Parser::nextToken()
{
    m_currentToken = m_lexer.nextToken();
}

bool Parser::eat(TokenType type)
{
    if (m_currentToken.type == type)
    {
        nextToken();
        return true;
    }
    else
    {
        std::cout << "Syntax Error: Expected " << (int)type
                  << " but found " << (int)m_currentToken.type
                  << " at line " << m_currentToken.line << std::endl;
        return false;
    }
}

bool Parser::parse()
{
    if (program())
    {
        std::cout << "Parsing completed successfully!" << std::endl;
        return true;
    }
    else
    {
        std::cout << "Parsing failed." << std::endl;
        return false;
    }
}

bool Parser::program()
{
    if (!eat(TokenType::PROGRAM))
        return false;
    if (!eat(TokenType::IDENTIFIER))
        return false;
    if (!eat(TokenType::SEMICOLON))
        return false;

    if (m_currentToken.type == TokenType::CONST)
    {
        if (!constDecl())
            return false;
    }

    if (m_currentToken.type == TokenType::VAR)
    {
        if (!varDecl())
            return false;
    }

    if (!block())
        return false;

    if (!eat(TokenType::DOT))
        return false;

    emit(static_cast<int>(OpCode::HALT));

    return true;
}

bool Parser::constDecl()
{
    if (!eat(TokenType::CONST))
        return false;

    do
    {
        std::string name = std::get<std::string>(m_currentToken.value);
        if (!eat(TokenType::IDENTIFIER))
            return false;

        if (m_symbols.find(name) != nullptr)
        {
            std::cout << "Semantic Error: Constant '" << name << "' already declared.\n";
            return false;
        }

        if (!eat(TokenType::EQ))
            return false;

        Symbol *s = m_symbols.insert(name, SymbolType::CONSTANT);

        if (m_currentToken.type == TokenType::INTEGER)
        {
            s->value = std::get<int>(m_currentToken.value);
            s->isString = false;
            eat(TokenType::INTEGER);
        }
        else if (m_currentToken.type == TokenType::STRING)
        {
            s->value = 0;
            s->isString = true;
            eat(TokenType::STRING);
        }
        else
        {
            std::cout << "Syntax Error: Expected integer or string at line " << m_currentToken.line << "\n";
            return false;
        }

        if (m_currentToken.type == TokenType::COMMA)
        {
            eat(TokenType::COMMA);
        }
        else
        {
            break;
        }
    } while (true);

    return eat(TokenType::SEMICOLON);
}

bool Parser::block()
{
    if (!eat(TokenType::BEGIN))
        return false;
    if (!instruction())
        return false;
    while (m_currentToken.type == TokenType::SEMICOLON)
    {
        eat(TokenType::SEMICOLON);
        if (!instruction())
            return false;
    }
    if (!eat(TokenType::END))
        return false;
    return true;
}

bool Parser::varDecl()
{
    if (!eat(TokenType::VAR))
        return false;

    static int addressCounter = 0;

    do
    {
        std::string name = std::get<std::string>(m_currentToken.value);
        if (!eat(TokenType::IDENTIFIER))
            return false;

        if (m_symbols.find(name) != nullptr)
        {
            std::cout << "Semantic Error: Variable '" << name << "' already declared.\n";
            return false;
        }

        Symbol *s = m_symbols.insert(name, SymbolType::VARIABLE);
        s->address = addressCounter++;

        if (m_currentToken.type == TokenType::COMMA)
            eat(TokenType::COMMA);
        else
            break;
    } while (true);

    return eat(TokenType::SEMICOLON);
}

bool Parser::instruction()
{
    switch (m_currentToken.type)
    {
    case TokenType::IDENTIFIER:
        return assignment();
    case TokenType::READ:
        return read();
    case TokenType::WRITE:
        return write();
    case TokenType::BEGIN:
        return block();
    case TokenType::IF:
        return ifStatement();
    case TokenType::WHILE:
        return whileStatement();
    default:
        std::cout << "Syntax Error: Expected instruction at line " << m_currentToken.line << "\n";
        return false;
    }
}

bool Parser::assignment()
{
    std::string name = std::get<std::string>(m_currentToken.value);
    if (!eat(TokenType::IDENTIFIER))
        return false;

    Symbol *s = m_symbols.find(name);

    if (s == nullptr)
    {
        std::cout << "Semantic Error: '" << name << "' is undeclared.\n";
        return false;
    }
    if (s->type == SymbolType::CONSTANT)
    {
        std::cout << "Semantic Error: '" << name << "' is a constant and cannot be reassigned.\n";
        return false;
    }
    if (s->type != SymbolType::VARIABLE)
    {
        std::cout << "Semantic Error: '" << name << "' is not a valid variable.\n";
        return false;
    }

    emit(static_cast<int>(OpCode::PUSH));
    emit(s->address);

    if (!eat(TokenType::ASSIGN))
        return false;
    if (!expression())
        return false;

    emit(static_cast<int>(OpCode::ASSIGN));
    return true;
}

bool Parser::read()
{
    if (!eat(TokenType::READ))
        return false;
    if (!eat(TokenType::LPAREN))
        return false;

    do
    {
        std::string name = std::get<std::string>(m_currentToken.value);
        if (!eat(TokenType::IDENTIFIER))
            return false;

        Symbol *s = m_symbols.find(name);

        if (s == nullptr)
        {
            std::cout << "Semantic Error: '" << name << "' is undeclared.\n";
            return false;
        }
        if (s->type == SymbolType::CONSTANT)
        {
            std::cout << "Semantic Error: Cannot read input into constant '" << name << "'.\n";
            return false;
        }
        if (s->type != SymbolType::VARIABLE)
        {
            std::cout << "Semantic Error: '" << name << "' is not a valid variable.\n";
            return false;
        }

        emit(static_cast<int>(OpCode::PUSH));
        emit(s->address);
        emit(static_cast<int>(OpCode::READ_IN));

        if (m_currentToken.type == TokenType::COMMA)
            eat(TokenType::COMMA);
        else
            break;
    } while (true);

    return eat(TokenType::RPAREN);
}

bool Parser::write()
{
    if (!eat(TokenType::WRITE))
        return false;
    if (!eat(TokenType::LPAREN))
        return false;

    if (m_currentToken.type != TokenType::RPAREN)
    {
        do
        {
            if (!writeArg())
                return false;

            if (m_currentToken.type == TokenType::COMMA)
                eat(TokenType::COMMA);
            else
                break;
        } while (true);
    }

    emit(static_cast<int>(OpCode::WRITE_NL));

    return eat(TokenType::RPAREN);
}
bool Parser::writeArg()
{
    if (m_currentToken.type == TokenType::STRING)
    {
        std::string strVal = std::get<std::string>(m_currentToken.value);
        eat(TokenType::STRING);

        emit(static_cast<int>(OpCode::WRITE_STR));
        for (char c : strVal)
        {
            emit(static_cast<int>(c));
        }
        emit(static_cast<int>(OpCode::END_STR));

        return true;
    }

    if (expression())
    {
        emit(static_cast<int>(OpCode::WRITE_INT));
        return true;
    }
    return false;
}

bool Parser::expression()
{
    if (!term())
        return false;
    if (!termSequence())
        return false;
    return true;
}

bool Parser::termSequence()
{
    if (m_currentToken.type == TokenType::PLUS ||
        m_currentToken.type == TokenType::MINUS ||
        m_currentToken.type == TokenType::MULT ||
        m_currentToken.type == TokenType::DIV)
    {
        TokenType op = m_currentToken.type;

        if (!binaryOp())
            return false;
        if (!expression())
            return false;

        if (op == TokenType::PLUS)
            emit(static_cast<int>(OpCode::ADD));
        else if (op == TokenType::MINUS)
            emit(static_cast<int>(OpCode::SUB));
        else if (op == TokenType::MULT)
            emit(static_cast<int>(OpCode::MUL));
        else if (op == TokenType::DIV)
            emit(static_cast<int>(OpCode::DIV));

        return true;
    }
    return true;
}

bool Parser::binaryOp()
{
    switch (m_currentToken.type)
    {
    case TokenType::PLUS:
        return eat(TokenType::PLUS);
    case TokenType::MINUS:
        return eat(TokenType::MINUS);
    case TokenType::MULT:
        return eat(TokenType::MULT);
    case TokenType::DIV:
        return eat(TokenType::DIV);
    default:
        std::cout << "Expected binary operator at line " << m_currentToken.line << std::endl;
        return false;
    }
}

bool Parser::term()
{
    switch (m_currentToken.type)
    {
    case TokenType::INTEGER:
    {
        int val = std::get<int>(m_currentToken.value);
        if (!eat(TokenType::INTEGER))
            return false;

        emit(static_cast<int>(OpCode::PUSH));
        emit(val);
        return true;
    }
    case TokenType::IDENTIFIER:
    {
        std::string name = std::get<std::string>(m_currentToken.value);
        Symbol *s = m_symbols.find(name);

        if (s == nullptr)
        {
            std::cout << "Semantic Error: Undeclared identifier '" << name << "'.\n";
            return false;
        }

        if (!eat(TokenType::IDENTIFIER))
            return false;

        if (s->type == SymbolType::VARIABLE)
        {
            emit(static_cast<int>(OpCode::PUSH));
            emit(s->address);
            emit(static_cast<int>(OpCode::LOAD));
        }
        else if (s->type == SymbolType::CONSTANT)
        {
            if (s->isString)
            {
                std::cout << "Semantic Error: Type mismatch. Constant '" << name << "' is a string and cannot be used in arithmetic expressions.\n";
                return false;
            }

            emit(static_cast<int>(OpCode::PUSH));
            emit(s->value);
        }
        return true;
    }
    case TokenType::LPAREN:
        eat(TokenType::LPAREN);
        if (!expression())
            return false;
        return eat(TokenType::RPAREN);

    case TokenType::MINUS:
        eat(TokenType::MINUS);
        if (!term())
            return false;
        emit(static_cast<int>(OpCode::NEG));
        return true;

    default:
        std::cout << "Syntax Error: Expected term at line " << m_currentToken.line << "\n";
        return false;
    }
}

void Parser::emit(int instruction)
{
    m_vm.InstructionMemory[m_vm.PC++] = instruction;
}

bool Parser::whileStatement()
{
    if (!eat(TokenType::WHILE))
        return false;

    // Mark top to jump back
    int loopStartIndex = m_vm.PC;

    if (!expression())
        return false;
    if (!eat(TokenType::DO))
        return false;

    // Jump with zero + placeholder
    emit(static_cast<int>(OpCode::JMPZ));
    int jumpOutIndex = m_vm.PC;
    emit(0);

    if (!instruction())
        return false;

    // Jump to eval expr
    emit(static_cast<int>(OpCode::JMP));
    emit(loopStartIndex);

    // backpatch
    m_vm.InstructionMemory[jumpOutIndex] = m_vm.PC;

    return true;
}

bool Parser::ifStatement()
{
    if (!eat(TokenType::IF))
        return false;
    if (!expression())
        return false;
    if (!eat(TokenType::THEN))
        return false;

    emit(static_cast<int>(OpCode::JMPZ));
    int jumpElseIndex = m_vm.PC;
    emit(0);

    if (!instruction())
        return false;

    if (m_currentToken.type == TokenType::ELSE)
    {
        eat(TokenType::ELSE);

        emit(static_cast<int>(OpCode::JMP));
        int jumpEndIndex = m_vm.PC;
        emit(0);

        m_vm.InstructionMemory[jumpElseIndex] = m_vm.PC;

        if (!instruction())
            return false;

        m_vm.InstructionMemory[jumpEndIndex] = m_vm.PC;
    }
    else
    {
        m_vm.InstructionMemory[jumpElseIndex] = m_vm.PC;
    }

    return true;
}