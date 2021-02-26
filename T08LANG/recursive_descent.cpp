
/* to delete sub trees if there are errors */
#define DEL( TO_DEL_ )                              \
    if (TO_DEL_ != nullptr)                         \
        tree_del_subt (treep, TO_DEL_, &ERROR_);

/* actions in case of error */
#define CRITICAL_ERR( TO_RET_ , TO_CHECK_, ERR_STR_,                \
                      TO_DEL1_, TO_DEL2_ )                          \
if (TO_CHECK_)                                                      \
{                                                                   \
    fprintf (stderr, "\n\n" "REC DESC ERROR: " ERR_STR_ "\n\n");    \
                                                                    \
    DEL (TO_DEL1_)                                                  \
    DEL (TO_DEL2_)                                                  \
                                                                    \
    return TO_RET_;                                                 \
}

err_t setData( Compiler* const cmplp )
{
    CMPLP_   = cmplp;
    return OK;
}

err_t getTree()// освобождать память
{
    tree_t* treep = cmpl_get_treep (CMPLP_);

    CRITICAL_ERR (BAD_PTR, isBadPtr (CMPLP_)  , "BAD CALCP_ PTR", nullptr, nullptr);

    POSE_ = 0;

    treep->root_->data.type = UNDF_T_;

    getFunc ();

    if (ERROR_ != OK || CMPLP_->tokens_[POSE_].type != UNDF_T_)
    {
        fprintf (stderr, "SYNTAX ERROR" "\n\n");
    }

    tree_dump (treep);

    return ERROR_;
}

/* define to get righ sub tree and apply all (if needed) */
#define GET_N_APPLY( FUNC )                                         \
{                                                                   \
    /* creating parent node */                                      \
    parent = tree_new_node (treep, &token, &ERROR_);                \
    if (ERROR_ != OK)                                               \
    {                                                               \
        if (left != nullptr)                                        \
            free (left);                                            \
        return nullptr;                                             \
    }                                                               \
                                                                    \
    tree_att_node (treep, parent, left, LEFT_, &ERROR_);            \
                                                                    \
    POSE_++;                                                        \
    /* parsing righ part */                                         \
    righ = FUNC ();                                                 \
    if (righ == nullptr)                                            \
    {                                                               \
        DEL (parent);                                               \
        return nullptr;                                             \
    }                                                               \
                                                                    \
    tree_att_node (treep, parent, righ, RIGH_, &ERROR_);            \
                                                                    \
    /* if there are many tokens with such format,                   \
     * then curr parent will be left for next parse iteration       \
     */                                                             \
    left = parent;                                                  \
                                                                    \
    token = CMPLP_->tokens_[POSE_];                                 \
                                                                    \
}

/* define with all needed declarations */
#define INIT_STUFF                              \
    if (ERROR_ != OK) return nullptr;           \
                                                \
    data_t  token = POISON_;                    \
                                                \
    tree_t* treep = cmpl_get_treep (CMPLP_);    \
                                                \
    node_t* left    = nullptr;                  \
    node_t* righ    = nullptr;                  \
    node_t* parent  = nullptr;

node_t* getFunc()
{
    INIT_STUFF;

    token = CMPLP_->tokens_[POSE_];

    CRITICAL_ERR (nullptr, token.type != IDENT_,
                  "can't get func", nullptr, nullptr);

    parent = tree_new_node (treep, &token, &ERROR_);

    POSE_++;
    token = CMPLP_->tokens_[POSE_];
    CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_LBRACE1_),
                  "can't get func", parent, nullptr);

    POSE_++;
    token = CMPLP_->tokens_[POSE_];

    while (token.type == IDENT_)
    {
        node_t* new_var = tree_new_node (treep, &token, &ERROR_);

        if (left != nullptr)
        {
            tree_att_node (treep, new_var, left, LEFT_, &ERROR_);
            left = new_var;
        }

        else
            left = new_var;

        POSE_++;
        token = CMPLP_->tokens_[POSE_];
    }

    if (left != nullptr)
        tree_att_node (treep, parent, left, LEFT_, &ERROR_);

    left = parent;

    CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_RBRACE1_),
                  "can't get func", left, nullptr);

    POSE_++;
    token = CMPLP_->tokens_[POSE_];
    CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_LBRACE2_),
                  "can't get func", left, nullptr);

    POSE_++;
    righ = getLR();
    tree_att_node (treep, parent, righ, RIGH_, &ERROR_);

    token = CMPLP_->tokens_[POSE_];
    CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_RBRACE2_),
                  "can't get func", left, nullptr);

    token = CMPLP_->tokens_[POSE_ + 1];
    if (token.type != UNDF_T_)
    {
        token = { KEY_, { LR_[0], LR_[1] } };
        GET_N_APPLY (getFunc);
    }

    return left;
}

node_t* getLR()
{
    INIT_STUFF;

    token = CMPLP_->tokens_[POSE_];

    if (token.type == KEY_)
    {
        switch (token.info.name[0])
        {
            case KEY_IF_   [0]  :
            case KEY_WHILE_[0]  :
            if (!cmpl_name_cmpr (token.info.name, KEY_IF_   ) ||
                !cmpl_name_cmpr (token.info.name, KEY_WHILE_))
            {
                POSE_++;

                left = getExpr();

                CRITICAL_ERR (nullptr, cmpl_name_cmpr ((CMPLP_->tokens_)[POSE_].info.name, TR_LBRACE2_) || left == nullptr,
                              "can't get term brace", nullptr, nullptr);

                GET_N_APPLY (getLR)

                token = CMPLP_->tokens_[POSE_];
                CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_RBRACE2_),
                              "can't get term token", parent, nullptr);
                POSE_++;
            }
            break;

            case KEY_RETURN_[0]:
            if (!cmpl_name_cmpr(token.info.name, KEY_RETURN_))
            {
                POSE_++;
                parent = tree_new_node (treep, &token, &ERROR_);
                if (ERROR_ != OK) return nullptr;
                left = getExpr();
                if (left == nullptr)
                {
                    if (ERROR_ == OK)
                        ERROR_ = SYNT_ERR;
                    free (parent);
                    return nullptr;
                }

                token = CMPLP_->tokens_[POSE_];
                if (cmpl_name_cmpr (token.info.name, TR_LEX_END_))
                {
                    ERROR_ = SYNT_ERR;
                    free (parent);
                    DEL (left);
                    return nullptr;
                }

                tree_att_node (treep, parent, left, LEFT_, &ERROR_);

                left = parent;
                POSE_++;
            }
            break;

            case KEY_BREAK_[0]:
            if (!cmpl_name_cmpr(token.info.name, KEY_BREAK_))
            {
                left = tree_new_node (treep, &token, &ERROR_);
                if (ERROR_ != OK) return nullptr;
            }
            break;

            case KEY_VAR_DEC_[0]:
            if (!cmpl_name_cmpr(token.info.name, KEY_VAR_DEC_))
            {
                parent = tree_new_node (treep, &token, &ERROR_);
                if (ERROR_ != OK) return nullptr;

                POSE_++;
                token = CMPLP_->tokens_[POSE_];
                CRITICAL_ERR (nullptr, token.type != IDENT_,
                              "can't get ident token", parent, nullptr);

                left = tree_new_node (treep, &token, &ERROR_);
                CRITICAL_ERR (nullptr, ERROR_ != OK,
                              "tree error", parent, nullptr);

                tree_att_node (treep, parent, left, LEFT_, &ERROR_);
                left = parent;

                POSE_++;
                token = CMPLP_->tokens_[POSE_];
                CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_LEX_END_),
                              "can't get term token", parent, nullptr);
                POSE_++;
            }
            break;

            default:
                CRITICAL_ERR (nullptr, true,
                              "this key symbol can't be here", parent, nullptr);
        }
    }

    else
    {
        left = getExpr ();
        if (left == nullptr)
            return nullptr;

        token = CMPLP_->tokens_[POSE_];
        CRITICAL_ERR (nullptr, cmpl_name_cmpr (token.info.name, TR_LEX_END_),
                      "can't get term token", parent, nullptr);
        POSE_++;
    }

    token = { KEY_, { LR_[0], LR_[1] } };

    parent = tree_new_node (treep, &token, &ERROR_);
    if (ERROR_ != OK)
    {
        if (left != nullptr)
            DEL (left);
        return nullptr;
    }

    tree_att_node (treep, parent, left, LEFT_, &ERROR_);

    righ = getLR ();

    if (righ != nullptr)
        tree_att_node (treep, parent, righ, RIGH_, &ERROR_);

    return parent;
}

node_t* getExpr()
{
    return get3Prior ();
}

node_t* get3Prior()
{
    INIT_STUFF;
    left = get2Prior ();
    if (left == nullptr) return nullptr;

    token = CMPLP_->tokens_[POSE_];

    while (token.type == KEY_)
    {
        switch (token.info.name[0])
        {
            case KEY_PLUS_ [0]:
            case KEY_MINUS_[0]:
            if (!cmpl_name_cmpr (token.info.name, KEY_PLUS_ ) ||
                !cmpl_name_cmpr (token.info.name, KEY_MINUS_))
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

    token = CMPLP_->tokens_[POSE_];

    while (token.type == KEY_)
    {
        switch (token.info.name[0])
        {
            case KEY_MULTIPLY_[0]:
            case KEY_DIVIDE_  [0]:
            if (!cmpl_name_cmpr (token.info.name, KEY_MULTIPLY_ ) ||
                !cmpl_name_cmpr (token.info.name, KEY_DIVIDE_   ))
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

    token = CMPLP_->tokens_[POSE_];
    
    while (token.type == KEY_)
    {
        switch (token.info.name[0])
        {
            case KEY_POWER_[0]:
            if (!cmpl_name_cmpr (token.info.name, KEY_POWER_))
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
    data_t  token = CMPLP_->tokens_[POSE_];
    tree_t* treep = cmpl_get_treep (CMPLP_);

    /* parse */
    switch (token.type)
    {
        case TERM_:
            if (!cmpl_name_cmpr (token.info.name, TR_LBRACE1_))
            {
                POSE_++;
                node_t* br_expr = get3Prior();
                token = CMPLP_->tokens_[POSE_];
                if (token.type == TERM_ && !cmpl_name_cmpr (token.info.name, TR_RBRACE1_))
                {
                    POSE_++;
                    return br_expr;
                }

                /* first brace found, but there are no closing brace */
                DEL (br_expr);
                ERROR_ = SYNT_ERR;
                return nullptr;
            }

            return nullptr;

        case NUM_:

            POSE_++;

            if (treep->root_->data.type == UNDF_T_)
            {
                treep->root_->data = token;
                return treep->root_;
            }

            return tree_new_node (treep, &token, &ERROR_);

        case IDENT_:

            POSE_++;

            if (cmplSearchVar (CMPLP_, token.info.name) < 0)
                memcpy ((CMPLP_->variables_ + CMPLP_->last_var_id_), token.info.name, NAME_SIZE_);

            if (treep->root_->data.type == UNDF_T_)
            {
                treep->root_->data = token;
                return treep->root_;
            }

            return tree_new_node (treep, &token, &ERROR_);
    }

    return nullptr;
}

int cmplSearchVar( Compiler* cmpl, NamePart_t name[NAME_LEN_] )
{
    /* just cmpr names */
    for (size_t var_id = 0; var_id < cmpl->last_var_id_; var_id++)
    {
        if (!cmpl_name_cmpr (name, (NamePart_t* )(cmpl->variables_ + var_id)))
        {
            return var_id;
        }
    }

    /* no such variable */
    return -1;
}

#undef GET_N_APPLY
#undef DEL
