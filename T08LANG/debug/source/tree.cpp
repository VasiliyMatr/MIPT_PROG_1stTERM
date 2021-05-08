#include "compiler.hpp"

void error_prt(Err_t error)
{
    switch (error)
    {
        PRINT_ERR(OK_);
        PRINT_ERR(MEM_ERR_);
        PRINT_ERR(SYNT_ERR_);
        PRINT_ERR(BAD_PTR_);
        PRINT_ERR(BAD_INIT_ERR_);
        PRINT_ERR(ROOT_MATCH_ERR_);
        PRINT_ERR(ADD_ERR_);
        PRINT_ERR(FILE_ERR_);
        PRINT_ERR(CONF_ERR_);
        PRINT_ERR(LEAF_ERR_);
        PRINT_ERR(BAD_ERR_PTR_);
        PRINT_ERR(ROOT_ERR_);
        PRINT_ERR(LEFT_ERR_);
        PRINT_ERR(RIGH_ERR_);
        PRINT_ERR(PARENT_ERR_);
        PRINT_ERR(UNDEF_ERR_);
    }
}

/* FUNCS FOR tree_t */

    /* out format for nedded tree labels and proper coloring */
    const char COMMON_FORMAT_[] = { "%c" "\\n" "%.*s\", " "fillcolor=\"%s" };
    /* colors */
    const char COLORS_  [][16] = { "blue", "yellow", "red" };

    /* colors ids */
    const size_t BLU_COL_ = 0;
    const size_t YEL_COL_ = 1;
    const size_t RED_COL_ = 2;

    Err_t get_data_str( const data_t data, char* const buff )
    {
        const char* color = nullptr;

        switch (data.type)
        {
            /* printing number */
            case NUM_:
            {
                sprintf (buff, "%d", data.data.number);
                return OK_;
            }

            /* printing type symbol and name */
            case KEY_:
                color = COLORS_[BLU_COL_];
                break;
            case IDENT_:
                color = COLORS_[YEL_COL_];
                break;

            /* undefined */
            default:
            {
                sprintf (buff, "??? UNDEF ???\", fillcolor=\"red");
                return OK_;
            }
        }

        if (color != nullptr)
            sprintf (buff, COMMON_FORMAT_, data.type, DESCR_LEN_, data.data.name, color);

        return OK_;
    }

    /* not needed in this prj */
    int compare_data_f ( const data_t* const datap, const data_t* const to_compare )
    {
        return 1;
    }

    void copy_data( data_t* tocopy, data_t* dstn )
    {
        assert (!isBadPtr (tocopy) && !isBadPtr (dstn));

        /* just copying data */
        memcpy (dstn, tocopy, sizeof (data_t));
    }

/***********************************/

Err_t tree_ctor( tree_t* const tree, Err_t* const errp, const char* const dump_name )
{
    errp_verify(errp, error);

    /* ptrs check */
    if (isBadPtr (tree) || isBadPtr (dump_name))
    {
        *errp = BAD_PTR_;
        return  BAD_PTR_;
    }

    /* tree fields check */
    if (tree->size_ != 0 || tree->root_ != nullptr || tree->dump_name_ != nullptr)
    {
        *errp = BAD_INIT_ERR_;
        return  BAD_INIT_ERR_;
    }

    /* tree setup */
    tree->dump_name_    = dump_name;
    tree->size_         = 1;
    tree->root_         = (node_t* )calloc (1, sizeof (node_t));

    /* allocation error */
    if (tree->root_ == nullptr)
    {
        *errp = MEM_ERR_;
        return MEM_ERR_;
    }

    /* root setup */
    tree->root_->tree = tree;

    /* init is good */
    return OK_;
}

Err_t tree_dstr( tree_t* const tree, Err_t* const errp )
{
    errp_verify (errp, error);

    if (Err_t error = tree_verificator (tree))
    {
        *errp = error;
        return error;
    }

    /* rec tree nodes deleting */
    if (tree->root_ != nullptr)
        tree_del_subt (tree, tree->root_, errp);

    if (!*errp)
    {
        /* resetting data */
        tree->size_         = 0;
        tree->root_         = nullptr;
        tree->dump_name_    = nullptr;
    }

    return *errp;
}

node_t* tree_get_left( const tree_t* const tree, const node_t* const nodep, Err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->left_ch;
}

node_t* tree_get_righ( const tree_t* const tree, const node_t* const nodep, Err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->righ_ch;
}

node_t* tree_get_prnt( const tree_t* const tree, const node_t* const nodep, Err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->parent;
}

data_t  tree_get_data( const tree_t* const tree, const node_t* const nodep, Err_t* const errp )
{
    tree_full_verify (tree, nodep, POISON_, errp);

    return nodep->data;
}

node_t* tree_add_node( tree_t* const tree, node_t* const nodep, const side_t attach_side, Err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    /* allocating memory */
    node_t* new_node = (node_t* )calloc (1, sizeof (node_t));
    if (new_node == nullptr)
    {
        *errp = MEM_ERR_;
        return nullptr;
    }

    /* initing new node fields */
    new_node->tree   = tree;

    switch (attach_side)
    {
        /* attaching at left side */
        case LEFT_:
        {
            if (nodep->left_ch != nullptr)
            {
                *errp = ADD_ERR_;
                return nullptr;
            }

            new_node->parent = nodep;
            nodep->left_ch = new_node;

            break;
        }

        /* attaching at righ side */
        case RIGH_:
        {
            if (nodep->righ_ch != nullptr)
            {
                *errp = ADD_ERR_;
                return nullptr;
            }

            new_node->parent = nodep;
            nodep->righ_ch = new_node;

            break;
        }

        default:
        {
            *errp = ADD_ERR_;
            return nullptr;
        }
    }

    tree->size_++;

    /* returning new node ptr */
    return new_node;
}

node_t* tree_new_node( tree_t* const tree, const data_t* const data, Err_t* const errp )
{
    errp_verify (errp, nullptr);
    tree_verify (tree, nullptr, errp);

    /* allocating memory */
    node_t* new_node = (node_t* )calloc (1, sizeof (node_t));
    if (new_node == nullptr)
    {
        *errp = MEM_ERR_;
        return nullptr;
    }

    new_node->tree = tree;
    memcpy ((data_t*)((size_t)new_node + DATA_SHIFT_), data, sizeof (data_t));

    tree->size_++;

    return new_node;
}

Err_t tree_att_node( tree_t* const tree, node_t* const prnt, node_t* const chld, const side_t side, Err_t* const errp )
{
    tree_full_verify (tree, prnt, error, errp);
    node_verify (tree, chld, error, errp);

    switch (side)
    {
        /* attaching at left side */
        case LEFT_:
        {
            if (prnt->left_ch != nullptr)
            {
                *errp = ADD_ERR_;
                return ADD_ERR_;
            }

            chld->parent = prnt;
            prnt->left_ch = chld;

            break;
        }

        /* attaching at righ side */
        case RIGH_:
        {
            if (prnt->righ_ch != nullptr)
            {
                *errp = ADD_ERR_;
                return ADD_ERR_;
            }

            chld->parent = prnt;
            prnt->righ_ch = chld;

            break;
        }

        /* BAD SIDE */
        default:
        {
            *errp = ADD_ERR_;
            return ADD_ERR_;
        }
    }

    /* attached root->have new root */
    if (tree->root_ == chld)
    {
        node_t* new_root = prnt;

        while (new_root->parent != nullptr)
        {
            new_root = new_root->parent;
        }

        tree->root_ = new_root;
    }

    return OK_;
}

Err_t tree_set_data( tree_t* const tree, node_t* const nodep, data_t const data, Err_t* const errp )
{
    tree_full_verify (tree, nodep, error, errp);

    /* writing new data to node */
    nodep->data = data;

    return OK_;
}

Err_t tree_del_subt( tree_t* const tree, node_t* const nodep, Err_t* const errp )
{
    tree_full_verify (tree, nodep, error, errp);

    return *errp = tree_sub_t_del (tree, nodep);
}

Err_t tree_sub_t_del( tree_t* const tree, node_t* const nodep )
{
    /* there should not be nullptrs */
    if (nodep == nullptr)
    {
        return BAD_PTR_;
    }

    /* deleting left child */
    if (nodep->left_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_del (tree, nodep->left_ch))
        {
            return error;
        }

        nodep->left_ch = nullptr;
    }

    /* deleting righ child */
    if (nodep->righ_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_del (tree, nodep->righ_ch))
        {
            return error;
        }

        nodep->righ_ch = nullptr;
    }

    /* nulling parent ptr */
    if (nodep->parent != nullptr)
    {
        if (nodep == nodep->parent->left_ch)
        {
            nodep->parent->left_ch = nullptr;
        }

        else
        {
            nodep->parent->righ_ch = nullptr;
        }
    }

    /* free memory */
    free (nodep);
    /* tree size decr */
    tree->size_--;

    /* check for foor free */
    if (nodep == tree->root_)
        tree->root_ = nullptr;

    return OK_;
}

node_t* tree_slow_search_for_data( const tree_t* const tree, const data_t* const data_to_search, Err_t* const errp )
{
    errp_verify (errp, nullptr);
    tree_verify (tree, nullptr, errp);

    /* data node search */
    node_t* node = tree_sub_t_search (tree, tree->root_, data_to_search);

    /* returning found data node */
    return node;
}

node_t* tree_sub_t_search ( const tree_t* const tree, node_t* const node, const data_t* const to_cmpr )
{
    /* comparing data */
    if (!compare_data_f (&(node->data), to_cmpr))
    {
        return node;
    }

    /* searching for data in right sub tree */
    if (node->righ_ch != nullptr)
    {
        node_t* c_node = tree_sub_t_search (tree, node->righ_ch, to_cmpr);

        if (c_node != nullptr)
        {
            return c_node;
        }
    }
    
    /* searching for data in left sub tree */
    if (node->left_ch)
    {
        node_t* c_node = tree_sub_t_search (tree, node->left_ch, to_cmpr);

        if (c_node != nullptr)
        {
            return c_node;
        }
    }

    /* not found */
    return nullptr;
}

Err_t tree_copy( const tree_t* const ft_tree, tree_t* const sd_tree, node_t* tocopy, node_t* dstn, Err_t* errp )
{
    tree_full_verify (ft_tree, tocopy, error, errp);
    tree_full_verify (sd_tree, dstn  , error, errp);

    if (dstn->left_ch != nullptr || dstn->righ_ch != nullptr)
    {
        return BAD_PTR_;
    }

    Err_t error = tree_sub_t_copy (ft_tree, sd_tree, tocopy, dstn);

    return error;
}

Err_t tree_sub_t_copy( const tree_t* const ft_tree, tree_t* const sd_tree, node_t* tocopy, node_t* dstn )
{
    assert (!isBadPtr (tocopy) && !isBadPtr (dstn));

    copy_data (&(tocopy->data), &(dstn->data));

    Err_t error = OK_;

    if (tocopy->left_ch != nullptr)
    {
        tree_add_node (sd_tree, dstn, LEFT_, &error);
        if (error || (error = tree_sub_t_copy (ft_tree, sd_tree, tocopy->left_ch, dstn->left_ch)))
        {
            return error;
        }
    }

    if (tocopy->righ_ch != nullptr)
    {
        tree_add_node (sd_tree, dstn, RIGH_, &error);
        if (error || (error = tree_sub_t_copy (ft_tree, sd_tree, tocopy->righ_ch, dstn->righ_ch)))
        {
            return error;
        }
    }

    return OK_;
}

Err_t tree_is_nodep_valid( const tree_t* const tree, const node_t* const nodep )
{
    if (isBadPtr (nodep))
    {
        return BAD_PTR_;
    }

    if (nodep->tree != tree)
    {
        return ROOT_MATCH_ERR_;
    }

    return OK_;
}

/* verification func */
Err_t tree_verificator( const tree_t* const tree )
{
    if (isBadPtr (tree))
        return BAD_PTR_;
    
    if (isBadPtr (tree->root_))
        return OK_;

    if (tree->root_->left_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_ver (tree, tree->root_->left_ch))
        {
            return error;
        }
    }

    if (tree->root_->righ_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_ver (tree, tree->root_->righ_ch))
        {
            return error;
        }
    }

    return OK_;
}

Err_t tree_sub_t_ver( const tree_t* const tree, const node_t* const nodep )
{
    node_sub_t ntype = tree_get_node_st (tree, nodep);

    switch (ntype)
    {
        case ROOT:
        case BODY:
        case LEAF:
        {
            break;
        }

        case UNDEF:
        case FAKE_ROOT:
        case WRONG_LEFT:
        case WRONG_RIGHT:
        case WRONG_PARENT:
        {
            return (Err_t)ntype;
        }

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF NODE TYPE" "\n\n");
        }
    }

    if (nodep->left_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_ver (tree, nodep->left_ch))
        {
            return error;
        }
    }

    if (nodep->righ_ch != nullptr)
    {
        if (Err_t error = tree_sub_t_ver (tree, nodep->righ_ch))
        {
            return error;
        }
    }

    return OK_;
}

int numof_dumped_nodes_ = 0;

void tree_dump( const tree_t* const tree )
{
    if (isBadPtr (tree))
    {
        fprintf (stderr, "\n\n" "BAD PTR IN DUMP FUNC!" "\n\n");
    }

    /* max size of dump str about one data unit */
    static const int max_dump_str_len = 500;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * max_dump_str_len, tree->size_ + 1);
    if (buff == nullptr)
    {
        fprintf (stderr, "\n\nNO MEMORY FOR DUMP BUFFER!\n\n");
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init data about graph */
    buff_shift += sprintf (buff + buff_shift, "strict digraph G{\n"
                                              "rankdir=LR; \n"
                                              "edge[color=\"black\"]; \n"
                                              "node[color=\"black\", shape=record];\n");

    /* recoursive tree dump */
    buff_shift += tree_sub_t_dmp (tree, tree->root_, buff + buff_shift);
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

    /* writing tree data and free memory */
    fwrite (buff, sizeof (char), buff_shift, out_file);
    free (buff);
    fclose (out_file);

    /* system cmd str */
    char cmd[300] = "\0";

    /* dump */
    sprintf (cmd, "dot -Tpng dump_file.dot > %s", tree->dump_name_);
    system (cmd);

    /* dump out */
    sprintf (cmd, "google-chrome %s", tree->dump_name_);
    system (cmd);

    return;
}

int   tree_sub_t_dmp( const tree_t* const tree, const node_t* const nodep, char* const buff )
{
    if (isBadPtr (buff))
    {
        fprintf (stderr, "\n\n" "BAD BUFF PTR IN DUMP" "\n\n");
        return 0;
    }

    /* max len of strs for dump */
    const int  MAX_STR_LEN_ = 64;

    /* diff colors for left and right nodes */
    const char body_colors[2][8] = { "#cccc00", "#b38600" };

    /* left ro right */
    static bool tree_side = 0;

    /* helps if tree cycled */
    if (numof_dumped_nodes_ >= tree->size_)
    {
        return 0;
    }

    /* can't dump it */
    if (isBadPtr (nodep))
    {
        return 0;
    }

    char data_str[MAX_STR_LEN_] = "UNKNOWN DATA";

    get_data_str (nodep->data, data_str);

    size_t buff_shift = 0;

    /* node data print define */
    #define NODE_PRINT( color, node_type )                                                                                                  \
    {                                                                                                                                       \
        case node_type:                                                                                                                     \
        {                                                                                                                                   \
            buff_shift = sprintf (buff,                                                                                                     \
                "\nNODE%p[style=\"filled\", fillcolor=\"%s\", pin = \"true\", "                                                             \
                "label=\"<top> %p | <left> left  %p  | <right> right  %p |<father> father  %p| %s | %.*s\"]; \n",                           \
                 nodep, color, nodep, nodep->left_ch, nodep->righ_ch, nodep->parent, #node_type, MAX_STR_LEN_, data_str);                   \
            break;                                                                                                                          \
        }                                                                                                                                   \
    }

    /* node data out */
    switch (tree_get_node_st (tree, nodep))
    {
        NODE_PRINT ("#FF1D72", WRONG_RIGHT);
        NODE_PRINT ("#FF1D72", WRONG_LEFT);
        NODE_PRINT ("#FF1D72", WRONG_PARENT);
        NODE_PRINT ("#FF1D72", UNDEF);

        NODE_PRINT ("#ffcc66", ROOT);
        NODE_PRINT ("#33cc33", LEAF);

        NODE_PRINT (body_colors[tree_side], BODY);

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF TYPE IN DUMP" "\n\n");
        }
    }

    #undef NODE_PRINT

    /* PTRS INFO */
    if (nodep->left_ch != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<left>->NODE%p:<top>;\n",
                                                    nodep, nodep->left_ch);
    }

    if (nodep->righ_ch != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<right>->NODE%p:<top>;\n",
                                                    nodep, nodep->righ_ch);
    }

    if (!isBadPtr (nodep->left_ch))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", nodep->left_ch,  nodep->left_ch->parent);
    }

    if (!isBadPtr (nodep->righ_ch))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", nodep->righ_ch, nodep->righ_ch->parent);
    }

    /* ********* */

    numof_dumped_nodes_++;

    /* recoursion */
    if (nodep->left_ch != nullptr)
    {
        tree_side = 0;
        buff_shift += tree_sub_t_dmp (tree, nodep->left_ch, buff + buff_shift);
    }

    if (nodep->righ_ch != nullptr)
    {
        tree_side = 1;
        buff_shift += tree_sub_t_dmp(tree, nodep->righ_ch, buff + buff_shift);
    }

    return buff_shift;
}

node_sub_t tree_get_node_st( const tree_t* const tree, const node_t* const nodep )
{
    if (tree_is_nodep_valid (tree, nodep))
    {
        return UNDEF;
    }

    /* checking father ptr */
    if (isBadPtr (nodep->parent))
    {
        if (nodep->parent == nullptr)
        {
            if (tree->root_ != nodep)
            {
                return FAKE_ROOT;
            }

            return ROOT;
        }

        return WRONG_PARENT;
    }

    /* checking left ptr */
    if (isBadPtr (nodep->left_ch))
    {
        if (nodep->left_ch == nullptr)
        {
            if (nodep->righ_ch == nullptr)
            {
                return LEAF;
            }

            if (nodep->righ_ch->parent != nodep)
            {
                return WRONG_RIGHT;
            }

            return BODY;
        }

        return WRONG_LEFT;
    }

    else if (nodep->left_ch->parent != nodep)
    {
        return WRONG_LEFT;
    }

    /* checking right ptr */
    if (isBadPtr (nodep->righ_ch))
    {
        if (nodep->righ_ch == nullptr)
        {
            if (nodep->left_ch == nullptr)
            { 
                return LEAF;
            }

            if (nodep->left_ch->parent != nodep)
            {
                return WRONG_LEFT;
            }

            return BODY;
        }

        return WRONG_RIGHT;
    }

    else if (nodep->righ_ch->parent != nodep)
    {
        return WRONG_RIGHT;
    }

    /* father ptr check */
    if (nodep->parent->left_ch != nodep && nodep->parent->righ_ch != nodep)
    {
        return WRONG_PARENT;
    }

    return BODY;
}

Err_t tree_err_check( Err_t* errp )
{
    /* error is undefined */
    if (isBadPtr (errp))
    {
        return BAD_ERR_PTR_;
    }

    return *errp;
}