#ifndef CALCTOR_INCLUDE
#define CALCTOR_INCLUDE

/* DECLARATIONS THAT ARE NEEDED TO BE DONE BEFORE tree.h INCLUDE ( NEEDED TYPEDEFS ARE HERE )*/

/* name type (for simplification and speed long long is used to store name info and operation code, also check DSL.h file) */
typedef             long long       NAME_;

#include "loc_utils.h"
#include <math.h>

/* data description str max len */
const size_t        DESCR_LEN_      =  64;

/* len of operation name */
const size_t        NAME_LEN_       =  sizeof (NAME_);

/* max num of variables in expr */
const size_t        NUMOF_VAR_      = 8;

/* what can be in tree nodes */
enum unit_type
{
    NUMBER      =  'N'  ,
    VARIABLE    =  'V'  ,
    OPERATION   =  'O'  ,
    UNDEF_U     =   -1
};

/* operation types */
enum name_t : NAME_
{
    #define DEF_OP( name, code, oper_inf, priority, calca, diffa ) \
    OP_##name   =  code ,

    #include "DSL.h"

    #undef DEF_OP
};

/* tree node data type */
struct exp_unit_t
{
    /* this unit type */
    unit_type   type                    = UNDEF_U   ;

    /* name, stored in correct byte order for fast print and switches */
    NAME_       name                    = OP_UNDEFINED ;
    /* double number for calculations if nedded */
    double      num                     = NAN       ;
    /* id of variable in variables massive if nedded */
    int      var_id                     = 0         ;
};

/* poison val for dump */
const exp_unit_t POISON_ = { UNDEF_U, OP_UNDEFINED, NAN, 0 };

/* typedef for tree_t struct funcs */
typedef exp_unit_t data_t;

/* func to get str with data description for graphical dump */
err_t get_data_str( const data_t data, char* const buff );

/* func to compare data */
int compare_data_f ( const data_t* const datap, const data_t* const to_compare );

/* func to copy data */
void copy_data( data_t* tocopy, data_t* dstn );

/* *********************************** */

/* !!! tree_t FUNCS INCLUDED !!! */
    #include "tree.h"

#define RJOMBA 

/* SMEJAKA */
struct joke_info {
    const tree_t* diff_treep        = nullptr;
    char*   buff                    = nullptr;
    int     buff_shift              = 0;
};

/* braces */
const char          OPN_BR_         = '(';
const char          CLS_BR_         = ')';

/* braces for latex */
const char          OPN_BR_S_[]     = "\\left(";
const char          CLS_BR_S_[]     = "\\right)";

/* calctor struct */
struct calctor_t
{
    /* tree with expr */
    tree_t tree_;

    /* variables stuff */

        /* variables names */
        NAME_   variables[NUMOF_VAR_]   = { OP_UNDEFINED };
        /* variables values */
        double  var_values[NUMOF_VAR_]  = { NAN };
        /* curr unparsed var id */
        size_t  curr_var                = 0;
        /* id of var over which differenciation is made */
        size_t  diff_var_id             = 0;

    /* *************** */

    /* shows that tree was simplyfied */
    bool simplify_flag              = false;
};

/* shift to get to calctor tree */
const size_t CALC_TR_SHT_ = (size_t)&(((calctor_t* )nullptr)->tree_);
/* func to get ptr to calc tree */
tree_t* calc_get_treep( const calctor_t* const calc );

/* !!! recursiver_descent FUNCS INCLUDED !!! */
    #include "recursive_descent.h"

/* constructor and destructor */
err_t calc_ctor( calctor_t* const calc );
err_t calc_dstr( calctor_t* const calc );

// /* func to parse str with expr */
// err_t calc_pars( calctor_t* const calc, const char* const expr );

// /* func to parse name */
// NAME_ calc_pars_name( const char* const expr, int* const sht );

/* func to get latex expr dump */
void calc_latex( const calctor_t* const calc );

/* func to print latex operation name in buff */
int get_latex_name( NAME_ name, char* buff );

/* func to get operation priority */
int calc_get_prior( NAME_ type );

/* func to check operands mask (is there are correct number and position of operation args) */
bool calc_chck_operands( const bool left_fl, const bool righ_fl, const long long op_code );

/* func to verify expr tree */
err_t calc_verify( calctor_t* calc );

/* func to calculate expr */
err_t calc_calc( calctor_t* calc );

/* func to diff expr */
err_t calc_diff( calctor_t* expr , calctor_t* diff, size_t var_id );

/* func to fold constants */
err_t calc_const_fold( calctor_t* calc, joke_info* const info = nullptr );

/* func to fold neutral elements */
err_t calc_neutral_fold ( calctor_t* calc, joke_info* const info = nullptr );

/* ahah, what a story, Mark... */
void calc_perform_joke( const char* expr );

#ifndef NDBG

/* verify define */
#define verify( calc )                              \
if (err_t error = calc_verify (calc)) return error;

#else

#define verify( calc ) ;

#endif

/* FUNCS FOR SUB TREES, SHOULD NOT BE USED BY USER!!! */

    /* to get latex expr of subtree */
    int calc_sub_t_latex( const tree_t* treep, node_t* nodep, char* const buff, int* numof_dumped_nodes_ );

    /* to check sub tree operations */
    err_t calc_sub_t_op_chk( calctor_t* calc, node_t* nodep );

    /* to get sub tree value */
    double calc_sub_t_calc( calctor_t* calc, node_t* nodep );

    /* to diff subtree */
    err_t calc_sub_t_diff( calctor_t* expr, calctor_t* diff, node_t* tocalc, node_t* nodep );

    // /* to parse sub tree */
    // int calc_sub_t_pars( calctor_t* const calc, node_t* const nodep, const char* const expr, const char** const error_ptr );

    /* sub func for calc_const_fold func */
    err_t calc_sub_t_cfold( calctor_t* calc, node_t* nodep, joke_info* const info = nullptr);

    /* sub func for calc_neutral_fold func */
    err_t calc_sub_t_nfold( calctor_t* calc, node_t* nodep, joke_info* const info = nullptr );

    /* sub func to check if there are any variables in sub tree */
    bool calc_rec_var_search(const node_t* const node);

/* ************************************************** */

/* INCLUDING SRC */
    #include "calctor.cpp"

#endif /* CALCTOR_INCLUDE */

