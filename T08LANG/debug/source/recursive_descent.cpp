
#include "compiler.hpp"

/* compiler to work with */
    static Compiler*    CMPLP_      = nullptr;
/* curr token id */
    static size_t       POSE_       = 0;

/* for errors info output */
    static ErrorInfo_t  ERROR_INFO_ = { OK_, -1 };

void  getError( Compiler* const cmplp )
{
    cmplp->error_info_ = ERROR_INFO_;
}

Err_t setData( Compiler* const cmplp )
{
    CMPLP_   = cmplp;
    return OK_;
}

/* macro for errors check ir rec desc algo
 *   TO_RET_ - value to return in case of error; TO_CHECK_ - condition to check;
 *   ERROR_ASSIGN_ - new error value (if there is OK - no error change);
 *   ERR_STR_ - error descr; TO_DEL(1 or 2)_ - sub trees to delete in case of error
 */
    #define CRITICAL_ERR( TO_CHECK_, TO_RET_ , ERROR_ASSIGN_, TO_DEL1_, TO_DEL2_ )  \
    if (TO_CHECK_)                                                                  \
    {                                                                               \
        Err_t tree_err = OK_;                                                       \
        if (TO_DEL1_ != nullptr)                                                    \
            tree_del_subt (treep, TO_DEL1_, &tree_err);                             \
        if (TO_DEL2_ != nullptr)                                                    \
            tree_del_subt (treep, TO_DEL2_, &tree_err);                             \
                                                                                    \
        if (tree_err) fprintf (stderr, "IN CRITICAL_ERR MACRO - tree error");       \
                                                                                    \
        if (ERROR_ASSIGN_ != OK_) ERROR_INFO_ = { ERROR_ASSIGN_, token.location };  \
                                                                                    \
        return TO_RET_;                                                             \
    }

Err_t getTree()
{
    tree_t* treep = cmpl_getTreep (CMPLP_);

    if (isBadPtr (CMPLP_))
    {
        ERROR_INFO_.error_ = BAD_PTR_;
        return BAD_PTR_;
    }

    POSE_       = 0;
    ERROR_INFO_ = { OK_, -1 };

    treep->root_->data.type = UNDF_T_;

    getFuncs ();

    /* error detected */
    if (ERROR_INFO_.error_ != OK_) return ERROR_INFO_.error_;


    data_t token = CMPLP_->tokens_[POSE_];

    /* if there are no EOF token */
    CRITICAL_ERR (token.type != UNDF_T_, SYNT_ERR_, OK_,
                  tree_get_left (treep, treep->root_, &ERROR_INFO_.error_),
                  tree_get_righ (treep, treep->root_, &ERROR_INFO_.error_));

    return ERROR_INFO_.error_;
}

/* define to get righ sub tree and apply all (if needed) */
#define GET_N_APPLY( FUNC_ )                                                    \
{                                                                               \
    /* creating parent node */                                                  \
    parent = tree_new_node (treep, &token, &ERROR_INFO_.error_);                \
    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, left, nullptr)       \
                                                                                \
    /* attaching left node */                                                   \
    tree_att_node (treep, parent, left, LEFT_, &ERROR_INFO_.error_);            \
                                                                                \
    /* parsing righ part & attaching it */                                      \
    POSE_++;                                                                    \
    righ = FUNC_ ();                                                            \
    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, parent, nullptr)     \
    CRITICAL_ERR (righ == nullptr, nullptr, SYNT_ERR_, parent, nullptr)         \
    tree_att_node (treep, parent, righ, RIGH_, &ERROR_INFO_.error_);            \
                                                                                \
    /* if there are many tokens with such format,                               \
     * then curr parent will be left for next parse iteration                   \
     */                                                                         \
    left = parent;                                                              \
                                                                                \
    token = tokens[POSE_];                                                      \
}

/* define with all needed declarations for rec desc funcs */
#define INIT_STUFF                              \
    if (ERROR_INFO_.error_ != OK_)              \
        return nullptr;                         \
                                                \
    data_t  token   = POISON_;                  \
    data_t* tokens  = CMPLP_->tokens_;          \
                                                \
    tree_t* treep   = cmpl_getTreep (CMPLP_);   \
                                                \
    node_t* left    = nullptr;                  \
    node_t* righ    = nullptr;                  \
    node_t* parent  = nullptr;

node_t* getFuncs()
{
    /* init part */
        INIT_STUFF;
        token = tokens[POSE_];

    /* should be func name at first */
    CRITICAL_ERR (token.type != IDENT_, nullptr, SYNT_ERR_, nullptr, nullptr);
    parent = tree_new_node (treep, &token, &ERROR_INFO_.error_);
    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);

    /* brace arter func name */
    token = tokens[++POSE_];
    CRITICAL_ERR (nameCmpr (token.data.name, TR_LBRACE1_), nullptr, SYNT_ERR_, parent, nullptr);

    /* func args parsing */
        /* getting args */
        token = tokens[++POSE_];
        while (token.type == IDENT_)
        {
            node_t* new_farg = tree_new_node (treep, &token, &ERROR_INFO_.error_);
            CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, left, parent);

            if (left != nullptr)
            {
                tree_att_node (treep, new_farg, left, LEFT_, &ERROR_INFO_.error_);
                left = new_farg;
            }

            else left = new_farg;

            token = tokens[++POSE_];
        }

        /* args attach */
        if (left != nullptr)
            tree_att_node (treep, parent, left, LEFT_, &ERROR_INFO_.error_);

        left = parent;

    /* braces check */
        CRITICAL_ERR (nameCmpr (token.data.name, TR_RBRACE1_), nullptr, SYNT_ERR_, left, nullptr);
        token = tokens[++POSE_];
        CRITICAL_ERR (nameCmpr (token.data.name, TR_LBRACE2_), nullptr, SYNT_ERR_, left, nullptr);

    /* getting & attaching func body */
        POSE_++;
        righ = getLR ();
        CRITICAL_ERR (ERROR_INFO_.error_ != OK_   , nullptr, OK_      , parent, nullptr);
        CRITICAL_ERR (righ == nullptr, nullptr, SYNT_ERR_, parent, nullptr);
        tree_att_node (treep, parent, righ, RIGH_, &ERROR_INFO_.error_);

    /* close func body brace check */
        token = tokens[POSE_];
        CRITICAL_ERR (nameCmpr (token.data.name, TR_RBRACE2_), nullptr, SYNT_ERR_, parent, nullptr);

    /* next tok check, if no EOF - next func parce */
        token = tokens[POSE_ + 1];
        if (token.type != UNDF_T_)
        {
            token = { KEY_, -1, { LR_[0], LR_[1] } };
            GET_N_APPLY (getFuncs);
        }

        else POSE_++;

    return left;
}

node_t* getLR()
{
    /* init part */
    INIT_STUFF;
    token = tokens[POSE_];

    /* there can be expression or construction */
        left = getConstr ();
        CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);
        if (left == nullptr)
        {
            left = getExpr ();
            CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);

            token = tokens[POSE_];
            if (left == nullptr)
            {
                if (nameCmpr (token.data.name, TR_LEX_END_) &&
                    nameCmpr (token.data.name, TR_ADDITIONAL_LEX_END_))
                    return nullptr;
            }

            else
            {
                CRITICAL_ERR (nameCmpr (token.data.name, TR_LEX_END_) &&
                              nameCmpr (token.data.name, TR_ADDITIONAL_LEX_END_),
                              nullptr, SYNT_ERR_, left, nullptr);
            }

            POSE_++;
        }

    /* connecting with LR token */
        token = { KEY_, -1, { LR_[0], LR_[1] } };
        parent = tree_new_node (treep, &token, &ERROR_INFO_.error_);
        CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, left, nullptr);
        if (left != nullptr)
            tree_att_node (treep, parent, left, LEFT_, &ERROR_INFO_.error_);

    /* try to get next LR */
        righ = getLR ();
        CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, parent, nullptr);
        if (righ != nullptr)
            tree_att_node (treep, parent, righ, RIGH_, &ERROR_INFO_.error_);

    return parent;
}

node_t* getConstr()
{
    /* init part */
    INIT_STUFF;
    token = tokens[POSE_];

    /* there is key token */
    if (token.type == KEY_)
    {
        switch (token.data.name[0])
        {
            case KEY_IF_[0]:
            case KEY_WHILE_[0]:
                if (!nameCmpr (token.data.name, KEY_IF_) ||
                    !nameCmpr (token.data.name, KEY_WHILE_))
                {
                    POSE_++;
                    left = getExpr ();
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_  , nullptr, OK_      , nullptr, nullptr);
                    CRITICAL_ERR (left == nullptr, nullptr, SYNT_ERR_, nullptr, nullptr);

                    CRITICAL_ERR (nameCmpr ((tokens)[POSE_].data.name, TR_LBRACE2_),
                                  nullptr, SYNT_ERR_, left, nullptr);

                    GET_N_APPLY (getLR)

                    CRITICAL_ERR (nameCmpr ((tokens)[POSE_++].data.name, TR_RBRACE2_),
                        nullptr, SYNT_ERR_, left, nullptr);
                }

                break;

            case KEY_RETURN_[0]:
                if (!nameCmpr (token.data.name, KEY_RETURN_))
                {
                    parent = tree_new_node (treep, &token, &ERROR_INFO_.error_);
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);

                    POSE_++;

                    left = getExpr ();
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);
                    CRITICAL_ERR (left == nullptr, nullptr, SYNT_ERR_, nullptr, nullptr);
                    CRITICAL_ERR (nameCmpr ((tokens)[POSE_++].data.name, TR_LEX_END_) &&
                                  nameCmpr ((tokens)[POSE_  ].data.name, TR_ADDITIONAL_LEX_END_),
                        nullptr, SYNT_ERR_, left, nullptr);

                    tree_att_node (treep, parent, left, LEFT_, &ERROR_INFO_.error_);

                    left = parent;
                }

                break;

            case KEY_BREAK_[0]:
                if (!nameCmpr (token.data.name, KEY_BREAK_))
                {
                    left = tree_new_node (treep, &token, &ERROR_INFO_.error_);
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);
                    POSE_++;
                }

                break;

            case KEY_VAR_DEC_[0]:
            case KEY_INPUT_  [0]:
            case KEY_OUTPUT_ [0]:
                if (!nameCmpr (token.data.name, KEY_VAR_DEC_) ||
                    !nameCmpr (token.data.name, KEY_OUTPUT_)  ||
                    !nameCmpr (token.data.name, KEY_INPUT_))
                {
                    parent = tree_new_node (treep, &token, &ERROR_INFO_.error_);
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);

                    token = (tokens)[++POSE_];

                    CRITICAL_ERR (token.type != IDENT_, nullptr, SYNT_ERR_, parent, nullptr);

                    left = tree_new_node (treep, &token, &ERROR_INFO_.error_);
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, parent, nullptr);
                    tree_att_node (treep, parent, left, LEFT_, &ERROR_INFO_.error_);

                    left = parent;

                    POSE_++;
                }

                break;

            default:
                return nullptr;
        }

        return left;
    }

    return nullptr;
}

node_t* getExpr()
{
    return get4Prior ();
}

node_t* get4Prior()
{
    INIT_STUFF;
    left = get3Prior ();
    if (left == nullptr) return nullptr;

    token = tokens[POSE_];

    while (token.type == KEY_)
    {
        if (!nameCmpr (token.data.name, KEY_ASSIGN_))
        {
            GET_N_APPLY (get3Prior);
        }

        else return left;
    }

    return left;
}

node_t* get3Prior()
{
    INIT_STUFF;
    left = get2Prior ();
    if (left == nullptr) return nullptr;

    token = tokens[POSE_];

    while (token.type == KEY_)
    {
        switch (token.data.name[0])
        {
            case KEY_PLUS_ [0]:
            case KEY_MINUS_[0]:
            if (!nameCmpr (token.data.name, KEY_PLUS_ ) ||
                !nameCmpr (token.data.name, KEY_MINUS_))
                GET_N_APPLY (get2Prior)
                break;

            default:
                return left;
        }
    }

    return left;
}

node_t* get2Prior()
{
    INIT_STUFF;
    left = get1Prior ();
    if (left == nullptr) return nullptr;

    token = tokens[POSE_];

    while (token.type == KEY_)
    {
        switch (token.data.name[0])
        {
            case KEY_MULTIPLY_[0]:
            case KEY_DIVIDE_  [0]:
            if (!nameCmpr (token.data.name, KEY_MULTIPLY_ ) ||
                !nameCmpr (token.data.name, KEY_DIVIDE_   ))
                GET_N_APPLY (get1Prior);
                break;

            default:
                return left;
        }
    }

    return left;
}

node_t* get1Prior()
{
    INIT_STUFF;
    left = getBraces ();
    if (left == nullptr) return nullptr;

    token = tokens[POSE_];
    
    while (token.type == KEY_)
    {
        switch (token.data.name[0])
        {
            case KEY_POWER_[0]:
            if (!nameCmpr (token.data.name, KEY_POWER_))
                GET_N_APPLY (getBraces);
                break;

            default:
                return left;
        }
    }

    return left;
}

node_t* getBraces()
{
    INIT_STUFF
    token = tokens[POSE_];
    node_t* ident = nullptr;

    /* parse */
    switch (token.type)
    {
        case TERM_:

            if (!nameCmpr (token.data.name, TR_LBRACE1_))
            {
                POSE_++;
                node_t* br_expr = get4Prior ();
                token = tokens[POSE_];

                CRITICAL_ERR (nameCmpr (token.data.name, TR_RBRACE1_), nullptr, SYNT_ERR_, br_expr, nullptr);

                POSE_++;
                return br_expr;
            }

            return nullptr;

        case NUM_:

            POSE_++;

            if (treep->root_->data.type == UNDF_T_)
            {
                treep->root_->data = token;
                return treep->root_;
            }

            return tree_new_node (treep, &token, &ERROR_INFO_.error_);

        case IDENT_:

            ident = tree_new_node (treep, &token, &ERROR_INFO_.error_);
            CRITICAL_ERR (ident == nullptr, nullptr, OK_, nullptr, nullptr);

            if (!nameCmpr (tokens[++POSE_].data.name, TR_LBRACE1_))
            {
                node_t* prev_arg = ident;
                for (token = tokens[++POSE_]; token.type == IDENT_; token = tokens[++POSE_])
                {
                    node_t* new_arg = tree_new_node (treep, &token, &ERROR_INFO_.error_);
                    CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, ident, nullptr);
                    tree_att_node (treep, prev_arg, new_arg, LEFT_, &ERROR_INFO_.error_);
                    prev_arg = new_arg;
                }

                CRITICAL_ERR (nameCmpr ((tokens)[POSE_++].data.name, TR_RBRACE1_),
                    nullptr, SYNT_ERR_, ident, nullptr);

                Token_t func_marker_token = { KEY_ };
                node_t* func_marker_nodep = tree_new_node (treep, &func_marker_token, &ERROR_INFO_.error_);
                CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, ident, nullptr);

                tree_att_node (treep, ident, func_marker_nodep, RIGH_, &ERROR_INFO_.error_);

                return ident;
            }

            /* TODO: var check */
            if (searchVar (CMPLP_, token.data.name) < 0)
                memcpy ((CMPLP_->variables_ + CMPLP_->last_var_id_), token.data.name, NAME_SIZE_);

            if (treep->root_->data.type == UNDF_T_)
            {
                tree_del_subt (treep, ident, &ERROR_INFO_.error_);
                treep->root_->data = token;
                return treep->root_;
            }

            return ident;


        /* if there is in-build key-word function (sin, cos ...) */ /* MAYBE SPLASH DSL? */
        case KEY_:

            if (!nameCmpr (token.data.name, KEY_RETURN_) ||
                !nameCmpr (token.data.name, KEY_VAR_DEC_))
                 return nullptr;

            switch (token.data.name[0])
            {
                #define DEF_KEY( NAME_, CODE_FT_, CODE_SD_, OPRS_INFO_, ASM_NAME_ ) \
                case KEY_##NAME_[0]:                                                \
                    if (OPRS_INFO_ != UNR_) return nullptr;                         \
                    break;

                #include "DSL_KEY.h"
                #undef DEF_KEY
            }

            node_t* un_func = tree_new_node (treep, &token, &ERROR_INFO_.error_);
            CRITICAL_ERR (ERROR_INFO_.error_ != OK_, nullptr, OK_, nullptr, nullptr);

            CRITICAL_ERR (nameCmpr (tokens[++POSE_].data.name, TR_LBRACE1_),
                            nullptr, SYNT_ERR_, un_func, nullptr)

            POSE_++;

            node_t* args = get4Prior ();
            CRITICAL_ERR (ERROR_INFO_.error_ != OK_  , nullptr, OK_      , un_func, nullptr);
            CRITICAL_ERR (args == nullptr, nullptr, SYNT_ERR_, un_func, nullptr);
            CRITICAL_ERR (nameCmpr (tokens[POSE_++].data.name, TR_RBRACE1_),
                            nullptr, SYNT_ERR_, un_func, nullptr)

            tree_att_node (treep, un_func, args, RIGH_, &ERROR_INFO_.error_);
            return un_func;
    
    }

    return nullptr;
}

int searchVar( Compiler* cmpl, NamePart_t name[NAME_LEN_] )
{
    /* just cmpr names */
    for (size_t var_id = 0; var_id < cmpl->last_var_id_; var_id++)
    {
        if (!nameCmpr (name, (NamePart_t* )(cmpl->variables_ + var_id)))
        {
            return var_id;
        }
    }

    /* no such variable */
    return -1;
}

#undef GET_N_APPLY
#undef CRITICAL_ERR