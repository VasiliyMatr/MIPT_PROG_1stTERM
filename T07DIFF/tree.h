
#ifndef TREE_INCLUDED
#define TREE_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "loc_utils.h"

struct tree_t;

/* tree node struct */
struct node_t
{

    /* data */
    data_t  data    = POISON_;

    /* connected nodes */
    node_t* left_ch = nullptr;
    node_t* righ_ch = nullptr;
    node_t* parent  = nullptr;

    /* root node ptr */
    tree_t* tree    = nullptr;

};

const size_t DATA_SHIFT_ = (size_t)&(((node_t* )nullptr)->data);

/* nodes sub types for debug */
enum node_sub_t
{

    /* root node */
    ROOT                ,
    /* not leaf and not root ok node */
    BODY                ,
    /* leaf node */
    LEAF                ,
    /* BAD SUBTYPES */

        /* root addr don't matches this node addr, but this node father ptr == nullptr */
        FAKE_ROOT       = ROOT_ERR   ,

        /* wrong ptr connections detected */
        WRONG_LEFT      = LEFT_ERR   ,
        WRONG_RIGHT     = RIGH_ERR   ,
        WRONG_PARENT    = PARENT_ERR ,

        /* undefined node type: this node doesn't belong this tree */
        UNDEF           = UNDEF_ERR  ,

    /* ************ */

};

/* enum to choose side in tree_add_node */
enum   side_t {

    LEFT_ =  1    ,
    RIGH_ = -1    ,
    UNDF  =  228

};

/* tree struct */
struct tree_t
{

    /* tree root ptr */
    node_t*         root_           = nullptr;

    /* number of nodes in tree */
    size_t          size_           = 0;

    /* dump file name */
    const char*     dump_name_      = nullptr;

};

/* USERS FUNCS */

/* tree constructor */
err_t tree_ctor( tree_t* const tree, err_t* const errp, const char* const dump_name = "dump.pdf");

/* tree destructor */
err_t tree_dstr( tree_t* const tree, err_t* const errp );

/* GETTERS */

    /* funcs to get connected ptrs */
    node_t* tree_get_left( const tree_t* const tree, const node_t* const nodep, err_t* const errp );
    node_t* tree_get_righ( const tree_t* const tree, const node_t* const nodep, err_t* const errp );
    node_t* tree_get_prnt( const tree_t* const tree, const node_t* const nodep, err_t* const errp );

    /* func to get data */
    data_t  tree_get_data( const tree_t* const tree, const node_t* const nodep, err_t* const errp );

/* ******* */

/* funcs to add left and right childs (with new child constr) */
node_t* tree_add_node( tree_t* const tree, node_t* const nodep, const side_t attach_side, err_t* const errp );

/* func to constr new node, connected to this tree */
node_t* tree_new_node( tree_t* const tree, const data_t* const data, err_t* const errp );

/* func to attach not connected tree nodes */
err_t tree_att_node( tree_t* const tree, node_t* const prnt, node_t* const chld, const side_t side, err_t* const errp );

/* func to set data in node */
err_t   tree_set_data( tree_t* const tree, node_t* const nodep, const data_t data, err_t* const errp );

/* func to delete sub tree with root node with sended addr */
err_t   tree_del_subt( tree_t* const tree, node_t* const nodep, err_t* const errp );

/* RECOURSIVE search for data WORKS SLOW */
node_t* tree_slow_search_for_data( const tree_t* const tree, const data_t* const data_to_search, err_t* const errp );


err_t tree_copy( const tree_t* const ft_tree, tree_t* const sd_tree, node_t* tocopy, node_t* dstn, err_t* errp );

/* DEGUG FUNCS */

    /* tree dump func */
    void tree_dump( const tree_t* const tree );

    /* tree verification func */
    err_t tree_verificator( const tree_t* const tree );

    /* func to check validity of node_t ptr */
    err_t tree_is_nodep_valid( const tree_t* const tree, const node_t* const node );

    /* func to get node sub type */
    node_sub_t tree_get_node_st( const tree_t* const tree, const node_t* const nodep );

    /* func to check error pointer */
    err_t tree_err_check( err_t* errp );

/* *********** */

#ifndef NDBG

/* DEBUG DEFINES */

    /* tree verify define */
    #define tree_verify( tree, to_ret, errp )                                           \
    {                                                                                   \
        if (err_t error = tree_verificator (tree))                                      \
        {                                                                               \
            fprintf (stderr, "\n\n" "TREE VERIFY ERROR IN %s; CODE - ", __FUNCTION__);  \
            error_prt (error);                                                          \
            tree_dump (tree);                                                           \
            *errp = error;                                                              \
            return to_ret;                                                              \
        }                                                                               \
    }

    /* node verify define */
    #define node_verify( tree, nodep, to_ret, errp )                                    \
    {                                                                                   \
        if (err_t error = tree_is_nodep_valid (tree, nodep))                            \
        {                                                                               \
            fprintf (stderr, "\n\n" "NODE VERIFY ERROR IN %s; CODE - ", __FUNCTION__);  \
            error_prt (error);                                                          \
            *errp = error;                                                              \
            return to_ret;                                                              \
        }                                                                               \
    }

    #define errp_verify( errp, to_ret )                                                 \
    {                                                                                   \
        if (err_t error = tree_err_check (errp))                                        \
        {                                                                               \
            fprintf (stderr, "\n\n" "ERRP VERIFY ERROR IN %s; CODE - ", __FUNCTION__);  \
            error_prt (error);                                                          \
            return to_ret;                                                              \
        }                                                                               \
    }

    #define tree_full_verify( tree, nodep, to_ret, errp )                               \
    {                                                                                   \
        errp_verify (errp, to_ret);                                                     \
        node_verify (tree, nodep, to_ret, errp);                                        \
        tree_verify (tree, to_ret, errp);                                               \
    }

#else

/* NO DEBUG DEFINES */

    #define tree_verify( tree, to_ret ) ;
    #define node_verify( tree, nodep, to_ret ) ;
    #define errp_verify( errp, to_ret ) ;

#endif

/* RECOURSIVE FUNCS FOR SUB TREES */

    /* DON'T USE, INTERNAL TREE TYPE FUNCS, TREE SHOULD BE VERIFIED */
    err_t tree_sub_t_copy( const tree_t* const ft_tree, tree_t* const sd_tree, node_t* tocopy, node_t* dstn );
    int     tree_sub_t_dmp      ( const tree_t* const tree, const node_t* const nodep, char* const buff );
    err_t   tree_sub_t_ver      ( const tree_t* const tree, const node_t* const nodep );
    node_t* tree_sub_t_search   ( const tree_t* const tree, node_t* const node, const data_t* const to_cmpr );
    err_t   tree_sub_t_del      ( tree_t* const tree, node_t* const nodep );

/* ******************* */

#include "tree.cpp"

#endif