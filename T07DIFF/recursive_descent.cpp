#define CRITICAL_ERR(to_ret, to_check, err_str )                \
if (to_check)                                                   \
{                                                               \
    fprintf (stderr, "\n\n" "REC DESC ERROR: " err_str "\n\n"); \
    return to_ret;                                              \
}

err_t SetData( const char* newbuff, calctor_t* const calcp )
{
    BUFF_   = newbuff;
    CALCP_  = calcp;

    return OK;
}

err_t GetTree()
{
    CRITICAL_ERR (BAD_PTR, isBadPtr (BUFF_)  , "BAD BUFF_  PTR");
    CRITICAL_ERR (BAD_PTR, isBadPtr (CALCP_) , "BAD CALCP_ PTR");

    POSE_ = 0;

    Get3Prior ();

    if (ERROR_ != OK || BUFF_[POSE_] != END_)
    {
        fprintf (stderr, "SYNTAX ERROR" "\n\n");
        fprintf (stderr, "%s" "\n"    , BUFF_);
        fprintf (stderr, "%*c" "\n\n" , POSE_ + 1, '^');
    }

    tree_dump (calc_get_treep (CALCP_));

    return ERROR_;
}

/* define to add operation */
#define add_operation()                                             \
{                                                                   \
    data_t data = { OPERATION,                                      \
                    BUFF_[POSE_] << 8 * BYTE_SHT (NAME_LEN_, 0),    \
                    NAN,                                            \
                    -1 };                                           \
    op_ptr = tree_new_node (treep, &data, &ERROR_);                 \
                                                                    \
    if (ERROR_ != OK) return nullptr;                               \
                                                                    \
    tree_att_node (treep, op_ptr, left, LEFT_, &ERROR_);            \
                                                                    \
    op_ptr->data.name = BUFF_[POSE_];                               \
    op_ptr->data.type = OPERATION;                                  \
                                                                    \
    POSE_++;                                                        \
    node_t* righ = Get2Prior ();                                    \
    if (ERROR_ != OK) return nullptr;                               \
                                                                    \
    tree_att_node (treep, op_ptr, righ, RIGH_, &ERROR_);            \
                                                                    \
    left = op_ptr;                                                  \
                                                                    \
    break;                                                          \
}

node_t* Get3Prior()
{
    node_t* left = Get2Prior ();
    if (ERROR_ != OK) return nullptr;

    tree_t* treep = calc_get_treep (CALCP_);
    node_t* op_ptr = nullptr;

    while (1)
    {
        switch (BUFF_[POSE_])
        {
            case OP_PLUS:
            case OP_MINUS:
                add_operation ();

            default:
                return left;
        }
    }
}

node_t* Get2Prior()
{
    node_t* left = Get1Prior ();
    if (ERROR_ != OK) return nullptr;

    tree_t* treep = calc_get_treep (CALCP_);
    node_t* op_ptr = nullptr;

    while (1)
    {
        switch (BUFF_[POSE_])
        {
            case OP_MULTIPLY:
            case OP_DIVIDE:
                add_operation ();

            default:
                return left;
        }
    }
}

node_t* Get1Prior()
{
    node_t* left = GetBraces ();
    if (ERROR_ != OK) return nullptr;

    tree_t* treep = calc_get_treep (CALCP_);
    node_t* op_ptr = nullptr;

    while (1)
    {
        switch (BUFF_[POSE_])
        {
            case OP_POWER:
                add_operation ();

            default:
                return left;
        }
    }
}

#undef add_operation() 

node_t* GetBraces()
{
    /* braces parce */
    if (BUFF_[POSE_] == OPN_BR_)
    {
        POSE_++;
        node_t* br_expr = Get3Prior();
        if (BUFF_[POSE_] == CLS_BR_)
        {
            POSE_++;
            return br_expr;
        }

        ERROR_ = SYNT_ERR;
        return nullptr;
    }

    /* parcing name */
    NAME_ name = GetName ();

    /* no name found */
    if (name == 0)
    {
        return GetNumber ();
    }

    /* rval func parse */
    node_t* func = GetFunction (name);
    if (func != nullptr) return func;

    /* func not found */
    if (ERROR_ == NO_FUNC)
    {
        ERROR_ = OK;

        tree_t* treep = calc_get_treep (CALCP_);

        int i = calc_search_var (CALCP_, name);

        data_t new_data = { VARIABLE, OP_UNDEFINED, NAN, -1 };

        if (i >= 0)
        {
            new_data.var_id = i;
        }

        else
        {
            CALCP_->variables[CALCP_->curr_var] = name;
            new_data.var_id = CALCP_->curr_var++;
        }

        new_data.name = name;

        node_t* new_var = nullptr;
        if (treep->root_->data.type == 0)
        {
            new_var = treep->root_;
            new_var->data = new_data;
        }
        else
            new_var = tree_new_node (treep, &new_data, &ERROR_);

        if (ERROR_ != OK) return nullptr;
        new_var->data.type = VARIABLE;

        return new_var;
    }

    return nullptr;
}

node_t* GetFunction( NAME_ name )
{
    /* searching for func with such name */
    switch (name)
    {
        #define DEF_OP( NAME, code, oper_info, priority, calca, latex_todo, latex )     \
        case OP_##NAME :                                                                \
        {                                                                               \
            /* brace check */                                                           \
            if (BUFF_[POSE_] != OPN_BR_)                                                \
            {                                                                           \
                ERROR_ = SYNT_ERR;                                                      \
                return nullptr;                                                         \
            }                                                                           \
            /* skipping brace */                                                        \
            POSE_++;                                                                    \
                                                                                        \
            /* parcing func arg */                                                      \
            node_t* arg = Get3Prior();                                                  \
                                                                                        \
            /* brace check */                                                           \
            if (BUFF_[POSE_] != CLS_BR_)                                                \
            {                                                                           \
                ERROR_ = SYNT_ERR;                                                      \
                return nullptr;                                                         \
            }                                                                           \
            /* arg parse error check */                                                 \
            if (ERROR_ != OK)                                                           \
                return nullptr;                                                         \
                                                                                        \
            /* skipping brace */                                                        \
            POSE_++;                                                                    \
                                                                                        \
                                                                                        \
            data_t new_data = { OPERATION, name, NAN, -1 };                             \
                                                                                        \
            /* all is parced correctly */                                               \
            node_t* func = tree_new_node (calc_get_treep (CALCP_), &new_data, &ERROR_); \
            if (ERROR_ != OK)                                                           \
                return nullptr;                                                         \
                                                                                        \
            func->data.name = OP_##NAME;                                                \
            func->data.type = OPERATION;                                                \
            tree_att_node (calc_get_treep (CALCP_), func, arg, RIGH_, &ERROR_);         \
                                                                                        \
            return ERROR_ != OK ? nullptr : func;                                       \
        }

        #include "DSL.h"
        #undef DEF_OP()

        /* func not found */
        default:
            ERROR_ = NO_FUNC;
            return nullptr;
    }
}

node_t* GetNumber()
{
    /* just scanning BUFF_ and sending data to steck */
    double num      = 0;
    size_t shift    = 0;

    if (POSE_ >= MAX_BUFF_LEN_ || sscanf (BUFF_ + POSE_, "%lf%n", &num, &shift) == 0)
    {
        ERROR_ = SYNT_ERR;
        return nullptr;
    }

    tree_t* treep = calc_get_treep (CALCP_);
    data_t new_data = { NUMBER, OP_UNDEFINED, num, -1 };

    node_t* new_num = nullptr;
    if (treep->root_->data.type == 0)
    {
        new_num = treep->root_;
        new_num->data = new_data;
    }
    else
        new_num = tree_new_node (treep, &new_data, &ERROR_);

    if (ERROR_ != OK)
        return nullptr;

    POSE_ += shift;

    return new_num;
}

NAME_ GetName()
{
    /* to know name len */
    size_t  ch_id           = 0;
    /* to store name */
    NAME_   code            = 0;

    /* copying name code */
    for (; ch_id < NAME_LEN_; ch_id++)
    {
        if (!isalpha (BUFF_[ch_id + POSE_]))
        {
            break;
        }

        code += (NAME_)BUFF_[ch_id + POSE_] << (8 * BYTE_SHT (NAME_LEN_, ch_id));
    }

    /* adding shift */
    POSE_ += ch_id;

    return code;
}

int calc_search_var( calctor_t* calc, NAME_ name )
{
    /* just cmpr names */
    for (size_t var_id = 0; var_id < calc->curr_var; var_id++)
    {
        if ((calc->variables)[var_id] == name)
        {
            return var_id;
        }
    }

    /* no such variable */
    return -1;
}