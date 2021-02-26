
/* FUNCS FOR tree_t */

    /* out format for nedded tree labels and proper coloring */
    const char const COMMON_FORMAT_[] = { "%c" "\\n" "%.*s\", " "fillcolor=\"%s" };
    /* colors */
    const char const COLORS_  [][100] = { "blue", "yellow", "red" };

    /* colors ids */
    const size_t BLU_COL_ = 0;
    const size_t YEL_COL_ = 1;
    const size_t RED_COL_ = 2;

    err_t get_data_str( const data_t data, char* const buff )
    {
        const char* color = nullptr;

        switch (data.type)
        {
            /* printing number */
            case NUM_:
            {
                sprintf (buff, "%lf", data.info.number);
                return OK;
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
                return OK;
            }
        }

        if (color != nullptr)
            sprintf (buff, COMMON_FORMAT_, data.type, DESCR_LEN_, data.info.name, color);

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

/* compiler FUNCS */

tree_t* cmpl_get_treep( const Compiler* const cmpl )
{
    /* check calctor.h for calc_TR_SHT_ constand definition*/
    return (tree_t* )((size_t)cmpl + CMPL_TR_SHT_);
}

err_t cmpl_ctor( Compiler* const cmpl )
{
    err_t error = OK;
    /* just constructing data tree */
    tree_ctor (cmpl_get_treep (cmpl), &error);

    return error;
}

err_t cmpl_dstr( Compiler* const cmpl )
{
    err_t error = OK;
    /* just destructing data tree */
    tree_dstr (cmpl_get_treep (cmpl), &error);

    return error;
}

err_t cmpl_verify( const Compiler* const cmpl )
{
    return OK;
}

/* max format str size */
const int INPUT_FORMAT_MAX_LEN_ = 100;

/* choose correct sequences of symbols
 * first  format string in pair is for first symbol (usually first symbol have diffirent rules)
 * second format string in pair is for all next symbols
 */
const char INPUT_FORMATS_[][2][INPUT_FORMAT_MAX_LEN_] = {
      /* for non-inbuild identifiers
       * SHOULD CHECK AT FIRST, BECAUSE THEN,
       * IF THERE ARE NO MATCHES IN CASES, ERROR CAN BE EASELY FOUND
       */
      { "%1[A-Za-z_]"      , "%15[A-Za-z0-9_]%n"   } ,
      /* for terminal    sequences    */
      { "%1[=>{}()]"       , "%15[=>{}()]%n"       } ,
      /* for inbuild     keywords     */
      { "%1[A-Za-z_]"      , "%15[A-Za-z0-9_]%n"   } ,
      { "%1[+/*=|&><^-]"   , "%15[|&=]%n"          }   /* one symbol key words should be checked after all */
};

const int INPUT_FORMATS_NUM_ = sizeof (INPUT_FORMATS_) / (2 * INPUT_FORMAT_MAX_LEN_);

/* input formats for checks */
const int INPUT_FORMAT_IDENT_ = 0;

err_t cmpl_tokinize( Compiler* const cmpl, const char* const code_file_name )
{
    ERR_OUT (isBadPtr (code_file_name), "BAD PTR TO CODE FILE!", BAD_PTR);

    FILE* code_file = fopen (code_file_name, "rb");
    ERR_OUT (code_file == nullptr, "CAN'T OPEN FILE WITH CODE!", FILE_ERR);

    /* reading file */
    fseek (code_file, 0, SEEK_END);
    int num_of_chars = ftell (code_file) + 1;
    rewind (code_file);

    char* code = (char* )calloc (sizeof (char), num_of_chars);
    ERR_OUT (code == nullptr, "CAN'T ALLOC IN TOK FUNC!", MEM_ERR);

    fread (code, sizeof (char), num_of_chars, code_file);
    fclose (code_file);

    /* verifications and error checks */
    verify (cmpl, error);
    ERR_OUT (isBadPtr(code), "BAD PTR", BAD_PTR);
    ERR_OUT (cmpl->tokens_ != nullptr, "NOT NULL PTR", BAD_PTR);

    /* allocating memory for tokens */
    Token_t* tokens = (Token_t* )calloc (sizeof (Token_t), NUMOF_TOC_);
    ERR_OUT (tokens == nullptr, "ALLOC ERROR", MEM_ERR);

    /* global buff shift */
    int glob_shift = 0;
    /* curr token buff shift */
    int loc_shift  = 0;

    /* for reallocations */
    int numof_toks = NUMOF_TOC_;

    /* running trought all found tokens until EOF found */
    for (int tok_id = 0;; tok_id++)
    {
        /* number of tokens check, if there are to many tokens => reallocating */
        if (tok_id >= numof_toks)
        {
            numof_toks *= 2;
            Token_t* reallp = (Token_t* )realloc (tokens, sizeof (Token_t) * numof_toks);
            if (reallp == nullptr)
            {
                free (tokens);
                return MEM_ERR;
            }

            tokens = reallp;
        }

        /* skipping spaces */
            sscanf (code + glob_shift, " %n", &loc_shift);
            glob_shift += loc_shift;
            loc_shift = 0;

        /* to check for changes later */
        tokens[tok_id].type = UNDF_T_;

        for (int format_id = 0; format_id < INPUT_FORMATS_NUM_; format_id++)
        {

            /* trying to get symbol sequence, consisting of formats symbols */
            loc_shift = cmpl_get_sequence (tokens[tok_id].info.name, code + glob_shift, format_id);
            /* sequence getted */
            if (loc_shift != 0)
            {
                /* in-build sequences found (terminal sequences or keywords) ?*/
                switch (tokens[tok_id].info.name[0])
                {
                    #define DEF_TR( NAME, code_ft, code_sd )            \
                    case code_ft:                                       \
                        if (code_sd == tokens[tok_id].info.name[1])     \
                        {                                               \
                            tokens[tok_id].type = TERM_;                \
                            glob_shift += loc_shift;                    \
                            break;                                      \
                        }
                    #include "DSL_TR.h"
                    #undef DEF_TR

                    #define DEF_KEY( NAME, code_ft, code_sd, operands ) \
                    case code_ft:                                       \
                        if (code_sd == tokens[tok_id].info.name[1])     \
                        {                                               \
                            tokens[tok_id].type = KEY_;                 \
                            glob_shift += loc_shift;                    \
                            break;                                      \
                        }
                    #include "DSL_KEY.h"
                    #undef DEF_KEY
                    /* known sequences not found */
                    default:
                        /* identifier format used => all is ok */
                        if (format_id == INPUT_FORMAT_IDENT_)
                        {
                            tokens[tok_id].type = IDENT_;
                            glob_shift += loc_shift;
                            break;
                        }

                        /* last format was checked and nothing is found => tokinization error */
                        else if (format_id == INPUT_FORMATS_NUM_ - 1)
                        {
                            free (tokens);
                            return SYNT_ERR;
                        }
                }

                /* parsed token */
                if (tokens[tok_id].type != UNDF_T_)
                    break;
            }
        }

        if (tokens[tok_id].type != UNDF_T_)
            continue;

        /* trying to get number */
        loc_shift = cmpl_get_num (&(tokens[tok_id].info.number), code + glob_shift);
        if (loc_shift != 0)
        {
            glob_shift += loc_shift;
            tokens[tok_id].type = NUM_;
            continue;
        }

        /* can't parse token */
        if (tokens[tok_id].type == UNDF_T_)
        {
            /* skipping spaces */
            sscanf (code + glob_shift, " %n", &loc_shift);
            glob_shift += loc_shift;

            /* not EOF => not OK */
            if (code[glob_shift] != '\0')
                return SYNT_ERR;
            /* all is good, tokinization complete */
            else
            {
                cmpl->tokens_ = tokens;
                return OK;
            }
        }
    }

    return OK;
}

int cmpl_get_num( double* number, const char* const code )
{
    int symb_skip = 0;

    sscanf (code, " %lf%n ", number, &symb_skip);
    
    return symb_skip;
}

int cmpl_get_sequence( NamePart_t name[NAME_LEN_], const char* code, const int input_formats_id )
{
    assert (input_formats_id < INPUT_FORMATS_NUM_);

    int     symb_skip = 0;

    if (sscanf (code, INPUT_FORMATS_[input_formats_id][0], name) <= 0)
        return 0;

    sscanf (code + 1, INPUT_FORMATS_[input_formats_id][1], (int)name + 1, &symb_skip);

    return symb_skip + 1;
}

int cmpl_name_cmpr( const NamePart_t const ft_name[NAME_LEN_],
                    const NamePart_t const sd_name[NAME_LEN_] )
{
    for (int names_part_id = 0; names_part_id < NAME_LEN_; names_part_id++)
    {
        if (ft_name[names_part_id] != sd_name[names_part_id])
            return ft_name[names_part_id] - sd_name[names_part_id];
    }

    return 0;
}

err_t cmpl_make_code( node_t* root, char* buff )
{
    size_t pose = 0;
    node_t* vars = nullptr;

    while (root != nullptr)
    {
        pose += sprintf (buff + pose, "%.*s (", NAME_SIZE_, root->data.info.name);
        vars = root->left_ch;

        while (vars != nullptr)
        {
            pose += sprintf (buff + pose, " %.*s ", NAME_SIZE_, vars->data.info.name);
            vars = vars->left_ch;
        }


    }

    return OK;
}

int   cmpl_rec_code( node_t* root, char* buff)
{
    Token_t tok = root->data;
    size_t  pose = 0;

    switch (tok.type)
    {
        case KEY_:
            switch (tok.info.name[0])
            {
                case KEY_IF_[0]    :
                case KEY_WHILE_[0] :
                    pose += cmpl_rec_code (root->left_ch, buff + pose);
                    pose += sprintf (buff + pose, "\n%.*s\n", NAME_SIZE_, TR_LBRACE2_);
                    pose += cmpl_rec_code (root->righ_ch, buff + pose);
                    pose += sprintf (buff + pose, "\n%.*s\n", NAME_SIZE_, TR_RBRACE2_);

                    return pose;
            }

        case NUM_:
            pose += sprintf (buff + pose, " %lf ", tok.info.number);
            return pose;
    }

    pose += sprintf (buff + pose, " %.*s ", NAME_SIZE_, tok.info.name);

    return pose;
}