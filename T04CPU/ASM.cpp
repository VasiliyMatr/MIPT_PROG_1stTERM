#include "ASM.h"
#undef ERROR

const int name_max_size = 100;

enum CMD_CODES {

    #define DEF_CMD( name, number, to_do, compile_check ) \
    CMD_##name = number,

    #include "../MY_LIB/ASM_CMD.h"
    
    #undef DEF_CMD
};

const size_t lex_max_size = 80;

struct ASM_t {

    char file_name [name_max_size];

    PARSED_FILE_t code;
};

const size_t pf_shift = (size_t)&((ASM_t*)NULL)->code;

ASM_t* asm_constructor() 
{
    ASM_t* ASM = (ASM_t* )calloc (1, sizeof (ASM_t));

    if (ASM == NULL)
        return NULL;

    return ASM;
}

ERR_t asm_destructor( ASM_t* const ASM )
{
    ASM_ASSERT (ASM);

    free (ASM);
    
    return OK;
}

ERR_t asm_open_file( ASM_t* const ASM, const char* file_name )
{
    ASM_ASSERT (ASM);
    if (isBadReadPtr (file_name))
        return FILE_ERR;

    if (ERR_t ERROR = get_parsed_text ((PARSED_FILE_t* )((int)ASM + pf_shift), file_name))
        return ERROR;

    strncpy (ASM->file_name, file_name, name_max_size);

    return OK;
}

char* file_name_cat( char* frs_str, const char* sec_str )
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
        return NULL;

    // copying name
    char name[name_max_size]    = "\0";
    strcpy (name, (const char* )((int)frs_str + name_pose));

    frs_str[name_pose]      = '\0';

    // cancat name
    return strcat (strcat (frs_str, sec_str), name); 
}

// prefixes for file names
const char asm_pref[] = "asm_";
const char lst_pref[] = "lst_";

ERR_t asm_compile( ASM_t* const ASM, const char* file_name )
{
    ASM_ASSERT  (ASM);
    if (isBadReadPtr (file_name))
        return FILE_ERR;

    if (ERR_t ERROR = asm_open_file (ASM, file_name))
        return ERROR;

    // for fast access
    PARSED_FILE_t code = ASM->code;
    STR* strs = code.strs;

    // buffers for data to write if files
    char*   lst_buff = NULL;
    double* asm_buff = NULL;

    // allocating memory for buffers
    asm_buff = (double* )calloc (sizeof (double) * 2 , code.num_of_s);
    if (asm_buff == NULL)
    {
        pf_close ((PARSED_FILE_t* )((int)ASM + pf_shift));
        return MEM_ERR;
    }

    lst_buff = (char*   )calloc (lex_max_size, code.num_of_s);
    if (lst_buff == NULL)
    {
        free (asm_buff);
        pf_close ((PARSED_FILE_t* )((int)ASM + pf_shift));
        return MEM_ERR;
    }

    // sifts for buffers
    size_t lst_shift = 0;
    size_t asm_shift = 0;

    // cmd name
    char CMD_STR[10];
    
    // cmd argument
    double argument = NAN;

    // num of readen args form curr str with cmd
    int numof_readen_args = 0;

    // writing cmds info in buffers
    for (size_t cmd_id = 0; cmd_id < code.num_of_s; cmd_id++)
    {
        numof_readen_args = sscanf (strs[cmd_id].str, " %s %lf", CMD_STR, &argument);

        // cmd define
        #define DEF_CMD( name, num, to_do, numof_args )                                                                                     \
        if (!strcmp (CMD_STR, #name))                                                                                                       \
        {                                                                                                                                   \
            if (numof_readen_args - 1 != numof_args)                                                                                        \
            {                                                                                                                               \
                printf ("FUCK YOU!\n");                                                                                                     \
                return ASM_CMPL_ERROR;                                                                                                      \
            }                                                                                                                               \
            if (ERR_t ERROR = print_info (asm_buff + asm_shift, lst_buff + lst_shift, #name, argument, num, numof_readen_args, &lst_shift)) \
                return ERROR;                                                                                                               \
            asm_shift += numof_readen_args;                                                                                                 \
        }

        #include "../MY_LIB/ASM_CMD.h"		 

        #undef DEF_CMD
    }

    // names for files
    char asm_name[name_max_size + 10] = "\0";
    char lst_name[name_max_size + 10] = "\0";

    // open files
    FILE* bin_code_file = fopen (file_name_cat (strcpy (asm_name, file_name), asm_pref), "wb");
    if (bin_code_file == NULL)
        return FILE_ERR;

    FILE* lst_code_file = fopen (file_name_cat (strcpy (lst_name, file_name), lst_pref), "wb");
    if (lst_code_file == NULL)
    {
        fclose (bin_code_file);
        return FILE_ERR;
    }

    // sending data to files
    fwrite (lst_buff, sizeof (char)    , lst_shift, lst_code_file);
    fwrite (asm_buff, sizeof (double)  , asm_shift, bin_code_file);
    
    // close files
    fclose (bin_code_file);
    fclose (lst_code_file);

    // free buffers
    free (lst_buff);
    free (asm_buff);
    
    return OK;
}

ERR_t print_info( double* const asm_buff, char* const lst_buff, const char* name, const double argument, const int num, const int numof_readen_args, size_t* const lst_shft )
{
    // checking ptrs
    if (isBadReadPtr (asm_buff) || isBadReadPtr (lst_buff))
        return FILE_ERR;

    // sending cmd code to as buffer
    *asm_buff = (double)num;

    if (numof_readen_args == 2)                                      
    {
        // sending arg to asm buff
        *(double* )((long)asm_buff + sizeof(double)) = argument     ;
        // sending info to lst buff
        *lst_shft += sprintf (lst_buff, "%d %lf | %s\n", num, argument, name)   ;
    }                                                                
    else                                                             
    {                              
        // sending info to list buff
        *lst_shft += sprintf (lst_buff, "%d     | %s\n", num, name)             ;  
    }                                         

    return OK;
}