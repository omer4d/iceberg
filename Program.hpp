#ifndef _PROGRAM_HPP_
#define _PROGRAM_HPP_

#include "Opcode.hpp"
#include "VMTypes.hpp"

struct Program {
	uint8_t data[3000];
	uint8_t* cursor;
	
	void write(Opcode opcode)
	{
		*cursor = opcode;
		++cursor;
	}
	
	void write(Opcode opcode, Value v)
	{
		*cursor = opcode;
		++cursor;
		*(Value*)cursor = v;
		cursor += sizeof(Value);
	}
	
	void write(Opcode opcode, Addr addr)
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

#endif