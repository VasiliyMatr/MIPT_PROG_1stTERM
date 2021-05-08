
#include "tokenization.h"

/* max format str size */
const size_t INPUT_FORMAT_MAX_LEN_ = 64;

/* choose correct sequences of symbols
 * first  format string in pair is for first symbol (usually first symbol have diffirent rules)
 * second format string in pair is for all next symbols
 */
const char INPUT_FORMATS_[][2][INPUT_FORMAT_MAX_LEN_] = {
      /* for non-inbuild identifiers
       * SHOULD CHECK AT FIRST, BECAUSE THEN,
       * IF THERE ARE NO MATCHES IN CASES, ERROR CAN BE EASELY FOUND
       */
      { "%1[A-Za-z_]"       , "%15[A-Za-z0-9_]%n"   } ,
      /* for terminal    sequences    */
      { "%1[][:PE+R-]"      , "%15[ROCNDP0-9]%n"    } ,
      /* for in-build    cmd names    */
      { "%1[A-Z]"           , "%15[A-Z]%n"          }
};

const int INPUT_FORMATS_NUM_ = sizeof (INPUT_FORMATS_) / (2 * INPUT_FORMAT_MAX_LEN_);

/* input formats for checks */
const int INPUT_FORMAT_IDENT_ = 0;

/* define to free memory and return needed values in case of errors */
#define CRITICAL_ERR( COND_, TO_RET_, TO_DEL1_, TO_DEL2_ )  \
if (COND_)                                                  \
{                                                           \
    if (TO_DEL1_ != nullptr)                                \
        free (TO_DEL1_);                                    \
    if (TO_DEL2_ != nullptr)                                \
        free (TO_DEL2_);                                    \
                                                            \
    return TO_RET_;                                         \
}

const size_t NUMOF_TOC_ = 1024;

err_t tokenize( Token_t** ptr_2_toks_ms, size_t* toks_num, const char* const code_file_name )
{
    CRITICAL_ERR (isBadPtr (toks_num)      , BAD_PTR, nullptr, nullptr);
    CRITICAL_ERR (isBadPtr (code_file_name), BAD_PTR, nullptr, nullptr);
    CRITICAL_ERR (isBadPtr (ptr_2_toks_ms) , BAD_PTR, nullptr, nullptr);
    FILE* code_file = fopen (code_file_name, "rb");
    CRITICAL_ERR (code_file == nullptr, FILE_ERR, nullptr, nullptr);

    /* reading file */
        fseek (code_file, 0, SEEK_END);
        int num_of_chars = ftell (code_file) + 1;
        rewind (code_file);

        char* code = (char* )calloc (sizeof (char), num_of_chars);
        if (code == nullptr)
        {
            fclose (code_file);
            return MEM_ERR;
        }

        fread  (code, sizeof (char), num_of_chars, code_file);
        fclose (code_file);

    /* verifications and error checks */
        CRITICAL_ERR (*ptr_2_toks_ms != nullptr, BAD_PTR, code, nullptr);

    /* allocating memory for tokens */
        Token_t* tokens = (Token_t* )calloc (sizeof (Token_t), NUMOF_TOC_);
        CRITICAL_ERR (tokens == nullptr, MEM_ERR, code, nullptr);

    /* fill all tokens with POISON_ */
        for (int newToksId = 0; newToksId < NUMOF_TOC_; newToksId++)
            tokens[newToksId] = POISON_;

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
            /* reallocation */
                numof_toks *= 2;
                Token_t* reallp = (Token_t* )realloc (tokens, sizeof (Token_t) * numof_toks);
                CRITICAL_ERR (reallp == nullptr, MEM_ERR, code, tokens);

            /* new ptr assignation */
                tokens = reallp;

            /* fill all new tokens info with POISON_ */
                for (int newToksId = tok_id; newToksId < numof_toks; newToksId++)
                    tokens[newToksId] = POISON_;
        }

        /* skipping spaces */
            sscanf (code + glob_shift, " %n", &loc_shift);
            glob_shift += loc_shift;
            loc_shift = 0;

        for (int format_id = 0; format_id < INPUT_FORMATS_NUM_; format_id++)
        {

            /* trying to get symbol sequence, consisting of formats symbols */
            loc_shift = get_sequence (tokens[tok_id].name, code + glob_shift, format_id);

            /* sequence get success! */
            if (loc_shift != 0)
            {
                /* in-build sequences found (terminal sequences or cmd) ?*/
                    if (0) {}
                    #define DEF_TR( NAME_, STR_, TR_ID_ )           \
                    else if (!strcmp (#STR_, tokens[tok_id].name))  \
                    {                                               \
                        tokens[tok_id].type = TERM_;                \
                        tokens[tok_id].id   = TR_ID_;               \
                        glob_shift += loc_shift;                    \
                        break;                                      \
                    }

                    #include "DSL_TR.h"
                    #undef DEF_TR

                    #define DEF_CMD( NAME_, STR_, CMD_ID_, CMD_CPU_, CMD_ASM_ ) \
                    else if (!strcmp (#STR_, tokens[tok_id].name))              \
                    {                                                           \
                        tokens[tok_id].type = CMD_;                             \
                        tokens[tok_id].id   = CMD_ID_;                          \
                        glob_shift += loc_shift;                                \
                        break;                                                  \
                    }

                    #include "DSL_CMD.h"
                    #undef DEF_CMD

                    /* known sequences not found */
                    else if (format_id == INPUT_FORMAT_IDENT_)
                    {
                        /* identifier format used => all is ok */
                            tokens[tok_id].type = IDENT_;
                            glob_shift += loc_shift;
                            break;
                    }

                    /* last format was checked and nothing is found => tokinization error */
                    CRITICAL_ERR (format_id == INPUT_FORMATS_NUM_ - 1, SYNT_ERR, nullptr, code);
            }

            /* parsed token success */
            if (tokens[tok_id].type != UNDF_T_)
                break;
        }

        /* token parsed => try to get next */
        if (tokens[tok_id].type != UNDF_T_)
            continue;

        /* trying to get number */
        if (sscanf (code + glob_shift, " %d%n ", &(tokens[tok_id].number), &loc_shift) > 0)
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
            CRITICAL_ERR (code[glob_shift] != '\0', SYNT_ERR, tokens, code);

            /* all is good, tokinization complete */
            *ptr_2_toks_ms = tokens;
            *toks_num = tok_id + 1;

            return OK;
        }
    }

    return OK;
}

int get_sequence( char* dest, const char* code, const int input_formats_id )
{
    assert (input_formats_id < INPUT_FORMATS_NUM_);

    int     symb_skip = 0;

    if (sscanf (code, INPUT_FORMATS_[input_formats_id][0], dest) <= 0)
        return 0;

    sscanf (code + 1, INPUT_FORMATS_[input_formats_id][1], dest + 1, &symb_skip);

    return symb_skip + 1;
}