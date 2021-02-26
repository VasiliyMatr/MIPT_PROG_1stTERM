#ifndef TREE_INCLUDED
#define TREE_INCLUDED

#include <stdlib.h>
#include "utils.h"
#include "tree_utils.h"

template <typename data_t>
class tree_t;

template <typename data_t>
using node_ptr = typename tree_t<data_t>::ptr_t;

template <typename data_t>
class tree_t {

private: /* PRIVATE CONSTS */

    /* dump file name */
    char const* dump_file_name_ = nullptr;

private: /* PRIVATE DATA STRUCTURES */

    /* tree node structure */
    struct node_t {

        /* data in node */
        data_t       data ;
        
        /* addr of node that have this node in left or right addr */
        node_t*     father;

        /* left and right nodes */
        node_t*     left  ;
        node_t*     right ;

    #ifndef NDBG

        /* ptr to tree root for verification */
        node_t*     root  ;

    #endif
        /* length of path from root node to this node */
        size_t       depth;
    };

    /* enum type for output about node type for dump */
    enum node_sub_type {
        /* root node */
        ROOT                ,
        /* not leaf and not root ok node */
        BODY                ,
        /* leaf node */
        LEAF                ,
        /* BAD SUBTYPES */

            /* root addr don't matches this node addr, but this node father ptr == nullptr */
            FAKE_ROOT       = err_t::ROOT_ERR   ,

            /* wrong ptr connections detected */
            WRONG_LEFT      = err_t::LEFT_ERR   ,
            WRONG_RIGHT     = err_t::RIGHT_ERR  ,
            WRONG_FATHER    = err_t::FATHER_ERR ,

            /* undefined node type node */
            UNDEF           = err_t::UNDEF_ERR  ,
    };

public: /* PRUBLIC DATA STRUCTURES */

    /* for safe ptrs */
    class ptr_t {

    friend class tree_t<data_t>;

    private:

        /* ctor for friends */
        ptr_t( node_t* ptr );

        /* ptr */
        node_t* data_   = nullptr;

        /* to deref ptr */
        node_t& operator *();

        /* to assign ptr */
        node_ptr<data_t>&  operator =( node_t* ptr );

        /* to get ptr value */
        node_t* get_p();

        /* to get access to fields */
        node_t* operator ->();

    public:

        /* to compare ptrs */
        bool operator ==( ptr_t sd );

        /* c-tor for user */
        ptr_t();

        /* to get info about ptr validity */
        bool isBad();

    };

private: /* PRIVATE FIELDS */

    node_t* root_p_ = nullptr;

    /* number of nodes in tree */
    size_t           size_   = 1;

    /* error code */
    err_t            error_  = err_t::OK;

public : /* PUBLIC METHODS */

    /* constructor and destructor */
    tree_t( const char* dump_file_name = "tree_dump.pdf" );
   ~tree_t();

    /* copy constructor and = operator are deleted for safety */
    tree_t( const tree_t &tocopy )                  = delete;
    tree_t& operator = ( const tree_t& tocopy )     = delete;

    /* methods to add new node to tree */
    node_ptr<data_t> add_right( node_ptr<data_t> to_att_to );
    node_ptr<data_t> add_left ( node_ptr<data_t> to_att_to );

    /* method to write data to node */
    void set_data ( node_ptr<data_t> node, data_t* data );

    /* method to get data from node */
    data_t* get_data ( node_ptr<data_t> node );

    /* method to get node depth */
    int get_depth ( node_ptr<data_t> node );

    /* methods to get addrs */
    node_ptr<data_t> get_right( node_ptr<data_t> father );
    node_ptr<data_t> get_left ( node_ptr<data_t> father );
    node_ptr<data_t> get_fath ( node_ptr<data_t> dought );

    /* sub method for destructor method, can delete subtree with its root node */
    void del_sub_tree( node_ptr<data_t> root );

    /* graphical dump method */
    void dump();

    /* verify function */
    void verificator();

    /* RECOURSIVE search for data WORKS SLOW */
    node_ptr<data_t> slow_search_for_data( data_t* data_to_search );

    /* getters */
    err_t get_err();
    node_ptr<data_t> get_root();
    size_t get_size();

    /* to reset error_ */
    void reset_err();

private: /* PRIVATE METHODS */

    /* METHODS FOR SUB TREES !! TREE SHOULD BE VERIFIED BEFORE CALLS OF THIS METHODS !!*/

        /* sub method for destructor method and del_sub_tree method */
        void sub_tree_del( node_t* root );

        /* sub method for slow_search_for_data method*/
        node_t* sub_tree_search ( node_t*, data_t* to_cmpr );

        /* sub method for graphical dump method */
        int sub_tree_dump( node_t* root, char* buff );

        /* sub method for verify method */
        void sub_tree_verify( node_t* root );

    /* ********************* */

    /* FOR VERIFICATION */

        /* sub method for verificator and dump */
        node_sub_type node_verify( node_t* node );

#ifndef NDBG

        /* verify and dump define */
        #define verify(node, to_return)                 \
        {                                               \
            if ((bool)error_)                           \
            {                                           \
                return to_return;                       \
            }                                           \
                                                        \
            if (isBadReadPtr (node.get_p()))            \
            {                                           \
                error_ = err_t::BAD_PTR;                \
                return to_return;                       \
            }                                           \
                                                        \
            if (node->root != root_p_)                  \
            {                                           \
                error_ = err_t::WR_ROOT_ERR;            \
                return to_return;                       \
            }                                           \
                                                        \
            verificator();                              \
                                                        \
            if ((bool)error_)                           \
            {                                           \
                dump();                                 \
                return to_return;                       \
            }                                           \
        }

#else

        #define verify(node, to_return) ;

#endif

    /* **************** */
};

#include "tree.cpp"
#endif
