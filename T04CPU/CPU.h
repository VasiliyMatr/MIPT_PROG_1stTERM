#pragma once

#include <math.h>

#include "DEVE_STACK.h"
#include "USER_STACK.h"
#include "PARSED_F_funcs.h"
#include "utils.h"

class CPU_t// new גלוסעמ calloc
{
  private:
    
    static const size_t NUMOF_REG          = 8;

    STACK_t*  stack                        = nullptr;

    STACK_t*  call_stack                   = nullptr;

#ifndef NDBG 

    const DBG_MODE_t DBG_MODE              = NO_DBG;

    const char* const log_name             = "\0";

#endif

    char*        asm_code                  = nullptr;
    size_t       code_length               = 0;

    // generating cmd codes
    enum CMD_CODES {

        #define DEF_CMD( name, num, cpu_act, asm_act, dasm_act ) \
            CMD_##name = num,

        #include "../MY_LIB/ASM_CMD.h"
    
        #undef DEF_CMD
    };

  public: 

    // for info about erros for user
    ERR_t ERROR_CODE                    = OK;
    
    // constructor and destructor
    CPU_t( const size_t sizeof_storage, const char* num_stk_log_name, const char* call_stk_log_name, DBG_MODE_t DBG_MODE );
    ~CPU_t();

    // can't copy CPU with same stack log file
    CPU_t ( const CPU_t& tocopy ) = delete;

    // copy constructor, but with new log file
    CPU_t( const CPU_t& tocopy, const char* num_stk_log_name, const char* call_stk_log_name );

    // no = operator
    CPU_t & operator = ( const CPU_t& tocopy ) = delete;

    void upload_program( const char* file_name );

    void do_program();

    void error_output();
};