
#ifndef AKIN_INCLUDED
#define AKIN_INCLUDED

#include "tree.h"
#include "PARSED_F_funcs.h"

/* CONSTS */

/* how many new symbols to add = BUFF_ALL_MUL_ * MAX_STR_LEN_ */
const size_t        BUFF_ALL_MUL_           = 10;
/* initial num of buffers */
const size_t        INIT_NUMOF_BUFFS_       = 10;
/* descr max size */
const size_t        DESCR_MAX_SIZE_         = 200;
/* def ans for empty tree */
const char          DEF_ANS_[MAX_STR_LEN_]  = "unknown shit";


/* obj descr structure */
struct descr_t {

    /* obj node ptr */
    node_t* node_;

    /* obj properties */
    char* properties_[DESCR_MAX_SIZE_] = { nullptr };

    /* 1 - have prop; 0 - don't have prop */
    bool  prop_mask_ [DESCR_MAX_SIZE_] = { 0 };

    /* info about found properties */
    bool fals_prop_found               = false;
    bool true_prop_found               = false;

};

/* akinator structure */
struct akin_t
{

    /* tree with akin data */
    tree_t          tree_;

    /* file with text */
    PARSED_FILE_t   file_           = { nullptr, nullptr, nullptr, 0, 0 };

    /* buffs with data */
    char**          buffs_          = nullptr;

    /* curr buff shifh */
    size_t          buff_shift_     = 0;

    /* num of buffs */
    size_t          numof_buffs_    = INIT_NUMOF_BUFFS_;

    /* curr working buff id */
    size_t          curr_buff_id_   = 1;

};

/* shift to get to tree_ in akin struct */
const size_t AKIN_TR_SHT_ = (size_t)&(((akin_t* )nullptr)->tree_);

/* shift to get to PARSED_FILE in akin struct */
const size_t AKIN_PF_SHT_ = (size_t)&(((akin_t* )nullptr)->file_);

/* to get tree ptr */
tree_t*         akin_get_treep( const akin_t* const akin );

/* to get file ptr */
PARSED_FILE_t*  akin_get_filep( const akin_t* const akin );

/* constructor and destructor */
err_t akin_ctor( akin_t* const akin, const char* const dump_file_name = "akinator_dump.pdf" );
err_t akin_dstr( akin_t* const akin );

/* write descr of akinator to file */
err_t akin_print( const akin_t* const akin, const char* const print_file_name );

/* method to start akinator session */
err_t akin_menu( akin_t* const akin );

/* method to compare obj */
err_t akin_compare( const akin_t* const akin );

/* method to describe obj */
err_t akin_describe( const akin_t* const akin );

/* method to play game round */
err_t akin_play_round( akin_t* const akin );

/* method to get file name from stdin and write tree data to file with sended name */
err_t akin_get_name_n_write_2_file( const akin_t* const akin );

/* method to clear tree */
err_t akin_clear_tree( akin_t* const akin );

/* method to upload new tree */
err_t akin_upload_new( akin_t* akin, const char* scan_file_name );

/* sub method to get all info about obj */
err_t akin_form_descr( const akin_t* const akin, descr_t* const descr, const char* const name );

/* metod to add new data nodes */
err_t akin_add_nodes( akin_t* const akin, node_t* const nodep );


#ifndef NDBG

    /* DEBUG */
    #define verificator()                                           \
    {                                                               \
        if (err_t error = tree_verificator (akin_get_treep (akin))) \
        {                                                           \
            return error;                                           \
        }                                                           \
    }

#else

    /* NO DEBUG */

#endif

/* METHODS FRO SUB TREES */

    /* sub method for file constructor */
    int akin_sub_t_scan( akin_t* const akin, node_t* const nodep, int str_id );

    int akin_sub_t_print( const tree_t* const treep, const node_t* const nodep, char* const buff );

/* ********************* */

#endif