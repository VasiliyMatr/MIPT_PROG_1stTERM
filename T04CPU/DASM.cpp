#include "DASM.h"

ERR_t DASM_t::open_file( const char* file_name )
{
    // opening file with data
    FILE* dasm_file = fopen(file_name, "rb");
    if (dasm_file == nullptr)
        return FILE_ERR;

    // getting lenght of file with data
    this->code_size = get_length (dasm_file);
    if (this->code_size <= 0)
        return FILE_ERR;

    // allocating memory for data
    this->code = (char* )calloc (1, this->code_size);
    if (this->code == nullptr)
    {
        fclose(dasm_file);
        return MEM_ERR;
    }

    // reading file and closing it
    fread  (this->code, sizeof (char), this->code_size, dasm_file);
    fclose (dasm_file);

    // copying file name
    strncpy (this->file_name, file_name, name_max_size);
    
    return OK;
}


char* DASM_t::file_name_cat( char* frs_str, const char* sec_str )
{
    assert (!isBadReadPtr (frs_str));
    assert (!isBadReadPtr (sec_str));

    size_t name_pose = 0;
    size_t i = 0;

    // searching for file name
    for (; frs_str[i] != '\0'; i++)
    {
        if (frs_str[i] == '\\')
            name_pose = i + 1;
    }

    // no file name
    if (name_pose == i)
        return nullptr;

    // copying name
    char name[name_max_size]    = "\0";
    strcpy (name, (const char* )((int)frs_str + name_pose));

    frs_str[name_pose]          = '\0';

    // cancat name
    return strcat (strcat (frs_str, sec_str), name); 
}

void DASM_t::dasm( const char* file_name )
{
    if (isBadReadPtr (file_name))
    { 
        this->ERROR_CODE = FILE_ERR;
        return;
    }
    // reading data from file with given name
    if (ERR_t error = open_file (file_name))
    { 
        this->ERROR_CODE = error;
        return;
    }

    // for fast access
    char*   code      = this->code;
    size_t  code_size = this->code_size;
    
    // buffer for info to write in file
    char* dasm_buff = (char* )calloc (lex_max_size, code_size / sizeof (int));
    if (dasm_buff == nullptr)
    {
        free (code);
        this->ERROR_CODE = MEM_ERR;
        return;
    }

    // buffer shift
    size_t shift = 0;
    // id of curr lable
    size_t lable_id = 0;
    size_t func_id  = 0;

    search_funcs_n_lables ();

    // writing data to buffer
    for (size_t rip = 0; rip < code_size;)
    {

        // printing info about lables and functions declarations
        if (lables[lable_id].position == rip)
        {
            shift += sprintf (dasm_buff + shift, "lable%d : \n", lable_id);
            lable_id++;
        }

        if (functions[func_id].begin == rip)
        {
            shift += sprintf (dasm_buff + shift, "FUNC function%d \n", func_id);
        }

        if (functions[func_id].end == rip)
        {
            shift += sprintf (dasm_buff + shift, "END \n", func_id);
            func_id++;
        }
     
        // reading cmd and incr rip
        int cmd = *(int* )(code + rip);
        rip += sizeof (int);

        // checking cmd id without modifiers (only last byte)
        switch (cmd & 0xFF)
        { 
            #define DEF_CMD( name, num, cpu_act, asm_act, dasm_act )\
            {                                                       \
                case num:                                           \
                    dasm_act                                        \
                    break;                                          \
            }                                                                                                       

            #include "../MY_LIB/ASM_CMD.h"

            #undef DEF_CMD
        }
    }

    char dasm_name[name_max_size + 10] = "\0";

    // open file to write data in
    FILE* dasm_code_file = fopen (file_name_cat (strcpy (dasm_name, file_name), dasm_pref), "wb");
    if (dasm_code_file == NULL)
    {
        free (dasm_buff);
        free (code);
        this->ERROR_CODE = MEM_ERR;
        return;
    }

    fwrite (dasm_buff, sizeof (char), shift, dasm_code_file);

    fclose (dasm_code_file);

    return;
}

size_t DASM_t::get_lable( size_t shift )
{
    size_t lable_id = 0;
    size_t max_numof_lables = this->max_numof_lables;

    for (; lable_id < max_numof_lables; lable_id++)
    {
        lable curr_lable = lables[lable_id];
        if (curr_lable.position == shift)
            return lable_id + 1;
        if (curr_lable.position == -1)
            return 0;
    }
}

size_t DASM_t::get_function( size_t shift )
{
    size_t func_id = 0;
    size_t max_numof_functions = this->max_numof_functions;

    for (; func_id < max_numof_functions; func_id++)
    {
        function curr_func = functions[func_id];
        if (curr_func.begin == shift)
            return func_id + 1;
        if (curr_func.begin == -1)
            return 0;
    }
}

ERR_t DASM_t::search_funcs_n_lables()
{
    // for fast access
    char* code          = this->code;
    size_t code_size    = this->code_size;

    // id of curr lable and function
    size_t lable_id     = 0;
    size_t func_id      = 0;

    bool in_func        = false;
    bool ret_found      = true;

    // cmd name
    char CMD_STR[lex_max_size + 1] = "\0";

    // cmd argument str
    char ARG_STR[lex_max_size + 1] = "\0";

    // searching for lables and functions declarations
    for (size_t rip = 0; rip < code_size;)
    {
        // reading cmd and incr rip
        int cmd = *(int* )(code + rip);
        rip += sizeof (int);

        switch (cmd & 0xFF)
        {
            case CMD_CALL:
            {

                // no more space for functions
                if (func_id >= max_numof_functions)
                {
                    this->err_rip_pose = rip;
                    return ASM_FUNC_NUM_ERR;
                }

                // copying data
                functions[func_id].begin = cmd >> 8;

                break;
            }

            // in case of jumps
            case CMD_JMP:
            case CMD_JA:
            case CMD_JE:
            case CMD_JNE:
            {
                // no more space for lables
                if (lable_id >= max_numof_lables)
                {
                    this->err_rip_pose = rip;
                    return ASM_FUNC_NUM_ERR;
                }

                // copying data
                lables[lable_id].position = cmd >> 8;

                break;
            }

            case CMD_PUSH:
            {
                if (cmd >> 8 == 0)
                {
                    rip += sizeof (double);
                }
            }
        }
    }

    
    // saving info
    numof_lables    = lable_id + 1;
    numof_functions = func_id + 1;

    qsort (lables, lable_id + 1, sizeof (lable), lable_cmpr);
    qsort (functions, func_id + 1, sizeof (function), function_cmpr);

    return OK;
}

int DASM_t::lable_cmpr( const void* lable1, const void* lable2 )
{
    int pose1 = ((lable* )lable1)->position;
    int pose2 = ((lable* )lable2)->position;

    if (pose1 < 0) return  1;
    if (pose2 < 0) return -1;

    return (int)pose1- pose2;
}

int DASM_t::function_cmpr( const void* func1, const void* func2 )
{
    int pose1 = ((function* )func1)->begin;
    int pose2 = ((function* )func1)->begin;

    if (pose1 < 0) return  1;
    if (pose2 < 0) return -1;

    return (int)pose1- pose2;
}