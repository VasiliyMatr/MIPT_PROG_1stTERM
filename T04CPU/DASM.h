#pragma once

#include <math.h>

#include "utils.h"
#include "safe_stash.h"
#include "PARSED_F_funcs.h"
#include "ASM.h"

class DASM_t
{
  private:

    // max length of file name
    static const size_t name_max_size           = ASM_t::name_max_size;
    // max length of asm code str
    static const size_t lex_max_size            = ASM_t::lex_max_size;    
    // max length of lable name
    static const size_t lable_name_max_size     = ASM_t::lable_name_max_size;
    // max num of lables
    static const size_t max_numof_lables        = 10;//ASM_t::max_numof_lables;
    // max num of functions
    static const size_t max_numof_functions     = 10;//ASM_t::max_numof_functions;

    // prefix for dasm file
    const char dasm_pref[10] = "dasm_\0";

    // lable struct
    struct lable {
        int position                        = -1;
    };

    // func struct
    struct function {
        int begin                           = -1;
        int end                             = -1;
    };

    // generating cmd codes
    enum CMD_CODES {

        #define DEF_CMD( name, num, cpu_act, asm_act, dasm_act ) \
        CMD_##name = num,

        #include "../MY_LIB/ASM_CMD.h"
    
        #undef DEF_CMD
    };

    // to store info about lables
    lable lables[max_numof_lables];
    // to store info about functions
    function functions[max_numof_functions];


    // file name is stored to generate output file name
    char file_name [name_max_size];
    // code
    char* code;
    // size of deasembling code in bytes
    size_t code_size;

    // num of functions and lables
    size_t numof_lables                     = 0;
    size_t numof_functions                  = 0;


    // methods
    ERR_t open_file( const char* file_name );

    char* file_name_cat( char* const frs_str, const char* sec_str );
    
    size_t  get_function( size_t shift );
     
    size_t  get_lable( size_t shift );

    ERR_t search_funcs_n_lables();
    
    static int lable_cmpr( const void* lable1, const void* lable2 );

    static int function_cmpr( const void* func1, const void* func2 );

  public:

    size_t err_rip_pose = 0;

    ERR_t ERROR_CODE = OK;

    void dasm( const char* file_name );
};

