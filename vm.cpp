#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <map>
#include <string>
#include <stdexcept>
#include <cstring>

using namespace std;

#include "Util.hpp"
#include "VM.hpp"
#include "Program.hpp"
#include "Assembler.hpp"
#include "Scanner.hpp"
#include "AST.hpp"
#include "Parser.hpp"

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
            return LOAD_INT;

        case Var::FLOAT:
            return LOAD_FLOAT;

        case Var::DOUBLE:
            return LOAD_DOUBLE;
    }
}

Opcode getStoreOpcode(Var::Type type)
{
    switch (type)
    {
        case Var::INT:
            return STORE_INT;

        case Var::FLOAT:
            return STORE_FLOAT;

        case Var::DOUBLE:
            return STORE_DOUBLE;
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

struct LocalContext {
    typedef std::map<string, BindingData> Bindings;
    Bindings bindings;
    int bytesUsed;

    LocalContext(std::vector<Var> vars) : bytesUsed(0)
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
            die("Undefined variable " + name);

        return iter->second;
    }

    void writeLoad(Program& prog, string name)
    {
        BindingData bd = getBindingData(name);
        prog.write(LOAD_STACK_OFFS_CONST, bd.spOffset);
        prog.write(getLoadOpcode(bd.varType));
    }

    void writeStore(Program& prog, string name)
    {
        BindingData bd = getBindingData(name);
        prog.write(LOAD_STACK_OFFS_CONST, bd.spOffset);
        prog.write(getStoreOpcode(bd.varType));
    }
};

void sumTest()
{
    int res;

    Program prog;
    vector<AsmToken> toks = {
        PUSHB_CONST, sizeof (int[4]) + sizeof (int),

        LOAD_VAL_CONST, 2,
        LOAD_STACK_OFFS_CONST, -20,
        STORE_INT,

        LOAD_VAL_CONST, 3,
        LOAD_STACK_OFFS_CONST, -16,
        STORE_INT,

        LOAD_VAL_CONST, 4,
        LOAD_STACK_OFFS_CONST, -12,
        STORE_INT,

        LOAD_VAL_CONST, 5,
        LOAD_STACK_OFFS_CONST, -8,
        STORE_INT,

        LOAD_STACK_OFFS_CONST, -20,
        LOAD_STACK_OFFS_CONST, -4,
        STORE_ADDR,

        LOAD_VAL_CONST, 0.0,

        "loop1",
        LOAD_STACK_OFFS_CONST, -4,
        LOAD_ADDR,

        LOAD_INT,
        ADD,

        LOAD_STACK_OFFS_CONST, -4,
        LOAD_ADDR,
        LOAD_VAL_CONST, 4,

        ADD,
        LOAD_STACK_OFFS_CONST, -4,
        STORE_ADDR,
        LOAD_ADDR_CONST, "loop1",

        LOAD_STACK_OFFS_CONST, -4,
        LOAD_ADDR,
        LOAD_STACK_OFFS_CONST, -4,
        SUB,

        JLT,

        LOAD_ADDR_CONST, &res,
        STORE_INT,
        HALT,
    };
    
    Assembler assembler(prog, toks);

    VM vm(prog.data);
    vm.run();

    printf("RESULT = %d\n", res);
}

void testFrame()
{
    Program prog;

    LocalContext frame({
        Var(Var::INT, "a"),
        Var(Var::DOUBLE, "b"),
        Var(Var::INT, "c"),
    });

    frame.writeStackAlloc(prog);

    prog.write(LOAD_VAL_CONST, 2);
    frame.writeStore(prog, "a");

    prog.write(LOAD_VAL_CONST, 3);
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
}

void testScanner()
{
    Scanner scanner("123 (0456.@$@%789)))");

    auto tokens = scanner.scan();

    for (Token tok : tokens)
    {
        if (tok.type == Token::INVALID)
        {
            printf("Unexpected token (%s)\n", tok.text.c_str());
            break;
        }
        else
            printf("%s\n", tok.text.c_str());
    }
}

/*
 * expr = (list | atom)
 * list = '(' expr* ')'
 * atom = name | literal
*/







/*
(declfun zaza (int int (char *)) (int))
(defun zaza (x y z))
//(let ((int *) x 1)
 
 */

/*
 (let ((x 3 int) (y 4 float))
 * (f1 x)
 * (f2 y))
 */

//StackFrame compileLetForm(Program& prog, List const& lst)
//{
    //StackFrame frame;
//}




void compileBuiltinAdd(Program& prog)
{
    LocalContext ctx({
        Var(Var::INT, "retval"),
        Var(Var::INT, "a"),
        Var(Var::INT, "b"),
    });
    
    //prog.write();
    ctx.writeLoad(prog, "a");
    ctx.writeLoad(prog, "b");
    prog.write(ADD);
    ctx.writeStore(prog, "retval");
    
    ctx.writeStackFree(prog);
    
//    Program prog;
//
//    StackFrame frame({
//        Var(Var::INT, "a"),
//        Var(Var::DOUBLE, "b"),
//        Var(Var::INT, "c"),
//    });
//
//    frame.writeStackAlloc(prog);
//
//    prog.write(LOAD_VAL_CONST, 2);
//    frame.writeStore(prog, "a");
//
//    prog.write(LOAD_VAL_CONST, 3);
//    frame.writeStore(prog, "b");
//
//    frame.writeLoad(prog, "a");
//    frame.writeLoad(prog, "b");
//
//    prog.write(ADD);
//
//    frame.writeStore(prog, "c");
//
//    /*
//    AsmParser asmParser(prog, {
//        HALT,
//    });*/
//
//    VM vm(prog.data);
//    vm.run();
//    vm.printOpStack();
//
//    printf("%f\n", getVar(vm.sp, Var::INT, frame.getBindingData("c").spOffset));
}

int main()
{
    Scanner scanner("(tata () zaza (baz (kaka ())))");
    auto toks = scanner.scan();
    Parser parser(toks.begin());

    try
    {
        ASTNode::Sptr root = parser.readExpr();
        root->print();
    }
    
    catch(CompilationError const& e)
    {
        printf("%s\n", e.what());
    }
    
    printf("\n");
    
    //sumTest();
    //testFrame();
    
    return 0;
}