
/* FUNCS FOR tree_t */

    err_t get_data_str( const data_t data, char* const buff )
    {
        switch (data.type)
        {
            /* printing operation name */
            case OPERATION:
            {
                sprintf (buff, "operation" "\\n" "%.*s", NAME_LEN_, &(data.name));
                break;
            }

            /* printing value */
            case NUMBER:
            {
                sprintf (buff, "constant" "\\n" "%lf", data.num);
                break;
            }

            /* printing variables massive id and name */
            case VARIABLE:
            {
                sprintf (buff, "variable" "\\n" "id %d" "\\n" "%.*s", data.var_id, NAME_LEN_, &(data.name));
                break;
            }

            /* undefined */
            default:
            {
                sprintf (buff, "??? UNDEF ???");
                break;
            }
        }
        return OK;
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

/* calctor_t FUNCS */

tree_t* calc_get_treep( const calctor_t* const calc )
{
    /* check calctor.h for calc_TR_SHT_ constand definition*/
    return (tree_t* )((size_t)calc + CALC_TR_SHT_);
}

err_t calc_ctor( calctor_t* const calc )
{
    err_t error = OK;
    /* just constructing data tree */
    tree_ctor (calc_get_treep (calc), &error);

    return error;
}

err_t calc_dstr( calctor_t* const calc )
{
    /* tree verify */
    if (err_t error = tree_verificator (calc_get_treep (calc))) return error;

    err_t error = OK;
    /* just destructing data tree */
    tree_dstr (calc_get_treep (calc), &error);

    return error;
}

bool calc_chck_operands( const bool left_fl, const bool righ_fl, const long long op_code )
{
    switch (op_code)
    {
        #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
        case  OP_##name : return (oper_info ^ ((left_fl << 1) | righ_fl));

        #include "DSL.h"
        #undef DEF_OP
    }

    /* error case */
    return true;
}

int calc_get_prior( NAME_ type )
{
    switch (type)
    {
        #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
        case OP_##name : return priority;

        #include "DSL.h"
        #undef DEF_OP 
    }

    /* variable or number */
    return MIN_PRI;
}

err_t calc_calc( calctor_t* calc )
{
    verify (calc);

    double* vars = calc->var_values;

    /* variables setup */
    for (size_t var_id = 0; var_id < calc->curr_var; var_id++)
    {
        printf ("\n" "input %.*s  ", NAME_LEN_, (char* )(calc->variables + var_id));
        scanf  ("%lf", vars + var_id);
    }

    /* result out */
    printf ("\n" "%lf", calc_sub_t_calc (calc, calc->tree_.root_));
}

/* define to do all stuff with choosed side */
#define SUB_TREE_SWITCH( side )                                                                             \
{                                                                                                           \
    if (side != nullptr)                                                                                    \
    switch (side->data.type)                                                                                \
    {   /* there is NUMBER on choosed side */                                                               \
        case NUMBER :                                                                                       \
        {                                                                                                   \
            side##_val = side->data.num;                                                                    \
            break;                                                                                          \
        }                                                                                                   \
                                                                                                            \
        /* variable */                                                                                      \
        case VARIABLE :                                                                                     \
        {                                                                                                   \
            side##_val = calc->var_values[side->data.var_id];                                               \
            break;                                                                                          \
        }                                                                                                   \
                                                                                                            \
        case OPERATION:                                                                                     \
        {                                                                                                   \
            side##_val = calc_sub_t_calc (calc, side);                                                      \
            break;                                                                                          \
        }                                                                                                   \
                                                                                                            \
        default:                                                                                            \
        {                                                                                                   \
            return UNDEF_ERR;                                                                               \
        }                                                                                                   \
    }                                                                                                       \
}

double calc_sub_t_calc( calctor_t* calc, node_t* nodep )
{
    /* getting tree ptr */
    tree_t* treep = calc_get_treep (calc);

    /* ptrs check */
    if (isBadPtr (calc) || isBadPtr (treep) || isBadPtr (nodep))
    {
        fprintf (stderr, "\n\n" "BAD PTR IN calc_sub_t_calc" "\n\n");
        return BAD_PTR;
    }

    /* for error output */
    err_t error = OK;
    /* for handle access */
    data_t data = nodep->data;

    /* left sub tree value */
    double left_val = NAN;
    /* right sub tree value */
    double righ_val = NAN;

    /* left and right sub trees */
    node_t* left = tree_get_left(treep, nodep, &error);
    node_t* righ = tree_get_righ(treep, nodep, &error);

    /* there is left sub tree */
    SUB_TREE_SWITCH (left);

    /* there is righ sub tree */
    SUB_TREE_SWITCH (righ);

    /* performing operation */
    switch (data.name)
    {
        #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
        case OP_##name : return calca;

        #include "DSL.h"
        #undef DEF_OP()
    }

    /* shouldn't get here */
    assert(0);
}

#undef SUB_TREE_SWITCH()

err_t calc_verify( calctor_t* calc )
{
    /* getting tree ptr */
    tree_t* treep = calc_get_treep (calc);

    /* tree verification */
    err_t error = tree_verificator (treep);
    if (error)
    {
        return error;
    }

    /* operations operations check */
    error = calc_sub_t_op_chk (calc, treep->root_);

    return error;
}

err_t calc_sub_t_op_chk( calctor_t* calc, node_t* nodep )
{
    /* getting tree ptr */
    tree_t* treep = calc_get_treep (calc);
    /* for handle access */
    data_t data = nodep->data;
    /* error check */
    err_t error = OK;

    /* ptrs check */
    if (isBadPtr (calc) || isBadPtr (treep) || isBadPtr (nodep))
    {
        fprintf (stderr, "\n\n" "BAD PTR IN calc_sub_t_op_chk" "\n\n");
        return BAD_PTR;
    }

    /* sub trees */
    node_t* left = tree_get_left (treep, nodep, &error);
    node_t* righ = tree_get_righ (treep, nodep, &error);

    /* tree error */
    if (error)
    {
        return error;
    }

    /* sides flags */
    bool left_fl = left != nullptr;
    bool righ_fl = righ != nullptr;

    switch (data.type)
    {
        /* error */
        case UNDEF_U: return UNDF_UNIT_ERR;

        case NUMBER:
        case VARIABLE:
        {
            /* should not have childs */
            if (!left_fl && !righ_fl)
            {
                /* variable check */
                if (data.type == NUMBER || calc_search_var (calc, data.name) >= 0)
                {
                    break;
                }

                return VAR_ERR;
            }

            return CHLD_ERR;
        }

        case OPERATION:
        {
            /* operands check */
            if (calc_chck_operands (left_fl, righ_fl, data.name))
            {
                return SYNT_ERR;
            }

            /* recoursive check */
            if (left_fl)
                if (err_t error = calc_sub_t_op_chk (calc, left))
                {
                    return error;
                }

            /* recoursive check */
            if (righ_fl)
                if (err_t error = calc_sub_t_op_chk (calc, righ))
                {
                    return error;
                }

            break;
        }

        /* unknown type */
        default:
        {
            return UNDEF_ERR;
        }
    }

    /* all checks are OK */
    return OK;
}

err_t calc_diff( calctor_t* expr, calctor_t* diff, size_t var_id )
{
    verify (expr);

    if (var_id >= expr->curr_var)
    {
        return BAD_VAR;
    }

    /* trees ptrs */
    tree_t* exprt = calc_get_treep (expr);
    tree_t* difft = calc_get_treep (diff);

    if (err_t error = tree_verificator (difft)) return error;

    /* copying var names */
    for (size_t i = 0; i < expr->curr_var; i++)
    {
        diff->variables[i] = expr->variables[i];
    }
    diff->curr_var = expr->curr_var;

    /* rec func call */
    return calc_sub_t_diff (expr, diff, exprt->root_, difft->root_);
}

/* diff defines */
#define ROOT_ASSIGN_( NAME, LEFT, RIGH )                             \
{                                                                    \
    diff_datap->type = OPERATION;                                    \
    diff_datap->name = NAME;                                         \
                                                                     \
    node_t* left = LEFT;                                             \
    if (left != nullptr)                                             \
        tree_att_node (diff_treep, diff_nodep, left, LEFT_, &error); \
                                                                     \
    node_t* righ = RIGH;                                             \
    if (righ != nullptr)                                             \
        tree_att_node (diff_treep, diff_nodep, righ, RIGH_, &error); \
                                                                     \
    return OK;                                                       \
}

#define NEW( type, oper, num, id, LEFT, RIGH )                                              \
    new_ptr = tree_new_node (diff_treep, &(new_data = { type, oper, num, id }), &error);    \
                                                                                            \
    {                                                                                       \
        node_t* tmp_loc_new = new_ptr;                                                      \
        node_t* subt_ptr = LEFT;                                                            \
        new_ptr = tmp_loc_new;                                                              \
        if (subt_ptr != nullptr)                                                            \
            tree_att_node (diff_treep, new_ptr, subt_ptr, LEFT_, &error);                   \
                                                                                            \
        subt_ptr = RIGH;                                                                    \
        new_ptr = tmp_loc_new;                                                              \
        if (subt_ptr != nullptr)                                                            \
            tree_att_node (diff_treep, new_ptr, subt_ptr, RIGH_, &error);                   \
    }

#define NEW_OPR( operation, LEFT, RIGH ) \
NEW ( OPERATION, operation,    NAN, -1, LEFT, RIGH)

#define NEW_NUM( num ) \
NEW ( NUMBER,    OP_UNDEFINED, num, -1, nullptr, nullptr)

#define d( side )                                                       \
    new_ptr = tree_new_node (diff_treep, &undef, &error);               \
    if (error)                                                          \
        return error;                                                   \
                                                                        \
    calc_sub_t_diff (expr, diff,                                        \
    tree_get_##side (expr_treep, expr_nodep, &error), new_ptr);

#define c( side )                                                       \
    new_ptr = tree_new_node (diff_treep, &undef, &error);               \
    if (error)                                                          \
        return error;                                                   \
                                                                        \
    tree_copy (expr_treep, diff_treep,                                  \
    tree_get_##side (expr_treep, expr_nodep, &error), new_ptr, &error);

#define OP( symbol ) symbol << (8 * BYTE_SHT (NAME_LEN_, 0))

err_t calc_sub_t_diff( calctor_t* expr, calctor_t* diff, node_t* expr_nodep, node_t* diff_nodep )
{
    if (isBadPtr (expr_nodep))
        return BAD_PTR;

    /* trees ptrs */
    tree_t* expr_treep = calc_get_treep (expr);
    tree_t* diff_treep = calc_get_treep (diff);

    /* expr node data */
    data_t  expr_data  = expr_nodep->data;
    /* ptr to diff node data */
    data_t* diff_datap = &(diff_nodep->data);

    /* for new data */
    data_t new_data = {};
    const data_t undef    = {};

    /* to saave tmp pointers to diff or copied sub trees */
    node_t* subt_ptr    = nullptr;
    node_t* new_ptr     = nullptr;

    /* for errors output */
    err_t error = OK;

    switch (expr_data.type)
    {
        /* if there is diff variable -> have 1 in diff node */
        case VARIABLE:
        {
            if (expr_nodep->data.var_id == diff->diff_var_id)
            {
                diff_datap->type = NUMBER;
                diff_datap->num = 1;

                return OK;
            }
        }

        /* if there is num or not diff var -> have 0 in diff node */
        case NUMBER:
        {
            diff_datap->type = NUMBER;
            diff_datap->num = 0;

            return OK;
        }

        /* OPERATIONS DIFF */
        case OPERATION:
        {
            switch (expr_data.name)
            {
                /* dL +- dR */
                case OP_MINUS:
                case OP_PLUS:
                    ROOT_ASSIGN_ (expr_data.name, d(left), d(righ))

                /* dL * cR + cL * dR */
                case OP_MULTIPLY:
                    ROOT_ASSIGN_ (OP('+'), NEW_OPR (OP('*'), c(left), d(righ)), NEW_OPR(OP('*'), c(righ), d(left)));

                case OP_DIVIDE:
                    ROOT_ASSIGN_ (OP('/'),
                    /* numerator */
                    NEW_OPR (OP('-'), NEW_OPR (OP('*'), c(righ), d(left)), NEW_OPR (OP('*'), d(righ), c(left))),
                    /* denominator */
                    NEW_OPR (OP('*'), c(righ), c(righ)));

                /* f(x)^g(x) * ((g(x)/f(x)) * f'(x) + ln(f(x)) * g'(x)) */
                case OP_POWER:
                {
                    /* const^f(x) */
                    if (!calc_rec_var_search (expr_nodep->left_ch))
                    {
                        ROOT_ASSIGN_ (OP('*'), d(righ), NEW_OPR(OP('*'), NEW_OPR(OP_LN, nullptr, c(left)), NEW_OPR(OP('^'), c(left), c(righ))))
                    }

                    if (!calc_rec_var_search (expr_nodep->righ_ch))
                    {
                        ROOT_ASSIGN_ (OP('*'), d(left), NEW_OPR(OP('*'), c(righ), NEW_OPR(OP('^'), c(left), NEW_OPR(OP('-'), c(righ), NEW_NUM(1)))))
                    }

                    ROOT_ASSIGN_ (OP('*'),
                    /* copying power part */
                    NEW_OPR (OP('^'), c(left), c(righ)),
                    /* adding inside func dir */
                    NEW_OPR (OP('+'), NEW_OPR (OP('*'), NEW_OPR (OP('/'), c(left), c(righ)), d(left)), NEW_OPR (OP('*'), d(righ), NEW_OPR (OP_LN, nullptr, c(left)))))

                }
                /* f'(x) / f(x) */
                case OP_LN:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    c(righ))

                /* f'(x) / cos^2(f(x)) */
                case OP_TAN:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR(OP('^'), NEW_OPR(OP_COS, nullptr, c(righ)), NEW_NUM(2)))

                /* f'(x) / (0 - sin^2(f(x))) */
                case OP_CTG:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR(OP('-'), NEW_NUM(0), NEW_OPR(OP('^'), NEW_OPR(OP_COS, nullptr, c(righ)), NEW_NUM(2))))

                /* f'(x) * cos(x) */
                case OP_SIN:
                    ROOT_ASSIGN_ (OP('*'), d(righ),
                    NEW_OPR(OP_COS, nullptr, c(righ)))

                /* f'(x) * -sin(x) */
                case OP_COS:
                    ROOT_ASSIGN_ (OP('*'), d(righ),
                    NEW_OPR('-', NEW_NUM(0), NEW_OPR(OP_COS, nullptr, c(righ))))

                /* f'(x) / sqrt(1 - f(x)^2) */
                case OP_ASIN:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR('^', NEW_OPR('-', NEW_NUM(1), NEW_OPR('^', c(righ), NEW_NUM(2))), NEW_NUM(0.5)))

                /* -f'(x) / sqrt(1 - f(x)^2) */
                case OP_ACOS:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR('-', NEW_NUM(0), NEW_OPR('^', NEW_OPR('-', NEW_NUM(1), NEW_OPR('^', c(righ), NEW_NUM(2))), NEW_NUM(0.5))))

                /* f'(x) / (1 + f(x)^2) */
                case OP_ATAN:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR('+', NEW_NUM(1), NEW_OPR('^', c(righ), c(righ))))

                /* -f'(x) / (1 + f(x)^2) */
                case OP_ACTG:
                    ROOT_ASSIGN_ (OP('/'), d(righ),
                    NEW_OPR('-', NEW_NUM(-1), NEW_OPR('^', c(righ), c(righ))))

                /* f'(x) * sh(f(x)) */
                case OP_CH:
                    ROOT_ASSIGN_ (OP('*'), d(righ), NEW_OPR(OP_SH, nullptr, c(righ)))

                /* f'(x) * ch(f(x)) */
                case OP_SH:
                    ROOT_ASSIGN_ (OP('*'), d(righ), NEW_OPR(OP_CH, nullptr, c(righ)))

                /* f'(x) / (ch(f(x))^2) */
                case OP_TGH:
                    ROOT_ASSIGN_ (OP('/'), d(righ), NEW_OPR(OP('^'), NEW_OPR(OP_CH, nullptr, c(righ)), NEW_NUM(2)))

                /* f'(x) / (ch(f(x))^2) */
                case OP_CTH:
                    ROOT_ASSIGN_ (OP('/'), d(righ), NEW_OPR('-', NEW_NUM(0), NEW_OPR(OP('^'), NEW_OPR(OP_CH, nullptr, c(righ)), NEW_NUM(2))))

                default:
                    return UNDEF_ERR;
            }
        }
    }
}

#undef d()
#undef c()
#undef OP()
#undef ROOT_ASSIGN_()
#undef NEW_OPR()
#undef NEW_NUM()
#undef NEW()

bool calc_rec_var_search( const node_t* const node )
{
    assert (node == nullptr || !isBadPtr(node));

    if (node == nullptr)
        return false;

    if (node->data.type == VARIABLE)
        return true;

    return calc_rec_var_search (node->left_ch) || calc_rec_var_search (node->righ_ch);
}

/* just simple local util to compare double numbers */
bool double_cmp( double ft, double sd )
{
    const double DELTA_ = 1E-05;

    return fabs (ft - sd) < DELTA_;
}

err_t calc_simpl( calctor_t* calc )
{
    verify (calc);

    calc->simplify_flag = true;

    err_t error = OK;

    while (calc->simplify_flag)
    {
        calc->simplify_flag = false;

        error = calc_neutral_fold (calc, nullptr);
        if (error) return error;
        error = calc_const_fold (calc, nullptr);
        if (error) return error;
    }

    return OK;
}

void calc_latex( const calctor_t* const calc )
{
    if (isBadPtr (calc))
    {
        fprintf (stderr, "\n\n" "BAD PTR IN LATEX DUMP FUNC!" "\n\n");
        return;
    }

    const tree_t* treep = calc_get_treep (calc);

    /* max size of dump str */
    static const int MAX_DUMP_LEN_ = 8192;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char), MAX_DUMP_LEN_);
    if (buff == nullptr)
    {
        fprintf (stderr, "\n\nNO MEMORY FOR DUMP BUFFER!\n\n");
        return;
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info for latex */
    buff_shift += sprintf (buff + buff_shift, "\\documentclass[11pt,a4paper,oneside]{article}\n"
                                              "\\usepackage [utf8x] {inputenc}\n"
                                              "\\usepackage{amsmath}\n\n"

    #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
    "\\newcommand{"#latex"}[2]{"#latex_todo"}\n"
    #include "DSL.h"
    
    "\\begin{document}\n"
    "\\begin{equation}\n");

    #undef DEF_OP

    /* recoursive tree dump */
    int numof_dumped_nodes_ = 0;
    buff_shift += calc_sub_t_latex (treep, treep->root_, buff + buff_shift, &numof_dumped_nodes_);

    /* end of latex */
    buff_shift += sprintf (buff + buff_shift, "\n" "\\end{equation}" "\n" "\\end{document}");

    /* creating file */
    FILE* out_file = fopen ("latex.tex", "wb");
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
    // sprintf (cmd, "dot -Tpdf dump_file.dot > %s", tree->dump_name_);
    // system (cmd);

    /* dump out */
    // sprintf (cmd, "%s", "latex.tex");
    // system (cmd);

    return;
}

int calc_sub_t_latex( const tree_t* treep, node_t* nodep, char* const buff, int* numof_dumped_nodes_ )
{
    if (isBadPtr (buff))
    {
        fprintf (stderr, "\n\n" "BAD BUFF PTR IN DUMP" "\n\n");
        return 0;
    }

    /* helps if tree cycled */
    if (*numof_dumped_nodes_ >= treep->size_)
    {
        return 0;
    }

    if (isBadPtr (nodep))
    {
        return 0;
    }

    (*numof_dumped_nodes_)++;

    size_t buff_shift = 0;
    data_t data = nodep->data;

    if (data.type == UNDEF_U)
    {
        buff_shift += sprintf (buff + buff_shift, "(undef)");
        return buff_shift;
    }

    /* parent priority check */
    int parent_prior = nodep->parent != nullptr ? calc_get_prior (nodep->parent->data.name) : MIN_PRI;
    /* there are func => incr priority for braces */
    if (parent_prior == MAX_PRI)
    {
        parent_prior++;
    }

    /* braces are nedded if parent node have higher priority, but if there is divide operation in this node or in paren node, braces aren't needed */
    bool br_need = nodep->data.type == OPERATION && nodep->parent != nullptr && data.name != OP_DIVIDE && nodep->parent->data.name != OP_DIVIDE && calc_get_prior (data.name) < parent_prior;

    if (br_need)
    {
        buff_shift += sprintf (buff + buff_shift, "%s", OPN_BR_S_);
    }

    /* node data print */
    if (data.type == OPERATION)
    {
        buff_shift += get_latex_name (data.name, buff + buff_shift);
    }

    else if (data.type == VARIABLE)
    {
        buff_shift += sprintf (buff + buff_shift, "%.*s", NAME_LEN_ , &(data.name));
    }

    else if (data.type == NUMBER)
    {
        /* dont print if there are left zero in minus operation */
        if (nodep->parent == nullptr || nodep->parent->data.name != OP_MINUS ||
            nodep->parent->left_ch != nodep || !double_cmp (data.num, 0))
        {
            if (double_cmp (data.num, (int)data.num))
                buff_shift += sprintf (buff + buff_shift, "%d", (int)data.num);
            else
                buff_shift += sprintf (buff + buff_shift, "%.2lf", data.num);
        }
    }

    /* left lex print */
    buff_shift += sprintf (buff + buff_shift, "{");
    node_t* left = nodep->left_ch;
    buff_shift += calc_sub_t_latex (treep, left, buff + buff_shift, numof_dumped_nodes_);
    buff_shift += sprintf (buff + buff_shift, "}");

    /* righ lex print */
    buff_shift += sprintf (buff + buff_shift, "{");
    node_t* righ = nodep->righ_ch;
    buff_shift += calc_sub_t_latex (treep, righ, buff + buff_shift, numof_dumped_nodes_);
    buff_shift += sprintf (buff + buff_shift, "}");

    if (br_need)
    {
        buff_shift += sprintf (buff + buff_shift, "%s", CLS_BR_S_);
    }

    return buff_shift;
}

int get_latex_name( NAME_ name, char* buff )
{
    switch (name)
    {
        #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
        case OP_##name : return sprintf (buff, "%s" , #latex);

        #include "DSL.h"
        #undef DEF_OP
    }

    return sprintf (buff, "(undef)");
}

const char  PHRASES_[][MAX_STR_LEN_] = {

        "Можно заметить, что полученное уравнение эквивалентно данному:"                ,
        "Тогда истинно:"                                                                ,
        "Проделав преобразования, получим:"                                             ,
        "Можно заметить, что тогда:"                                                    ,
        "Доказательство следующего перехода мы оставим читателю в качестве упражнения:" ,
        "Из коммутативности получим:"                                                   ,
        "Домножив и разделив на $\\frac{\\pi}{666}$ получим:"                           ,
        "Совершив предельный переход получим:"                                          ,
        "Методом пристального вглядывания в картинку докажем, что:"                     ,
        "Согласно источнику \\cite{1}"                                                  ,
        "Согласно источнику \\cite{2}"                                                  ,
        "Согласно источнику \\cite{3}"                                                  ,
        "Согласно источнику \\cite{4}"
};

const size_t NUMOF_PH_ = sizeof (PHRASES_) / MAX_STR_LEN_;

const char TITLE_[] = {

        "\\begin{titlepage}"
        "\\newgeometry{left=15mm, right = 20mm, bottom = 28mm, top=35mm}"
        "    {\\color[HTML]{000000}" "\n"
        "    \\begin{tikzpicture} [overlay,remember picture]" "\n"
        "    \\draw [line width=0.2mm ]" "\n"
        "    ($ (current page.north west) + (1.7cm, -3.5cm) $)" "\n"
        "    rectangle" "\n"
        "    ($ (current page.south east) + (-1.7cm, 10.3cm) $);" "\n"
        "    \\end{tikzpicture}}" "\n"
        "    \\begin{center}" "\n"
        "    Министерство образования РФ\\\\[0.2cm]" "\n"
        "    Московский физико-технический институт\\\\[0.2cm]" "\n"
        "    2020-2021 учебный год\\\\[0.8cm]" "\n"
        "    Факультет ФРКТ\\\\[0.6cm]" "\n\n"
        
        "    Реферат по теме:\\\\[0.8cm]" "\n\n"
        
        "    {\\huge<<Производные сложных функций и их алгебраические приложения>>}\\\\[5.0cm]" "\n"
        "    \\end{center}" "\n\n"
        
        "    \\begin{flushright}" "\n\n"
        
        "        Выполнил:\\\\[0.3cm]" "\n"
        "        Студент 1 курса\\\\[0.3cm]" "\n"
        "        Группы Б01-006\\\\[0.3cm]" "\n"
        "        Матренин Василий Николаевич\\\\[0.3cm]" "\n"
        "        Проверил:\\\\[0.5cm]" "\n"
        "        Доц. Кафедры нереальной математики\\\\[0.3cm]" "\n"
        "        Жмышенко В. А.\\\\[3.8cm]" "\n\n"
        
        "    \\end{flushright}" "\n\n"
        
        "    \\begin{center}" "\n\n"
        
        "        Г. Москва (Долгопрудный)\\\\" "\n"
        "        2020 г." "\n\n"
        
        "    \\end{center}" "\n\n"
        
        "    \\restoregeometry" "\n\n"
        
        "\\end{titlepage}" "\n\n"
        
        "\\newpage" "\n"
};

const char BIBLIOGRAPHY_[] = {

        "\\begin{thebibliography}{13}" "\n"
        "\\bibitem{1}" "\n"
        "Г.М. Фихтенгольц, Основы математического анализа, 322 том, стр 228 - 1337. \\\\[0.25cm]" "\n"
        "\n"
        "\\bibitem{2}" "\n"
        "\\href{https://vk.com/video?z=video-171296758_456239038}{https://vk.com/video?z=video-171296758\\_456239038} \\\\[0.25cm]" "\n"
        "\n"
        "\\bibitem{3}" "\n"
        "А сейчас быстро нажал(а) на Follow:\\\\" "\n"
        "\\href{https://github.com/VasiliyMatr/T07DIFF}{https://github.com/VasiliyMatr/T07DIFF} \\\\[0.25cm]" "\n"
        "\n"
        "\\bibitem{4}" "\n"
        "\\href{https://www.youtube.com/watch?v=dmdgOov-4q8}{https://www.youtube.com/watch?v=dmdgOov-4q8} \\\\[0.25cm]" "\n"
        "\n"
        "\\end{thebibliography}" "\n"
};

err_t calc_const_fold( calctor_t* calc, joke_info* const info )
{
    verify (calc);

    return calc_sub_t_cfold (calc, calc_get_treep (calc)->root_, info);
}

err_t calc_sub_t_cfold( calctor_t* calc, node_t* nodep, joke_info* const info )
{
    tree_t* treep = calc_get_treep (calc);
    err_t error = OK;

    node_t* left = tree_get_left (treep, nodep, &error);
    node_t* righ = tree_get_righ (treep, nodep, &error);
    data_t data = nodep->data;

    double new_data = NAN;

    if ((left == nullptr || left->data.type == NUMBER) && (righ == nullptr || righ->data.type == NUMBER))
    {
        double left_val = left == nullptr ? NAN : left->data.num;
        double righ_val = righ == nullptr ? NAN : righ->data.num;


        switch (data.name)
        {
            #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
            case OP_##name :        \
            {                       \
                new_data = calca;   \
                break;              \
            }

            #include "DSL.h"
            #undef DEF_OP()

            default:
            {
                assert(0);
            }
        }

        tree_del_subt (treep, left, &error);
        tree_del_subt (treep, righ, &error);

        nodep->data.type = NUMBER;
        nodep->data.name = OP_UNDEFINED;
        nodep->data.num = new_data;
        nodep->left_ch = nullptr;
        nodep->righ_ch = nullptr;
        calc->simplify_flag = true;

        #ifdef RJOMBA

            assert (!isBadPtr (info));
            assert (!isBadPtr (info->buff));
            assert (!isBadPtr (info->diff_treep));

            info->buff_shift += sprintf(info->buff + info->buff_shift,
            "\\noindent%s \\\\[0.15cm]" "\n\n" "\\begin{equation}" "\n", PHRASES_[rand() % NUMOF_PH_]);

            int numof_dumped_nodes_ = 0;
            info->buff_shift += calc_sub_t_latex (info->diff_treep, info->diff_treep->root_, info->buff + info->buff_shift, &numof_dumped_nodes_);
            info->buff_shift += sprintf (info->buff + info->buff_shift, "\n" "\\end{equation}" "\\\\[0.25cm]" "\n\n");

        #endif
    }

    if (nodep->left_ch != nullptr && nodep->left_ch->data.type == OPERATION)
    {
        calc_sub_t_cfold (calc, nodep->left_ch, info);
    }

    if (nodep->righ_ch != nullptr && nodep->righ_ch->data.type == OPERATION)
    {
        calc_sub_t_cfold (calc, nodep->righ_ch, info);
    }

    return OK;
}

err_t calc_neutral_fold ( calctor_t* calc, joke_info* const info )
{
    verify (calc);

    return calc_sub_t_nfold (calc, calc_get_treep (calc)->root_, info);
}

err_t calc_sub_t_nfold( calctor_t* calc, node_t* nodep, joke_info* const info )
{
    tree_t* treep = calc_get_treep (calc);
    err_t error = OK;

    node_t* left = tree_get_left (treep, nodep, &error);
    node_t* righ = tree_get_righ (treep, nodep, &error);
    node_t* prnt = tree_get_prnt (treep, nodep, &error);

    bool del_left_fl = false;
    bool del_righ_fl = false;

    switch (nodep->data.name)
    {
        case OP_PLUS:
        {
            if (left->data.type == NUMBER && double_cmp (left->data.num, 0))
            {
                del_left_fl = true;
                break;
            }
        }

        case OP_MINUS:
        {
            if (righ->data.type == NUMBER && double_cmp (righ->data.num, 0))
            {
                del_righ_fl = true;
            }

            break;
        }

        case OP_DIVIDE:
        {
            if (righ->data.type == NUMBER && double_cmp (righ->data.num, 1))
            {
                del_righ_fl = true;
            }

            if (left->data.type == NUMBER && double_cmp (left->data.num, 0))
            {
                tree_del_subt (treep, left, &error);
                tree_del_subt (treep, righ, &error);

                nodep->data.type = NUMBER;
                nodep->data.name = OP_UNDEFINED;
                nodep->data.num = 0;
                calc->simplify_flag = true;

                return OK;
            }

            break;
        }

        case OP_MULTIPLY:
        {
            if (righ->data.type == NUMBER && double_cmp (righ->data.num, 1))
            {
                del_righ_fl = true;
            }

            if (left->data.type == NUMBER && double_cmp (left->data.num, 1))
            {
                del_left_fl = true;
            }

            if ((left->data.type == NUMBER && double_cmp (left->data.num, 0)) ||
                (righ->data.type == NUMBER && double_cmp (righ->data.num, 0)))
            {
                tree_del_subt (treep, left, &error);
                tree_del_subt (treep, righ, &error);

                nodep->data.type = NUMBER;
                nodep->data.name = OP_UNDEFINED;
                nodep->data.num = 0;
                calc->simplify_flag = true;

                return OK;
            }

            break;
        }

        case OP_POWER:
        {
            if (righ->data.type == NUMBER && double_cmp (righ->data.num, 1))
            {
                del_righ_fl = true;
                break;
            }
        }
    }

    #define DELETE_IF_NEED( tosafe, todel )                             \
    if (del_##todel##_fl)                                               \
    {                                                                   \
        ----treep->size_;                                               \
                                                                        \
        *nodep = *tosafe;                                               \
        if (nodep->left_ch != nullptr) nodep->left_ch->parent = nodep;  \
        if (nodep->righ_ch != nullptr) nodep->righ_ch->parent = nodep;  \
        nodep->parent = prnt;                                           \
                                                                        \
        free (left);                                                    \
        free (righ);                                                    \
                                                                        \
                                                                        \
        calc->simplify_flag = true;                                     \
    }

    DELETE_IF_NEED (left, righ);
    DELETE_IF_NEED (righ, left);

#ifdef RJOMBA

    if (del_left_fl || del_righ_fl)
    {
        assert (!isBadPtr (info));
        assert (!isBadPtr (info->buff));
        assert (!isBadPtr (info->diff_treep));

        info->buff_shift += sprintf(info->buff + info->buff_shift,
        "\\noindent%s \\\\[0.15cm]" "\n\n" "\\begin{equation}" "\n", PHRASES_[rand() % NUMOF_PH_]);
        
        int numof_dumped_nodes_ = 0;
        info->buff_shift += calc_sub_t_latex (info->diff_treep, info->diff_treep->root_, info->buff + info->buff_shift, &numof_dumped_nodes_);
        info->buff_shift += sprintf (info->buff + info->buff_shift, "\n" "\\end{equation}" "\\\\[0.25cm]" "\n\n");
    }

#endif

    left = tree_get_left (treep, nodep, &error);
    righ = tree_get_righ (treep, nodep, &error);

    if (left != nullptr)
    {
        error = calc_sub_t_nfold (calc, left, info);
        if (error) return error;
    }

    if (righ != nullptr)
    {
        error = calc_sub_t_nfold (calc, righ, info);
        if (error) return error;
    }

    return OK;
}

/* define for errors out */
#define ERR_OUT( cond, str )            \
if (cond)                               \
{                                       \
    fprintf (stderr, "\n\n" str "\n\n");\
    return;                             \
}

void calc_perform_joke( const char* expr_file_name )
{

    ERR_OUT (isBadPtr (expr_file_name), "BAD PTR JOKE FUNC!");

    FILE* expr_file = fopen (expr_file_name, "rb");
    ERR_OUT (expr_file == nullptr, "CAN'T OPEN FILE WITH EXPR!");

    /* reading file */
    fseek (expr_file, 0, SEEK_END);
    int num_of_chars = ftell (expr_file) + 1;
    rewind (expr_file);

    char* expr = (char* )calloc (sizeof (char), num_of_chars);
    ERR_OUT (expr == nullptr, "CAN'T ALLOC IN JOKE FUNC!");

    fread (expr, sizeof (char), num_of_chars, expr_file);
    fclose (expr_file);

    calctor_t exprc;
    calctor_t diffc;

    calc_ctor (&exprc);
    calc_ctor (&diffc);

    const tree_t* expr_treep = calc_get_treep (&exprc);
    const tree_t* diff_treep = calc_get_treep (&diffc);

    SetData (expr, &exprc);
    GetTree ();

    calc_diff (&exprc, &diffc, 0);

    /* max size of dump str */
    static const int MAX_DUMP_LEN_ = 32768;

    /* allocating memory for dump buffer */
    char* buff = (char* )calloc (sizeof(char), MAX_DUMP_LEN_);

    if (buff == nullptr)
    {
        fprintf (stderr, "\n\n" "CAN'T ALLOC IN JOKE FUNC!" "\n\n");
        free (expr);
        return;
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info for latex */
    buff_shift += sprintf (buff + buff_shift, "\\documentclass[11pt,a4paper,oneside]{article}" "\n"
                                              "\\usepackage [utf8x] {inputenc}" "\n"
                                              "\\usepackage[T2A] {fontenc}" "\n"
                                              "\\usepackage[english, russian]{babel}" "\n"
                                              "\\usepackage{hyperref}" "\n\n"

                                              "\\usepackage[left=12mm, top=12mm, right=12mm, bottom=28mm, nohead, footskip=10mm]{geometry}" "\n"
                                              "\\usepackage{amsmath}" "\n\n"

                                              "\\usepackage{tikz}" "\n"
                                              "\\usetikzlibrary{calc}" "\n"
                                              "\\usetikzlibrary{decorations.pathmorphing}" "\n\n"

    #define DEF_OP( name, code, oper_info, priority, calca, latex_todo, latex ) \
    "\\newcommand{"#latex"}[2]{"#latex_todo"}\n"
    #include "DSL.h"
    
    "\n" "\\begin{document}" "\n" "%s" "\n" "\\noindentИзначальное уравнение имеет вид: \\\\[0.1cm]" "\n"
    "\\begin{equation}\n", TITLE_);

    #undef DEF_OP

    int numof_dumped_nodes_ = 0;
    buff_shift += calc_sub_t_latex (expr_treep, expr_treep->root_, buff + buff_shift, &numof_dumped_nodes_);
    buff_shift += sprintf (buff + buff_shift, "\\end{equation}" "\n" "Очевидно, что его производная имеет вид: \\\\[0.1cm]" "\n" "\\begin{equation}" "\n");

    numof_dumped_nodes_ = 0;
    buff_shift += calc_sub_t_latex (diff_treep, diff_treep->root_, buff + buff_shift, &numof_dumped_nodes_);
    buff_shift += sprintf (buff + buff_shift, "\n" "\\end{equation}" "\n" "Теперь упростим полученное выражение: \\\\[0.3cm]" "\n" );


    joke_info info = { diff_treep, buff, buff_shift };

    for (;;)
    {
        diffc.simplify_flag = false;

        calc_neutral_fold (&diffc, &info);

        calc_const_fold   (&diffc, &info);

        if (!diffc.simplify_flag)
            break;
    }

    buff_shift = info.buff_shift;

    buff_shift += sprintf(buff + buff_shift, "\n\n" "\\newpage" "\n\n" "%s" "\n" "\\end{document}", BIBLIOGRAPHY_);

    calc_dstr (&diffc);
    calc_dstr (&exprc);

    /* creating file */
    FILE* out_file = fopen ("latex.tex", "wb");
    if (out_file == nullptr)
    {
        free (buff);
        free (expr);
        return;
    }

    /* writing tree info and free memory */
    fwrite (buff, sizeof (char), buff_shift, out_file);
    fclose (out_file);
    free (buff);
    free (expr);

    return;
}