#ifndef _VM_HPP_
#define _VM_HPP_

#include <vector>
#include <cstdio>
#include <cstdint>

#include "Opcode.hpp"
#include "VMTypes.hpp"

#define GP_STACK_BYTES (1024 * 1024 * 2)

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

        for (Value v : opStack)
            printf("%f\n", v);

        printf("---------\n");
    }

    Value progReadValue()
    {
        Value v = *(Value*) ip;
        ip += sizeof (Value);
        return v;
    }

    Addr progReadAddr()
    {
        return (Addr) (uintptr_t) progReadValue();
    }

    void run()
    {
        while (ip)
        {
            printf("%s\n", OPCODE_NAMES[*ip]);

            switch (*ip)
            {
                case HALT: ++ip;
                    halt();
                    break;
                case GOTO: ++ip;
                    goto_(progReadAddr());
                    break;
                case JMP: ++ip;
                    jmp();
                    break;
                case JE: ++ip;
                    je();
                    break;
                case JNE: ++ip;
                    jne();
                    break;
                case JGT: ++ip;
                    jgt();
                    break;
                case JLT: ++ip;
                    jlt();
                    break;
                case JGET: ++ip;
                    jget();
                    break;
                case JLET: ++ip;
                    jlet();
                    break;

                case BAND: ++ip;
                    band();
                    break;
                case BOR: ++ip;
                    bor();
                    break;
                case BXOR: ++ip;
                    bxor();
                    break;
                case BSL1: ++ip;
                    bsl1();
                    break;
                case BSR1: ++ip;
                    bsr1();
                    break;
                case BSL: ++ip;
                    bsl();
                    break;
                case BSR: ++ip;
                    bsr();
                    break;

                case ADD: ++ip;
                    add();
                    break;
                case SUB: ++ip;
                    sub();
                    break;
                case MUL: ++ip;
                    mul();
                    break;
                case DIV: ++ip;
                    div();
                    break;
                case MOD: ++ip;
                    mod();
                    break;

                case LOAD_UCHAR: ++ip;
                    load_uchar();
                    break;
                case LOAD_USHORT: ++ip;
                    load_ushort();
                    break;
                case LOAD_ULONG: ++ip;
                    load_ulong();
                    break;
                case LOAD_UINT: ++ip;
                    load_uint();
                    break;

                case LOAD_CHAR: ++ip;
                    load_char();
                    break;
                case LOAD_SHORT: ++ip;
                    load_short();
                    break;
                case LOAD_LONG: ++ip;
                    load_long();
                    break;
                case LOAD_INT: ++ip;
                    load_int();
                    break;

                case LOAD_FLOAT: ++ip;
                    load_float();
                    break;
                case LOAD_DOUBLE: ++ip;
                    load_double();
                    break;
                case LOAD_ADDR: ++ip;
                    load_addr();
                    break;

                case LOAD_STACK_OFFS_CONST: ++ip;
                    load_stack_offs_const(progReadValue());
                    break;
                case LOAD_VAL_CONST: ++ip;
                    load_val_const(progReadValue());
                    break;
                case LOAD_ADDR_CONST: ++ip;
                    load_addr_const(progReadAddr());
                    break;

                case STORE_UCHAR: ++ip;
                    store_uchar();
                    break;
                case STORE_USHORT: ++ip;
                    store_ushort();
                    break;
                case STORE_ULONG: ++ip;
                    store_ulong();
                    break;
                case STORE_UINT: ++ip;
                    store_uint();
                    break;

                case STORE_CHAR: ++ip;
                    store_char();
                    break;
                case STORE_SHORT: ++ip;
                    store_short();
                    break;
                case STORE_LONG: ++ip;
                    store_long();
                    break;
                case STORE_INT: ++ip;
                    store_int();
                    break;

                case STORE_FLOAT: ++ip;
                    store_float();
                    break;
                case STORE_DOUBLE: ++ip;
                    store_double();
                    break;
                case STORE_ADDR: ++ip;
                    store_addr();
                    break;

                case PUSHB: ++ip;
                    pushb();
                    break;
                case POPB: ++ip;
                    popb();
                    break;
                case PUSHB_CONST: ++ip;
                    pushb_const(progReadValue());
                    break;
                case POPB_CONST: ++ip;
                    popb_const(progReadValue());
                    break;
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
        opStack.push_back((uintptr_t) addr);
    }

    Value popVal()
    {
        Value val = opStack.back();
        opStack.pop_back();
        return val;
    }

    Addr popAddr()
    {
        Addr addr = (Addr) (uintptr_t) opStack.back();
        opStack.pop_back();
        return addr;
    }

    int popIval()
    {
        return (int) popVal();
    }

    // *********
    // * STACK *
    // *********

    void pushb_const(Value bytes)
    {
        sp += (int) bytes;
    }

    void popb_const(Value bytes)
    {
        sp -= (int) bytes;
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
        ip = (uint8_t*) addr;
    }

    void jmp()
    {
        ip = (uint8_t*) popAddr();
    }

    void je()
    {
        if (popVal() == 0)
            ip = (uint8_t*) popAddr();
        else
            popAddr();
    }

    void jne()
    {
        if (popVal() != 0)
            ip = (uint8_t*) popAddr();
        else
            popAddr();
    }

    void jgt()
    {
        if (popVal() > 0)
            ip = (uint8_t*) popAddr();
        else
            popAddr();
    }

    void jlt()
    {
        if (popVal() < 0)
            ip = (uint8_t*) popAddr();
        else
            popAddr();
    }

    void jlet()
    {
        if (popVal() <= 0)
            ip = (uint8_t*) popAddr();
        else
            popAddr();
    }

    void jget()
    {
        if (popVal() >= 0)
            ip = (uint8_t*) popAddr();
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
        load_val_const(a & b);
    }

    void bor()
    {
        int b = popIval();
        int a = popIval();
        load_val_const(a | b);
    }

    void bxor()
    {
        int b = popIval();
        int a = popIval();
        load_val_const(a ^ b);
    }

    void bsl1()
    {
        load_val_const(popIval() << 1);
    }

    void bsr1()
    {
        load_val_const(popIval() >> 1);
    }

    void bsl()
    {
        int b = popIval();
        int a = popIval();
        load_val_const(a << b);
    }

    void bsr()
    {
        int b = popIval();
        int a = popIval();
        load_val_const(a >> b);
    }

    // **************
    // * ARITHMETIC *
    // **************

    void add()
    {
        Value b = popVal();
        Value a = popVal();
        load_val_const(a + b);
    }

    void sub()
    {
        Value b = popVal();
        Value a = popVal();
        load_val_const(a - b);
    }

    void mul()
    {
        Value b = popVal();
        Value a = popVal();
        load_val_const(a * b);
    }

    void div()
    {
        Value b = popVal();
        Value a = popVal();
        load_val_const(a / b);
    }

    void mod()
    {
        int b = popIval();
        int a = popIval();
        load_val_const(a % b);
    }

    // ************
    // * LOAD OPS *
    // ************

    void load_uchar()
    {
        pushVal(*(unsigned char*) popAddr());
    }

    void load_ushort()
    {
        pushVal(*(unsigned short*) popAddr());
    }

    void load_ulong()
    {
        pushVal(*(unsigned long*) popAddr());
    }

    void load_uint()
    {
        pushVal(*(unsigned int*) popAddr());
    }

    void load_char()
    {
        pushVal(*(char*) popAddr());
    }

    void load_short()
    {
        pushVal(*(short*) popAddr());
    }

    void load_long()
    {
        pushVal(*(long*) popAddr());
    }

    void load_int()
    {
        pushVal(*(int*) popAddr());
    }

    void load_float()
    {
        pushVal(*(float*) popAddr());
    }

    void load_double()
    {
        pushVal(*(double*) popAddr());
    }

    void load_addr()
    {
        pushVal((uintptr_t)*(Addr*) popAddr());
    }

    void load_stack_offs_const(Value offs)
    {
        pushVal((uintptr_t) sp + offs);
    }

    void load_val_const(Value lit)
    {
        pushVal(lit);
    }

    void load_addr_const(Addr addr)
    {
        pushAddr(addr);
    }

    // *************
    // * STORE OPS *
    // *************

    void store_uchar()
    {
        unsigned char* dest = (unsigned char*) popAddr();
        *dest = (unsigned char) popVal();
    }

    void store_ushort()
    {
        unsigned short* dest = (unsigned short*) popAddr();
        *dest = (unsigned short) popVal();
    }

    void store_ulong()
    {
        unsigned long* dest = (unsigned long*) popAddr();
        *dest = (unsigned long) popVal();
    }

    void store_uint()
    {
        unsigned int* dest = (unsigned int*) popAddr();
        *dest = (unsigned int) popVal();
    }

    void store_char()
    {
        char* dest = (char*) popAddr();
        *dest = (char) popVal();
    }

    void store_short()
    {
        short* dest = (short*) popAddr();
        *dest = (short) popVal();
    }

    void store_long()
    {
        long* dest = (long*) popAddr();
        *dest = (long) popVal();
    }

    void store_int()
    {
        int* dest = (int*) popAddr();
        *dest = (int) popVal();
    }

    void store_float()
    {
        float* dest = (float*) popAddr();
        *dest = (float) popVal();
    }

    void store_double()
    {
        double* dest = (double*) popAddr();
        *dest = popVal();
    }

    void store_addr()
    {
        Addr* dest = (Addr*) popAddr();
        *dest = (Addr) (uintptr_t) popVal();
    }
};

#endif