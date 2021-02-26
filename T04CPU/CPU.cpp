#include "CPU.h"

#ifndef NDBG

CPU_t::CPU_t( const size_t sizeof_storage, const char* num_stk_log_name, const char* call_stk_log_name, DBG_MODE_t DBG_MODE )
    : DBG_MODE (DBG_MODE), log_name (log_name), 
    stack       (stack_construct (sizeof_storage, sizeof (double)   , num_stk_log_name  , DBG_MODE)), 
    call_stack  (stack_construct (sizeof_storage, sizeof (int)      , call_stk_log_name , DBG_MODE))

#else

CPU_t::CPU_t( const size_t sizeof_storage, const char* num_stk_log_name, const char* call_stk_log_name, DBG_MODE_t DBG_MODE )
    :   stack       (stack_construct (sizeof_storage, sizeof (double)   , num_stk_log_name  , DBG_MODE)), 
        call_stack  (stack_construct (sizeof_storage, sizeof (int)      , call_stk_log_name , DBG_MODE))

#endif
{
    // init error
    if (this->stack == nullptr)
    {
        if (this->call_stack != nullptr)
            stack_deconstruct (call_stack);
        this->ERROR_CODE = CPU_NO_STACK_ERROR;
        return;
    }

    else if (this->call_stack == nullptr)
    {
            stack_deconstruct (stack);
        this->ERROR_CODE = CPU_NO_STACK_ERROR;
        return;
    }
}

#ifndef NDBG

CPU_t::CPU_t( const CPU_t& tocopy, const char* num_stk_log_name, const char* call_stk_log_name )
    : DBG_MODE (tocopy.DBG_MODE), 
    stack       (stack_construct (tocopy.stack->sizeof_storage, sizeof (double) , num_stk_log_name  , tocopy.DBG_MODE)), 
    call_stack  (stack_construct (tocopy.stack->sizeof_storage, sizeof (int)    , call_stk_log_name , tocopy.DBG_MODE))

#else

CPU_t::CPU_t( const CPU_t& tocopy, const char* num_stk_log_name, const char* call_stk_log_name )
    :   stack       (stack_construct (tocopy.stack->sizeof_storage, sizeof (double) , num_stk_log_name  , FULL_DBG)), 
        call_stack  (stack_construct (tocopy.stack->sizeof_storage, sizeof (int)    , call_stk_log_name , FULL_DBG))

#endif
{
    // init error
    if (this->stack == nullptr)
    {
        if (this->call_stack != nullptr)
            stack_deconstruct (call_stack);
        this->ERROR_CODE = CPU_NO_STACK_ERROR;
        return;
    }

    else if (this->call_stack == nullptr)
    {
        stack_deconstruct (stack);
        this->ERROR_CODE = CPU_NO_STACK_ERROR;
        return;
    }

    // for fast access
    char*  code          = tocopy.asm_code;
    size_t   code_length = tocopy.code_length;

    if (code != nullptr)
    {
        char* this_code = (char* ) calloc (code_length, sizeof (double));
        if (this_code == nullptr)
        {
            this->ERROR_CODE = CPU_NO_CODE_ERROR;
            return;
        }
        memcpy (this_code, code, code_length * sizeof (double));

        this->asm_code = this_code;
    }

    this->code_length = code_length;
}

CPU_t::~CPU_t()
{
    if (isBadReadPtr (this))
        return;

    // for fast access
    STACK_t* stack      = this->stack;
    STACK_t* call_stack = this->call_stack;
    char*    code       = this->asm_code;

    if (!isBadReadPtr(stack))
    {
        stack_deconstruct(stack);
        this->stack = nullptr;
    }

    if (!isBadReadPtr(call_stack))
    {
        stack_deconstruct(call_stack);
        this->call_stack = nullptr;
    }

    if (!isBadReadPtr(asm_code))
    {
        free(asm_code);
        this->asm_code = nullptr;
    }

}

void CPU_t::upload_program( const char* file_name )
{
    if (isBadReadPtr (this))
        return;

    if (isBadReadPtr (file_name))
    { 
        this->ERROR_CODE = MINOR_ERROR;
        return;
    }  

    // file to read from
    FILE*   asm_file;
    // buffer to write to
    char* asm_code;

    if ((asm_file = fopen (file_name, "rb")) == nullptr)
    {
        this->ERROR_CODE = FILE_ERR;
        return;
    }
    size_t numof_bytes = get_length (asm_file);

    // allocating memory for file buffer
    asm_code = (char* )calloc (numof_bytes, sizeof (char));
    if (asm_code == nullptr)
    {
        fclose (asm_file);
        this->ERROR_CODE = CPU_NO_CODE_ERROR;
        return;
    }

    // reading data from file
    fread (asm_code, sizeof (char), numof_bytes, asm_file);

    // wirting data to CPU_t class obj
    this->code_length = numof_bytes;
    this->asm_code = asm_code;

    fclose (asm_file);

    return;
}

void CPU_t::do_program()
{    
    if (isBadReadPtr (this))
        return;
        
    /* for fast access */ 
    char*  code                     = this->asm_code;
    STACK_t* stack                  = this->stack;
    STACK_t* call_stack             = this->call_stack;
    double   REGISTERS[NUMOF_REG];

    // for easier debugging
    for (int i = 0; i < NUMOF_REG; i++)
        REGISTERS[i] = NAN;

    // num of numbers in exe file
    const size_t code_length = this->code_length;

    if (isBadReadPtr (asm_code))
    { 
        this->ERROR_CODE = CPU_NO_CODE_ERROR;
        return;
    }

    if (isBadReadPtr (stack) || isBadReadPtr (call_stack))
    {
        this->ERROR_CODE = CPU_NO_STACK_ERROR;
        return;
    }

    // code of current cmd
    double  cmd_code  = NAN;
    
    // given cmd argument
    double  number[1] = { NAN };
    
    // number of readen code symbols
    size_t  numof_readen_symbols    = 0;

    // for popable values from stack
    double  value1 = NAN;
    double  value2 = NAN;

    // poping all values from stack
    while (true)
    {
        double val  = NAN;
        int    adr  = 0;

        ERR_t error_val  = stack_pop (stack      , &val);
        ERR_t error_call = stack_pop (call_stack , &adr);

        if (error_val == EMPT && error_call == EMPT) break;
        
        if (error_val != OK &&  error_val != EMPT)
        {
            this->ERROR_CODE = error_val;
            return;
        }

        if (error_call != OK && error_call != EMPT)
        {
            this->ERROR_CODE = error_call;
            return;
        }
    }

    // executing program
    for (size_t rip = 0; rip < code_length;)
    { 
        // reading cmd and incr rip
        int cmd = *(int* )(code + rip);
        rip += sizeof (int);

        // checking cmd id without modifiers (only last byte)
         switch (cmd & 0x3F)
        {
            #define DEF_CMD( name, num, cpu_act, asm_act, dasm_act )\
            case CMD_##name:                                        \
            {                                                       \
                cpu_act                                             \
                break;                                              \
            }                             

            #include "../MY_LIB/ASM_CMD.h"   
            
            #undef DEF_CMD
        }
    }

    return;
}

void CPU_t::error_output()
{
    switch (this->ERROR_CODE)
    {
        case OK:
            printf ("\nno errors!\n");
            return;
        case CPU_NO_STACK_ERROR:
            printf ("\nerror with cpu stack init!\n");
            return;
        case FILE_ERR:
            printf ("\nerror with file!\n");
            return;
        case MEM_ERR:
            printf ("\nerror with memory allocation\n");
            return;
        case MINOR_ERROR:
            printf ("\nminor error, like bad ptr to file name\n");
            return;
        case CPU_NO_CODE_ERROR:
            printf ("\nerror with cpu code init\n");
            return;
        default:
            printf ("\nunknown or stack data error, look for log file!\n");
            stack_out (this->stack);
            stack_out (this->call_stack);
            return;
    }
}