#ifndef UTILS_TREE
#define UTILS_TREE
#include "utils.h"

/*   output type for tree methods: 
 *
 *   poistive nums - addrs
 *
 *   negative nums - errors codes
 */
typedef int output_t;

/*   error codes
 */
enum class err_t {

    OK          =  0    ,
    /* error with memory alloation */
    MEM_ERR     = -1000 ,
    /* error with file */
    FILE_ERR            ,
    /* bad ptr error */
    BAD_PTR             ,
    /* roots dont match */
    WR_ROOT_ERR         ,
    /* conf error */
    CONF_ERR            ,
    /* syntax error */
    SYNT_ERR            ,
    /* should be leaf */
    LEAF_ERR            ,


    /* VERIFICATION ERRORS */
        ROOT_ERR        ,
        LEFT_ERR        ,
        RIGHT_ERR       ,
        FATHER_ERR      ,
    /* ******************* */

    /* undef error */
    UNDEF_ERR
};

/* overloaded akinator func to copy data in data tree */
err_t copy_data_f( char** data, char** buff )
{
    *data = *buff;

    return err_t::OK;
}

/* overloaded akinator func to get data from data tree */
char** get_data_f( char** data )
{
    return data;
}

int compare_data_f ( char** data, char** to_compare )
{
    if (!strcmp (*data, *to_compare))
    {
        return 0;
    }

    return 1;
}

const char BAD_STR[2] = "\0";

char* prt_data_f (char** data)
{
    if (!isBadReadPtr(*data))
    {
        return *data;
    }

    return (char* )BAD_STR;
}

#endif