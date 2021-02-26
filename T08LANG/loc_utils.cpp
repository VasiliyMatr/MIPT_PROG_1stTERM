
#include "loc_utils.h"

bool isBadPtr( const void* ptr )
{
    MEMORY_BASIC_INFORMATION mbi = {0};
    if (::VirtualQuery(ptr, &mbi, sizeof(mbi)))
    {
        DWORD mask = (PAGE_READONLY|PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY);
        bool bad = !(mbi.Protect & mask);
        // check the page is not a guard page
        if (mbi.Protect & (PAGE_GUARD|PAGE_NOACCESS)) bad = true;

        return bad;
    }
    return true;
}

#define PRINT_ERR( error )                  \
                                            \
case error:                                 \
{                                           \
    fprintf (stderr, "%s" "\n\n", #error);  \
    break;                                  \
}

void error_prt (err_t error)
{
    switch (error)
    {
        PRINT_ERR (OK             );
        PRINT_ERR (MEM_ERR        );
        PRINT_ERR (SYNT_ERR       );
        PRINT_ERR (BAD_PTR        );
        PRINT_ERR (BAD_INIT_ERR   );
        PRINT_ERR (ROOT_MATCH_ERR );
        PRINT_ERR (ADD_ERR        );
        PRINT_ERR (FILE_ERR       );
        PRINT_ERR (CONF_ERR       );
        PRINT_ERR (LEAF_ERR       );
        PRINT_ERR (BAD_ERR_PTR    );
        PRINT_ERR (ROOT_ERR       );
        PRINT_ERR (LEFT_ERR       );
        PRINT_ERR (RIGH_ERR       );
        PRINT_ERR (PARENT_ERR     );
        PRINT_ERR (UNDEF_ERR      );
    }
}