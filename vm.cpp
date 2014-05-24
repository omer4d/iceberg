#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <string>

using namespace std;

#include "Util.hpp"
#include "VM.hpp"
#include "Program.hpp"
#include "Assembler.hpp"
#include "Scanner.hpp"

struct Var {

    enum Type {
        INT, FLOAT, DOUBLE,
    };

    Type type;
    string name;

    Var(Type pType, string pName) : type(pType), name(pName)
    {
    }

    int size() const
    {
        switch (type)
        {
            case INT:
                return sizeof (int);
            case FLOAT:
                return sizeof (float);
            case DOUBLE:
                return sizeof (double);
        }
    }
};

struct BindingData {
    Var::Type varType;
    int spOffset;

    BindingData(Var::Type pVarType, int pSpOffset) : varType(pVarType), spOffset(pSpOffset)
    {
    }
};

Opcode getLoadOpcode(Var::Type type)
{
    switch (type)
    {
        case Var::INT:
            return LOADI;

        case Var::FLOAT:
            return LOADF;

        case Var::DOUBLE:
            return LOADD;
    }
}

Opcode getStoreOpcode(Var::Type type)
{
    switch (type)
    {
        case Var::INT:
            return STOREI;

        case Var::FLOAT:
            return STOREF;

        case Var::DOUBLE:
            return STORED;
    }
}

Value getVar(uint8_t* buff, Var::Type type, int offs = 0)
{
    switch (type)
    {
        case Var::INT:
            return *(int*) (buff + offs);
        case Var::FLOAT:
            return *(float*) (buff + offs);
        case Var::DOUBLE:
            return *(double*) (buff + offs);
    }
}

struct StackFrame {
    typedef map<string, BindingData> Bindings;
    Bindings bindings;
    int bytesUsed;

    StackFrame(vector<Var> vars) : bytesUsed(0)
    {
        for (Var const& var : vars)
        {
            bytesUsed += var.size();
            bindings.insert(pair<string, BindingData>(var.name, BindingData(var.type, -bytesUsed)));
        }
    }

    void writeStackAlloc(Program& prog)
    {
        prog.write(PUSHB_CONST, bytesUsed);
    }

    void writeStackFree(Program& prog)
    {
        prog.write(POPB_CONST, bytesUsed);
    }

    BindingData getBindingData(string name)
    {
        auto iter = bindings.find(name);

        if (iter == bindings.end())
            error("Undefined variable " + name);

        return iter->second;
    }

    void writeLoad(Program& prog, string name)
    {
        BindingData bd = getBindingData(name);
        prog.write(LOADSP_CONST, bd.spOffset);
        prog.write(getLoadOpcode(bd.varType));
    }

    void writeStore(Program& prog, string name)
    {
        BindingData bd = getBindingData(name);
        prog.write(LOADSP_CONST, bd.spOffset);
        prog.write(getStoreOpcode(bd.varType));
    }
};

void sumTest()
{
    int res;

    Program prog;

    Assembler assembler(prog,{
                        PUSHB_CONST, sizeof (int[4]) + sizeof (int),

                        LOADV_CONST, 2,
                        LOADSP_CONST, -20,
                        STOREI,

                        LOADV_CONST, 3,
                        LOADSP_CONST, -16,
                        STOREI,

                        LOADV_CONST, 4,
                        LOADSP_CONST, -12,
                        STOREI,

                        LOADV_CONST, 5,
                        LOADSP_CONST, -8,
                        STOREI,

                        LOADSP_CONST, -20,
                        LOADSP_CONST, -4,
                        STOREA,

                        LOADV_CONST, 0.0,

                        "loop1",
                        LOADSP_CONST, -4,
                        LOADA,

                        LOADI,
                        ADD,

                        LOADSP_CONST, -4,
                        LOADA,
                        LOADV_CONST, 4,

                        ADD,
                        LOADSP_CONST, -4,
                        STOREA,
                        LOADA_CONST, "loop1",

                        LOADSP_CONST, -4,
                        LOADA,
                        LOADSP_CONST, -4,
                        SUB,

                        JLT,

                        LOADA_CONST, &res,
                        STOREI,
                        HALT,
    });

    VM vm(prog.data);
    vm.run();

    printf("RESULT = %d\n", res);
}

void testScanner()
{
    Scanner scanner("123 (0456.@$@%789)))");

    auto tokens = scanner.scan();

    for (Token tok : tokens)
    {
        if (tok.type == Token::INVALID)
        {
            printf("Unexpected token (%s)\n", tok.text().c_str());
            break;
        }
        else
            printf("%s\n", tok.text().c_str());
    }
}

int main()
{
    Program prog;

    StackFrame frame({
                     Var(Var::INT, "a"),
                     Var(Var::DOUBLE, "b"),
                     Var(Var::INT, "c"),
    });

    frame.writeStackAlloc(prog);

    prog.write(LOADV_CONST, 2);
    frame.writeStore(prog, "a");

    prog.write(LOADV_CONST, 3);
    frame.writeStore(prog, "b");

    frame.writeLoad(prog, "a");
    frame.writeLoad(prog, "b");

    prog.write(ADD);

    frame.writeStore(prog, "c");

    /*
    AsmParser asmParser(prog, {
        HALT,
    });*/

    VM vm(prog.data);
    vm.run();
    vm.printOpStack();

    printf("%f\n", getVar(vm.sp, Var::INT, frame.getBindingData("c").spOffset));

    testScanner();

    //printf("RESULT = %f (%d)\n", vm.opStack[0], (int)vm.opStack.size());

    return 0;
}