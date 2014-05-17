#include <stdio.h>
#include <stdlib.h>

#include <map>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

#define GP_STACK_BYTES (1024 * 1024 * 2)

void error(std::string const& msg)
{
    printf("%s", msg.c_str());
    char* ptr = nullptr;
    *ptr = 0;
}

enum Opcode {
	HALT, GOTO, JMP, JE, JNE, JGT, JLT, JGET, JLET,
	BAND, BOR, BXOR, BSL1, BSR1, BSL, BSR,
	ADD, SUB, MUL, DIV, MOD,
		
	LOADUI8, LOADUI16, LOADUI32, LOADUI,
	LOADI8, LOADI16, LOADI32, LOADI,
	LOADF, LOADD, LOADA, 
	LOADV_CONST, LOADA_CONST, LOADSP_CONST,
		
	STOREUI8, STOREUI16, STOREUI32, STOREUI,
	STOREI8, STOREI16, STOREI32, STOREI,
	STOREF, STORED, STOREA,
		
	PUSHB_CONST, POPB_CONST, PUSHB, POPB
};

char const* OPCODE_NAMES[] = {
    "HALT", "GOTO", "JMP", "JE", "JNE", "JGT", "JLT", "JGET", "JLET",
	"BAND", "BOR", "BXOR", "BSL1", "BSR1", "BSL", "BSR",
	"ADD", "SUB", "MUL", "DIV", "MOD",
	
	"LOADUI8", "LOADUI16", "LOADUI32", "LOADUI",
	"LOADI8", "LOADI16", "LOADI32", "LOADI",
	"LOADF", "LOADD", "LOADA",
	"LOADV_CONST", "LOADA_CONST", "LOADSP_CONST",
	
	"STOREUI8", "STOREUI16", "STOREUI32", "STOREUI",
	"STOREI8", "STOREI16", "STOREI32", "STOREI",
	"STOREF", "STORED", "STOREA",
	
	"PUSHB_CONST", "POPB_CONST", "PUSHB", "POPB"
};

typedef double Value;
typedef void* Addr;

struct VM {
	vector<Value> opStack;
	uint8_t* gpStack;
	uint8_t* program;
	uint8_t* sp;
	uint8_t* ip;
	
	VM(uint8_t* program)
	{
		gpStack = new uint8_t[GP_STACK_BYTES];
		sp = gpStack;
		ip = program;
	}
	
	~VM()
	{
		delete[] gpStack;
	}
	
	void printOpStack()
	{
	    printf("--------\n");
	    
	    for(Value v : opStack)
	        printf("%f\n", v);
	        
	    printf("---------\n");
	}
	
	Value progReadValue()
	{
		Value v = *(Value*)ip;
		ip += sizeof(Value);
		return v;
	}
	
	Addr progReadAddr()
	{
		return (Addr)(uintptr_t)progReadValue();
	}
	
	void run()
	{
		while(ip)
		{
			printf("%s\n", OPCODE_NAMES[*ip]);
			
			switch(*ip)
			{
				case HALT: ++ip; halt(); break;
				case GOTO: ++ip; goto_(progReadAddr()); break;
				case JMP: ++ip; jmp(); break;
				case JE: ++ip; je(); break;
				case JNE: ++ip; jne(); break;
				case JGT: ++ip; jgt(); break;
				case JLT: ++ip; jlt(); break;
				case JGET: ++ip; jget(); break;
				case JLET: ++ip; jlet(); break;
				
				case BAND: ++ip; band(); break;
				case BOR: ++ip; bor(); break;
				case BXOR: ++ip; bxor(); break;
				case BSL1: ++ip; bsl1(); break;
				case BSR1: ++ip; bsr1(); break;
				case BSL: ++ip; bsl(); break;
				case BSR: ++ip; bsr(); break;
				
				case ADD: ++ip; add(); break;
				case SUB: ++ip; sub(); break;
				case MUL: ++ip; mul(); break;
				case DIV: ++ip; div(); break;
				case MOD: ++ip; mod(); break;
		
				case LOADUI8: ++ip; loadui8(); break;
				case LOADUI16: ++ip; loadui16(); break;
				case LOADUI32: ++ip; loadui32(); break;
				case LOADUI: ++ip; loadui(); break;
				
				case LOADI8: ++ip; loadi8(); break;
				case LOADI16: ++ip; loadi16(); break;
				case LOADI32: ++ip; loadi32(); break;
				case LOADI: ++ip; loadi(); break;
				
				case LOADF: ++ip; loadf(); break;
				case LOADD: ++ip; loadd(); break;
				case LOADA: ++ip; loada(); break;
				
				case LOADSP_CONST: ++ip; loadsp_const(progReadValue()); break;
				case LOADV_CONST: ++ip; loadv_const(progReadValue()); break;
				case LOADA_CONST: ++ip; loada_const(progReadAddr()); break;
				
				case STOREUI8: ++ip; storeui8(); break;
				case STOREUI16: ++ip; storeui16(); break;
				case STOREUI32: ++ip; storeui32(); break;
				case STOREUI: ++ip; storeui(); break;
				
				case STOREI8: ++ip; storei8(); break;
				case STOREI16: ++ip; storei16(); break;
				case STOREI32: ++ip; storei32(); break;
				case STOREI: ++ip; storei(); break;
				
				case STOREF: ++ip; storef(); break;
				case STORED: ++ip; stored(); break;
				case STOREA: ++ip; storea(); break;
				
				case PUSHB: ++ip; pushb(); break;
				case POPB: ++ip; popb(); break;
				case PUSHB_CONST: ++ip; pushb_const(progReadValue()); break;
				case POPB_CONST: ++ip; popb_const(progReadValue()); break;
			}
		}
	}
	
	// *****************
	// * STACK HALPERS *
	// *****************
	
	void pushVal(Value val)
	{
		opStack.push_back(val);
	}
	
	void pushAddr(Addr addr)
	{
		opStack.push_back((uintptr_t)addr);
	}
	
	Value popVal()
	{
		Value val = opStack.back();
		opStack.pop_back();
		return val;
	}
	
	Addr popAddr()
	{
		Addr addr = (Addr)(uintptr_t)opStack.back();
		opStack.pop_back();
		return addr;
	}
	
	int popIval()
	{
		return (int)popVal();
	}
	
	// *********
	// * STACK *
	// *********
	
	void pushb_const(Value bytes)
	{
		sp += (int)bytes;
	}
	
	void popb_const(Value bytes)
	{
		sp -= (int)bytes;
	}
	
	void pushb()
	{
		pushb_const(popIval());
	}
	
	void popb()
	{
		popb_const(popIval());
	}
	
	// ****************
	// * FLOW CONTROL *
	// ****************
	
	void goto_(Addr addr)
	{
		ip = (uint8_t*)addr;
	}
	
	void jmp()
	{
		ip = (uint8_t*)popAddr();
	}
	
	void je()
	{
		if(popVal() == 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void jne()
	{
		if(popVal() != 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void jgt()
	{
		if(popVal() > 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void jlt()
	{
		if(popVal() < 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void jlet()
	{
		if(popVal() <= 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void jget()
	{
		if(popVal() >= 0)
			ip = (uint8_t*)popAddr();
		else
			popAddr();
	}
	
	void halt()
	{
		ip = nullptr;
	}
	
	// ****************
	// * BITEWISE OPS *
	// ****************
	
	void band()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a & b);
	}
	
	void bor()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a | b);
	}
	
	void bxor()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a ^ b);
	}
	
	void bsl1()
	{
		loadv_const(popIval() << 1);
	}
	
	void bsr1()
	{
		loadv_const(popIval() >> 1);
	}
	
	void bsl()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a << b);
	}
	
	void bsr()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a >> b);
	}
	
	// **************
	// * ARITHMETIC *
	// **************
	
	void add()
	{
		Value b = popVal();
		Value a = popVal();
		loadv_const(a + b);
	}
	
	void sub()
	{
		Value b = popVal();
		Value a = popVal();
		loadv_const(a - b);
	}
	
	void mul()
	{
		Value b = popVal();
		Value a = popVal();
		loadv_const(a * b);
	}
	
	void div()
	{
		Value b = popVal();
		Value a = popVal();
		loadv_const(a / b);
	}
	
	void mod()
	{
		int b = popIval();
		int a = popIval();
		loadv_const(a % b);
	}
	
	// ************
	// * LOAD OPS *
	// ************
	
	void loadui8()	 	{ pushVal(*(uint8_t*)popAddr()); }
	void loadui16()		{ pushVal(*(uint16_t*)popAddr()); }
	void loadui32()		{ pushVal(*(uint32_t*)popAddr()); }
	void loadui()		{ pushVal(*(unsigned int*)popAddr()); }
	
	void loadi8()	 	{ pushVal(*(int8_t*)popAddr()); }
	void loadi16()		{ pushVal(*(int16_t*)popAddr()); }
	void loadi32()		{ pushVal(*(int32_t*)popAddr()); }
	void loadi()		{ pushVal(*(int*)popAddr()); }
	
	void loadf() 		{ pushVal(*(float*)popAddr()); }
	void loadd() 		{ pushVal(*(double*)popAddr()); }
	void loada()		{ pushVal((uintptr_t)*(Addr*)popAddr());  }
	
	void loadsp_const(Value offs)	{ pushVal((uintptr_t)sp + offs); }
	void loadv_const(Value lit)		{ pushVal(lit); }
	void loada_const(Addr addr)		{ pushAddr(addr); }
	
	// *************
	// * STORE OPS *
	// *************
	
	void storeui8()		{ uint8_t* dest = (uint8_t*)popAddr(); *dest = (uint8_t)popVal(); }
	void storeui16()	{ uint16_t* dest = (uint16_t*)popAddr(); *dest = (uint16_t)popVal(); }
	void storeui32()	{ uint32_t* dest = (uint32_t*)popAddr(); *dest = (uint32_t)popVal(); }
	void storeui()		{ unsigned int* dest = (unsigned int*)popAddr(); *dest = (unsigned int)popVal(); }
	
	void storei8()		{ int8_t* dest = (int8_t*)popAddr(); *dest = (int8_t)popVal(); }
	void storei16()		{ int16_t* dest = (int16_t*)popAddr(); *dest = (int16_t)popVal(); }
	void storei32()		{ int32_t* dest = (int32_t*)popAddr(); *dest = (int32_t)popVal(); }
	void storei()		{ int* dest = (int*)popAddr(); *dest = (int)popVal(); }
	
	void storef()		{ float* dest = (float*)popAddr(); *dest = (float)popVal(); }
	void stored()		{ double* dest = (double*)popAddr(); *dest = popVal(); }
	void storea()		{ Addr* dest = (Addr*)popAddr(); *dest = (Addr)(uintptr_t)popVal(); }
};

struct Program {
	uint8_t data[3000];
	uint8_t* cursor;
	
	void write(int opcode)
	{
		*cursor = opcode;
		++cursor;
	}
	
	void write(int opcode, Value v)
	{
		*cursor = opcode;
		++cursor;
		*(Value*)cursor = v;
		cursor += sizeof(Value);
	}
	
	void write(int opcode, Addr addr)
	{
		Value v = (uintptr_t)addr;
		
		*cursor = opcode;
		++cursor;
		*(Value*)cursor = v;
		cursor += sizeof(Value);
	}
	
	Program()
	{
		for(int i = 0; i < 3000; ++i)
			data[i] = 0;
			
		cursor = data;
	}
};

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
	
	AsmToken(Opcode opcode): type(OPCODE)
	{
		data.opcode = opcode;
	}
	
	AsmToken(Value value): type(VALUE)
	{
		data.value = value;
	}
	
	AsmToken(Addr addr): type(ADDR)
	{
		data.addr = addr;
	}
	
	AsmToken(char const* label): type(LABEL)
	{
		data.label = label;
	}
};

struct AsmParser {
	Program& prog;
	std::vector<AsmToken> const& tokens;
	int tokIndex;
	std::map<std::string, Addr> labMap;
	
	AsmParser(Program& pProg, std::vector<AsmToken> const& pTokens): prog(pProg), tokens(pTokens), tokIndex(0)
	{
		while(!reachedEnd())
		{
			if(tokens[tokIndex].type == AsmToken::LABEL)
			{
				labMap[tokens[tokIndex].data.label] = prog.cursor;
				++tokIndex;
			}
			else
				parseInstruction();
		}
	}
	
	bool reachedEnd()
	{
		return tokIndex >= (int)tokens.size();
	}
	
	Opcode parseOpcode()
	{
		if(tokens[tokIndex].type != AsmToken::OPCODE)
			error("Expected opcode!");
			
		Opcode opcode = tokens[tokIndex].data.opcode;
		++tokIndex;
		return opcode;
	}
	
	Value parseValue()
	{
		if(tokens[tokIndex].type != AsmToken::VALUE)
			error("Expected value!");

		Value value = tokens[tokIndex].data.value;
		++tokIndex;
		return value;
	}
	
	Addr parseAddrOrLabel()
	{
		Addr addr = nullptr;
		AsmToken const& tok = tokens[tokIndex];
	
		if(tok.type == AsmToken::ADDR)
			addr = tokens[tokIndex].data.addr;
		else if(tok.type == AsmToken::LABEL)
		{
			auto kv = labMap.find(tok.data.label);
			
			if(kv == labMap.end())
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
	
		switch(opcode)
		{
			case LOADA_CONST:
				prog.write(opcode, parseAddrOrLabel());
				break;
		
			case LOADV_CONST: case LOADSP_CONST: case PUSHB_CONST: case POPB_CONST:
				prog.write(opcode, parseValue());
				break;
				
			default:
				prog.write(opcode);
				break;
		}
	}
};

struct Var {
	enum Type {
		INT, FLOAT, DOUBLE,
	};
	
	Type type;
	string name;
	
	Var(Type pType, string pName): type(pType), name(pName)
	{
	}
	
	int size() const
	{
		switch(type)
		{
			case INT:
				return sizeof(int);
			case FLOAT:
				return sizeof(float);
			case DOUBLE:
				return sizeof(double);
		}
	}
};

struct BindingData {
	Var::Type varType;
	int spOffset;
	
	BindingData(Var::Type pVarType, int pSpOffset): varType(pVarType), spOffset(pSpOffset)
	{
	}
};

Opcode getLoadOpcode(Var::Type type)
{
    switch(type)
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
    switch(type)
    {
        case Var::INT:
            return STOREI;
	        
	    case Var::FLOAT:
	        return STOREF;
	       
	    case Var::DOUBLE:
	        return STORED;
	}
}

struct StackFrame {
    typedef map<string, BindingData> Bindings;
	Bindings bindings;
	int bytesUsed;
	
	StackFrame(vector<Var> vars): bytesUsed(0)
	{
		for(Var const& var : vars)
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
	    
	    if(iter == bindings.end())
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

	AsmParser asmParser(prog, {
		PUSHB_CONST, sizeof(int[4]) + sizeof(int),
		
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

int main()
{	
	Program prog;
	
	StackFrame frame({
	    Var(Var::INT, "a"),
	    Var(Var::INT, "b"),
	});
	
	frame.writeStackAlloc(prog);
	prog.write(LOADV_CONST, 2);
	frame.writeStore(prog, "a");
	
	prog.write(LOADV_CONST, 3);
	frame.writeStore(prog, "b");
	
	frame.writeLoad(prog, "a");
	frame.writeLoad(prog, "b");
	prog.write(ADD);
	
	/*
	AsmParser asmParser(prog, {
	    HALT,
	});*/
	
    VM vm(prog.data);
	vm.run();
	vm.printOpStack();


    //printf("RESULT = %f (%d)\n", vm.opStack[0], (int)vm.opStack.size());
    
	return 0;
}