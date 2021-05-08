
#ifndef CPU_INCLUDED
#define CPU_INCLUDED

#include "utils.h"

#include "local_utils.h"
/* all project dbg off/on (to use less memory in release) */
#define PRJ_STK_DBG_ON_ true
#include "stack.h"

typedef int Reg_t;
const size_t REG_NUM_ = 0x10;

struct Processor
{
    /* exec code info */
        int* code_ = nullptr;
        size_t code_size_ = 0;

    /* flags vecor */
        bool EqualFlag_ = 0;
        bool AboveFlag_ = 0;
        bool LessFlag_  = 0;

    /* users registers */
        Reg_t Regs_[REG_NUM_] = {};
    /* proc stack */
        Stack<int> stk_ = Stack<int>(AbstractStack::dbgMode_t::DBG_CAN_);

    /* memory */
        int RAM_[0x10000];
};

/* to load programm */
    err_t cpu_load( Processor* CPU, const char* const in_file_name );
/* to exec programm */
    err_t cpu_exec( Processor CPU );
    
#endif