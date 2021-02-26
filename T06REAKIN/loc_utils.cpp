
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

err_t get_data_str( data_t data, char* buff )
{
    sprintf (buff, "%.*s", MAX_STR_LEN_, data);

    return OK;
}

int compare_data_f ( const data_t* const datap, const data_t* const to_compare )
{
    if (!strcmp (*datap, *to_compare))
    {
        return 0;
    }

    return 1;
}

size_t get_line( char* buff, size_t max_len )
{
    size_t char_id = 0;

    /* skipping trash */
    scanf(" ");
        
    /* scaning symbols */
    for (; char_id < max_len; char_id++)
    {
        scanf ("%c", buff + char_id);
        
        /* end of str found */
        if (*(buff + char_id) == '\n')
        {
            return char_id;
        }
    }

    fseek (stdin, 0, SEEK_END);

    return char_id;
}

size_t get_str( char* buff, size_t max_len )
{
    size_t char_id = 0;

    /* getting str from sended frame until succsess */
    while (true)
    {
        char_id = get_line (buff, max_len);

        /* str found */
        if (char_id > 0 && char_id < max_len)
        {
            /* zero in the end of str */
            *(buff + char_id) = '\0';

            /* return num of readen symbols */
            return char_id + 1;
        }

        /* stupid user */
        printf("\n\ninput normal str:");

    }
}

err_t get_confirmation()
{
    char ch = '\0';

    printf ("\n\n" "are you shure?" "\n\n" "y - yes\n");

    /* getting the confirmation */
    scanf (" %c", &ch);
    if (ch == 'y' || ch == 'Y')
    {
        return OK;
    }

    return CONF_ERR;
}

#define PRINT_ERR( error )                  \
                                            \
case error:                                 \
{                                           \
    fprintf (stderr, " %s" "\n\n", #error); \
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
        PRINT_ERR (BODY_ERR       );

        default:
            fprintf (stderr, "\n\n");
    }
}