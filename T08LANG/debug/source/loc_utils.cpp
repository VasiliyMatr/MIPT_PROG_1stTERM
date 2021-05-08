
#include "compiler.hpp"

bool isBadPtr( const void* ptr )
{
    return ptr == nullptr;
}

/* To get Error line */
size_t getErrorLine( Compiler* cmplp );
/* To get Error line begin */
char*  getErrorLineBebin( Compiler* cmplp );
/* To get Error line length */
int getErrorLineLength( char* errorLineBeginP );
/* For normal error output */
void   errorDecode( Err_t error );

void cmplErrOut( Compiler* cmplp )
{
    printf ("compilation error!" "\n");
    errorDecode (cmplp->error_info_.error_);

    /* If there is syntax error */
    if (cmplp->error_info_.error_ == SYNT_ERR_)
    {
        assert (cmplp->error_info_.location_ != -1);

        char* errorLineP = getErrorLineBebin (cmplp);

        printf ("error here:" "\n"
                "%04ld | %.*s"   "\n",
                getErrorLine (cmplp), getErrorLineLength (errorLineP), errorLineP);
    }
}

void errorDecode( Err_t error )
{
    /* Error message for diff error values print macro */
        #define ERROR_PRINT( ERROR_, MSG_ ) \
        case ERROR_:                        \
            printf (MSG_ "\n");             \
            break;

    /* Printing error info */
    switch (error)
    {
        ERROR_PRINT (OK_        , "no errors");
        ERROR_PRINT (MEM_ERR_   , "error with memory allocation");
        ERROR_PRINT (FILE_ERR_  , "error with file open");
        ERROR_PRINT (SYNT_ERR_  , "syntax error");

        default:
            printf ("comiler internal error detected." "\n"
                    "error code: %d" "\n"
                    "please, left issue note here (with error code & your program code):" "\n"
                    "https://github.com/VasiliyMatr/LANG/issues" "\n", error);
    }

    #undef ERROR_PRINT
}

size_t getErrorLine( Compiler* cmplp )
{
    assert (!isBadPtr (cmplp->code_buff_));

    size_t lineNum = 1;
    size_t charId = 0;

    for (; charId < cmplp->error_info_.location_; ++charId)
        if (cmplp->code_buff_[charId] == '\n')
            ++lineNum;

    return lineNum;
}

char*  getErrorLineBebin( Compiler* cmplp )
{
    assert (!isBadPtr (cmplp->code_buff_));

    int codeBuffShift = cmplp->error_info_.location_;

    for (; codeBuffShift > 0; --codeBuffShift)
        if (cmplp->code_buff_[codeBuffShift] == '\n')
        {
            codeBuffShift++;
            break;
        }
    
    if (codeBuffShift == -1)
        return nullptr;

    return cmplp->code_buff_ + codeBuffShift;
}

int getErrorLineLength( char* errorLineBeginP )
{
    assert (!isBadPtr (errorLineBeginP));

    int length = 0;

    while (errorLineBeginP [length++] != '\n' && errorLineBeginP [length++] != '\0');

    return length;
}

int nameCmpr(const NamePart_t ft_name[NAME_LEN_],
             const NamePart_t sd_name[NAME_LEN_])
{
    for (int names_part_id = 0; names_part_id < NAME_LEN_; names_part_id++)
    {
        if (ft_name[names_part_id] != sd_name[names_part_id])
            return ft_name[names_part_id] - sd_name[names_part_id];
    }

    return 0;
}