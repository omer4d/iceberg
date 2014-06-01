#ifndef _ASSEMBLER_HPP_
#define _ASSEMBLER_HPP_

#include <vector>
#include <map>
#include <string>

#include "Util.hpp"
#include "Opcode.hpp"
#include "VMTypes.hpp"

struct AsmToken {

    enum Type {
        OPCODE, VALUE, ADDR, LABEL
    };

    union Data {
        Opcode opcode;
        Value value;
        Addr addr;
        char const* label;
    };

    Type type;
    Data data;

    AsmToken(Opcode opcode) : type(OPCODE)
    {
        data.opcode = opcode;
    }

    AsmToken(Value value) : type(VALUE)
    {
        data.value = value;
    }

    AsmToken(Addr addr) : type(ADDR)
    {
        data.addr = addr;
    }

    AsmToken(char const* label) : type(LABEL)
    {
        data.label = label;
    }
};

struct Assembler {
    Program& prog;
    std::vector<AsmToken> const& tokens;
    int tokIndex;
    std::map<std::string, Addr> labMap;

    Assembler(Program& pProg, std::vector<AsmToken> const& pTokens) : prog(pProg), tokens(pTokens), tokIndex(0)
    {
        while (!reachedEnd())
        {
            if (tokens[tokIndex].type == AsmToken::LABEL)
            {
                labMap[tokens[tokIndex].data.label] = prog.cursor;
                ++tokIndex;
            }
            else
                parseInstruction();
        }
    }

    static void error(std::string const& msg)
    {
        die(msg);
    }

    bool reachedEnd()
    {
        return tokIndex >= (int) tokens.size();
    }

    Opcode parseOpcode()
    {
        if (tokens[tokIndex].type != AsmToken::OPCODE)
            error("Expected opcode!");

        Opcode opcode = tokens[tokIndex].data.opcode;
        ++tokIndex;
        return opcode;
    }

    Value parseValue()
    {
        if (tokens[tokIndex].type != AsmToken::VALUE)
            error("Expected value!");

        Value value = tokens[tokIndex].data.value;
        ++tokIndex;
        return value;
    }

    Addr parseAddrOrLabel()
    {
        Addr addr = nullptr;
        AsmToken const& tok = tokens[tokIndex];

        if (tok.type == AsmToken::ADDR)
            addr = tokens[tokIndex].data.addr;
        else if (tok.type == AsmToken::LABEL)
        {
            auto kv = labMap.find(tok.data.label);

            if (kv == labMap.end())
                error("Unknown label!");
            else
                addr = kv->second;
        }

        else
            error("Expected address or label!");

        ++tokIndex;
        return addr;
    }

    void parseInstruction()
    {
        Opcode opcode = parseOpcode();

        switch (opcode)
        {
            case LOAD_ADDR_CONST:
                prog.write(opcode, parseAddrOrLabel());
                break;

            case LOAD_VAL_CONST: case LOAD_STACK_OFFS_CONST: case PUSHB_CONST: case POPB_CONST:
                prog.write(opcode, parseValue());
                break;

            default:
                prog.write(opcode);
                break;
        }
    }
};

#endif