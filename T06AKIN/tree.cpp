#include "tree.h"


/*   PTR_T METHODS   */

template <typename data_t>
tree_t<data_t>::ptr_t::ptr_t() {}

template <typename data_t>
typename tree_t<data_t>::node_t& tree_t<data_t>::ptr_t::operator *()
{

#ifndef NDBG

    /* ptr check */
    if (isBadReadPtr (data_))
    {
        fprintf (stderr, "\n\n" "BAD PTR DEREF" "\n\n");
        return get_poison_val <node_t>();
    }

#endif

    return *data_;
}


template <typename data_t>
node_ptr<data_t>& tree_t<data_t>::ptr_t::operator =( node_t* ptr )
{
    /* assignation */
    data_  = ptr;
    return *this;
}

template <typename data_t>
typename tree_t<data_t>::node_t* tree_t<data_t>::ptr_t::get_p() { return data_; }

template <typename data_t>
typename tree_t<data_t>::node_t* tree_t<data_t>::ptr_t::operator ->() { return data_; }

template <typename data_t>
tree_t<data_t>::ptr_t::ptr_t( node_t* ptr ) : data_( ptr ){}

template <typename data_t>
bool tree_t<data_t>::ptr_t::isBad()
{

#ifndef NDBG

    if (isBadReadPtr (data_))
    {
        if (data_ != nullptr)
        {
            fprintf (stderr, "\n" "BAD AND NOT NULL PTR FOUND!" "\n");
        }

        return true;
    }

    return false;

#else

    return data_ == nullptr;

#endif
}

template <typename data_t>
bool tree_t<data_t>::ptr_t::operator ==( ptr_t sd )
{
    return data_ == sd.get_p();
}

/* **************** */

/*  TREE_T METHODS  */

template <typename data_t>
tree_t<data_t>::tree_t( const char const* dump_file_name ) : dump_file_name_(dump_file_name)
{
    root_p_ = (node_t* )calloc (1, sizeof (node_t));
    if (root_p_ == nullptr)
    {
        error_ = err_t::MEM_ERR;
        return;
    }

    root_p_->left   = nullptr;
    root_p_->right  = nullptr;
    root_p_->father = nullptr;

#ifndef NDBG

    root_p_->root = root_p_;

#endif

}

template <typename data_t>
tree_t<data_t>::~tree_t()
{
    /* is this tree already deleted? */
    if (root_p_ != nullptr)
    {
        del_sub_tree (root_p_);
        if ((bool)error_)
        {
            return;
        }

        root_p_ = nullptr;
    }
}

template <typename data_t>
void tree_t<data_t>::del_sub_tree( node_ptr<data_t> node )
{
    verify (node, );

    sub_tree_del (node.get_p());
}

template <typename data_t>
void tree_t<data_t>::sub_tree_del( node_t* root )
{
    if (root->left != nullptr)
    {
        sub_tree_del (root->left);
    }

    if (root->right != nullptr)
    {
        sub_tree_del (root->right);
    }

    if (root->father != nullptr)
    {
        if (root->father->left == root)
        {
            root->father->left = nullptr;
        }

        else
        {
            root->father->right = nullptr;
        }
    }

    free (root);
    size_--;
}

template <typename data_t>
void tree_t<data_t>::reset_err()
{
    error_ = err_t::OK;
}

template <typename data_t>
err_t tree_t<data_t>::get_err()
{
    return error_;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::get_root()
{
    return root_p_;
}

template <typename data_t>
size_t tree_t<data_t>::get_size()
{
    return size_;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::add_left( node_ptr<data_t> father )
{
    verify (father, nullptr);

    /* mem allocation */
    node_t* left = (node_t* )calloc (1, sizeof (node_t));
    if (left == nullptr)
    {
        error_ = err_t::MEM_ERR;
        return nullptr;
    }

    left  ->father = father.get_p();

#ifndef NDBG

    left ->root   = root_p_;

#endif

    left  ->depth  = father->depth + 1;
    father->left   = left;

    size_++;

    return left;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::add_right( node_ptr<data_t> father )
{
    verify (father, nullptr);

    /* mem allocation */
    node_t* right = (node_t*)calloc (1, sizeof(node_t));
    if (right == nullptr)
    {
        error_ = err_t::MEM_ERR;
        return nullptr;
    }

#ifndef NDBG

    right ->root   = root_p_;

#endif

    right ->father = father.get_p();
    right ->depth  = father->depth + 1;
    father->right  = right;

    size_++;

    return right;
}

template <typename data_t>
void tree_t<data_t>::set_data( node_ptr<data_t> node, data_t* data )
{
    verify (node, );

    /* !!! this func should be overloaded for choosed data type !!! err_t copy_data( data_t* dst, data_t* srs )*/
    error_ = copy_data_f (&(node->data), data);
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::get_left( node_ptr<data_t> father )
{
    verify (father, ptr_t (nullptr));

    return father->left;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::get_right( node_ptr<data_t> father )
{
    verify (father, ptr_t (nullptr));

    return father->right;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::get_fath( node_ptr<data_t> dought )
{
    verify (dought, ptr_t (nullptr));

    return dought->father;
}

template <typename data_t>
data_t* tree_t<data_t>::get_data( node_ptr<data_t> node )
{
    verify (node, nullptr);

    /* !!! this func should be overloaded for choosed data type !!! data_t& copy_data( data_t* data )*/
    return get_data_f (&(node->data));
}

template <typename data_t>
int tree_t<data_t>::get_depth( node_ptr<data_t> node )
{
    verify (node, -1);

    return node->depth;
}

template <typename data_t>
node_ptr<data_t> tree_t<data_t>::slow_search_for_data( data_t* data_to_search )
{
    /* verification */
    verificator ();
    if ((bool)error_)
    {
        dump ();
        return nullptr;
    }

    /* data node search */
    node_t* node = sub_tree_search (root_p_, data_to_search);

    /* returning found data node */
    return node;
}

template <typename data_t>
typename tree_t<data_t>::node_t* tree_t<data_t>::sub_tree_search ( node_t* node, data_t* to_cmpr )
{
    /* comparing data */
    if (!compare_data_f (&(node->data), to_cmpr))
    {
        return node;
    }

    /* searching for data in right sub tree */
    if (node->right)
    {
        node_t* d_node = sub_tree_search (node->right, to_cmpr);

        if (d_node != nullptr)
        {
            return d_node;
        }
    }
    
    /* searching for data in left sub tree */
    if (node->left)
    {
        node_t* d_node = sub_tree_search (node->left, to_cmpr);

        if (d_node != nullptr)
        {
            return d_node;
        }
    }

    /* not found */
    return nullptr;
}

template <typename data_t>
void tree_t<data_t>::verificator()
{
    if ((bool)error_)
    {
        return;
    }

    sub_tree_verify (root_p_);
}

template <typename data_t>
void tree_t<data_t>::sub_tree_verify( node_t* root )
{
    /* error already occured */
    if ((bool)error_)
    {
        return;
    }

    /* can't check bad ptr, but if it's nullptr, then all is ok */
    if (isBadReadPtr (root))
    {
        if (root == nullptr)
        {
            error_ = err_t::OK;
            return;
        }

        error_ = err_t::BAD_PTR;
        return;
    }

    /* verifying curr node */
    switch (node_sub_type type = node_verify (root))
    {
        case ROOT:
        case BODY:
        {
            break;
        }

        case LEAF:
        {
            return;
        }

        case FAKE_ROOT      :
        case WRONG_FATHER   :
        case WRONG_RIGHT    :
        case WRONG_LEFT     :
        case UNDEF          :
        {
            error_ = (err_t)type;
        }

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF NODE TYPE IN VERIF" "\n\n");
            break;
        }
    }

    /* left subtree verify */
    if (root->left != nullptr)
    {
        sub_tree_verify (root->left);
        if ((bool)error_)
        {
            return;
        }
    }

    /* right sub tree verify */
    if (root->right != nullptr)
    {
        sub_tree_verify (root->right);
    }
}

size_t numof_dumped_nodes_ = 0;

template <typename data_t>
void tree_t<data_t>::dump()
{
    /* max size of dump str about one data unit */
    static const int max_dump_str_len = 500;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * max_dump_str_len, size_ + 1);
    if (buff == nullptr)
    {
        error_ = err_t::MEM_ERR;
        return;
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info about graph */
    buff_shift += sprintf (buff + buff_shift, "strict digraph G{\n"
                                               "rankdir=LR; \n"
                                               "edge[color=\"black\"]; \n"
                                               "node[color=\"black\", shape=record];\n");

    /* recoursive tree dump */
    buff_shift += sub_tree_dump (root_p_, buff + buff_shift);
    numof_dumped_nodes_ = 0;

    /* end of graph */
    buff_shift += sprintf (buff + buff_shift, "}");

    /* creating file */
    FILE* out_file = fopen ("dump_file.dot", "wb");
    if (out_file == nullptr)
    {
        free (buff);
        return;
    }

    /* writing tree info and free memory */
    fwrite (buff, sizeof (char), buff_shift, out_file);
    free (buff);
    fclose (out_file);

    /* system cmd str */
    char cmd[300] = "\0";

    /* dump */
    sprintf (cmd, "dot -Tpdf dump_file.dot > %s", dump_file_name_);
    system (cmd);

    /* dump out */
    sprintf (cmd, "%s", dump_file_name_);
    system (cmd);

    return;
}

/* node info print define */
#define NODE_PRINT( color, node_type )                                                                                          \
{                                                                                                                               \
    case node_type:                                                                                                             \
    {                                                                                                                           \
        buff_shift = sprintf (buff,                                                                                             \
            "\nNODE%p[style=\"filled\", fillcolor=\"%s\", pin = \"true\", "                                                     \
            "label=\"<top> %p | <left> left  %p  | <right> right  %p |<father> father  %p| %s | depth %d | \\\"%s\\\"\"]; \n",  \
             root, color, root, root->left, root->right, root->father, #node_type, root->depth, data);                          \
        break;                                                                                                                  \
    }                                                                                                                           \
}

template <typename data_t>
int tree_t<data_t>::sub_tree_dump( node_t* root, char* buff )
{

    if (isBadReadPtr (buff))
    {
        fprintf (stderr, "\n\n" "BAD BUFF PTR IN DUMP" "\n\n");
    }

    /* diff colors for left and right nodes */
    const char body_colors[2][8] = { "#cccc00", "#b38600" };

    /* left ro right */
    static bool tree_side = 0;

    /* helps if tree cycled */
    if (numof_dumped_nodes_ >= size_)
    {
        return 0;
    }

    /* can't dump it */
    if (isBadReadPtr (root))
    {
        return 0;
    }

    size_t buff_shift = 0;
    /* !!! this func should be overloaded for choosed data type !!! char* prt_data_f (data_t*) */
    char* data = prt_data_f (&(root->data));

    /* node info out */
    switch (node_verify (root))
    {
        NODE_PRINT ("#FF1D72", WRONG_RIGHT);
        NODE_PRINT ("#FF1D72", WRONG_LEFT);
        NODE_PRINT ("#FF1D72", WRONG_FATHER);

        NODE_PRINT ("#ffcc66", ROOT);
        NODE_PRINT ("#33cc33", LEAF);

        NODE_PRINT (body_colors[tree_side], BODY);

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF TYPE IN DUMP" "\n\n");
        }
    }

    /* PTRS INFO */
    if (root->left != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<left>->NODE%p:<top>;\n",
                                                    root, root->left);
    }

    if (root->right != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<left>->NODE%p:<top>;\n",
                                                    root, root->right);
    }

    if (!isBadReadPtr (root->left))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", root->left,  root->left->father);
    }

    if (!isBadReadPtr (root->right))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", root->right, root->right->father);
    }

    /* ********* */

    numof_dumped_nodes_++;

    /* recoursion */
    if (root->left != nullptr)
    {
        tree_side = 0;
        buff_shift += sub_tree_dump (root->left,  buff + buff_shift);
    }

    if (root->right != nullptr)
    {
        tree_side = 1;
        buff_shift += sub_tree_dump(root->right, buff + buff_shift);
    }

    return buff_shift;
}

template <typename data_t>
typename tree_t<data_t>::node_sub_type tree_t<data_t>::node_verify( node_t* node )
{
    if (isBadReadPtr (node))
    {
        return UNDEF;
    }

    /* checking father ptr */
    if (isBadReadPtr (node->father))
    {
        if (node->father == nullptr)
        {
            if (root_p_ != node)
            {
                return FAKE_ROOT;
            }

            return ROOT;
        }

        return WRONG_FATHER;
    }

    /* checking left ptr */
    if (isBadReadPtr (node->left))
    {
        if (node->left == nullptr)
        {
            if (node->right == nullptr)
            {
                return LEAF;
            }

            if (node->right->father != node)
            {
                return WRONG_RIGHT;
            }

            return BODY;
        }

        return WRONG_LEFT;
    }

    /* checking right ptr */
    if (isBadReadPtr (node->right))
    {
        if (node->right == nullptr)
        {
            if (node->left == nullptr)
            { 
                return LEAF;
            }

            if (node->left->father != node)
            {
                return WRONG_LEFT;
            }

            return BODY;
        }

        return WRONG_RIGHT;
    }

    /* father ptr check */
    if (node->father->left != node && node->father->right != node)
    {
        return WRONG_FATHER;
    }

    return BODY;
}