
#ifndef AKINATOR_INCLUDED
#define AKINATOR_INCLUDED

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "PARSED_F_funcs.h"

class akinator_t {

    typedef char* data_t;

public:  /* PUBLIC CONSTS */

    /* max str length */
    static const size_t max_akin_str_len_   = 100;
    /* how many new symbols to add = buff_realloc_multiplayer * max_akin_str_len */
    const size_t buff_alloc_multiplayer     = 10;
    /* initial num of buffers */
    const size_t initial_buff_num           = 10;
    /* descr max size */
    static const size_t descr_max_size_     = 200;
    /* def ans for empty tree */
    const char default_ans[max_akin_str_len_] = "UNKNOWN SHIT";

private: /* PRIVATE FIELDS */

    /* tree with akin data */
    tree_t<data_t> data_tree_;

    /* buffs with data */
    char** buffs_ = nullptr;

    /* file with text */
    PARSED_FILE_t file_ = { nullptr, nullptr, nullptr, 0, 0 };

    /* curr buff shifh */
    size_t buff_shift_  = 0;

    /* curr working buff id */
    size_t curr_buff_id = 1;

private: /* PRIVATE DATA STRUCTS */

    /* obj descr structure */
    struct descr_t {

        /* node with obj name ref */
        node_ptr<data_t> node_;

        /* obj properties */
        char* properties_[descr_max_size_] = { nullptr };

        /* 1 - have prop; 0 - don't have prop */
        bool  prop_mask_ [descr_max_size_] = { 0 };

        size_t size_                       = 0;

        /* info about found properties */
        bool fals_prop_found               = false;
        bool true_prop_found               = false;
    };

public:  /* PUBLIC METHODS */

    /* constructor and destructor */
    akinator_t( char const*  dump_file_name = "akinator_dump.pdf" );
   ~akinator_t();

    /* copy constructor and = operator are deleted for safety */
    akinator_t( const akinator_t &tocopy )                  = delete;
    akinator_t& operator = ( const akinator_t& tocopy )     = delete;

    /* write descr of akinator to file */
    err_t print( const char const*  descr_file_name );

    /* method to start akinator session */
    err_t menu();

private: /* PRIVATE METHODS */

    /* METHODS FRO SUB TREES */

        /* sub method for file constructor */
        int sub_tree_scan( node_ptr<data_t> root, int str_id );

        int sub_tree_print( node_ptr<data_t> root, char* buff );

    /* ********************* */

    /* method to compare obj */
    err_t compare();

    /* method to describe obj */
    err_t describe();

    /* method to play game round */
    err_t play_round();

    /* method to get file name from stdin and write tree data to file with sended name */
    err_t get_name_n_write_2_file();

    /* method to clear tree */
    err_t clear_tree();

    /* method to upload new tree */
    err_t upload_new( const char* );

    /* method to get confr from user */
    err_t get_confirmation();

    /* sub method to get all info about obj */
    err_t form_descr( descr_t& descr, char* name );

    /* metod to add new data nodes */
    err_t add_nodes( node_ptr<data_t> node );

    /* method to get str form frame */
    size_t get_str( FILE* frame, char* buff, size_t max_len );

    #define verificator()                               \
    {                                                   \
        data_tree_.verificator();                       \
                                                        \
        if ((bool)(data_tree_.get_err()))               \
        {                                               \
            return data_tree_.get_err();                \
        }                                               \
    }

    #define err_chk( type )                             \
    {                                                   \
        if ((bool) data_tree_.get_err())                \
        {                                               \
            return (type) data_tree_.get_err();         \
        }                                               \
    }

};

#include "akinator.cpp"

#endif