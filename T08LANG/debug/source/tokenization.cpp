
#include "compiler.hpp"

/* max format str size */
static const size_t INPUT_FORMAT_MAX_LEN_ = 64;

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
      { "%1[=>{}()|]"       , "%15[=>{}()-]%n"     } ,
      /* for inbuild     keywords     */
      { "%1[A-Za-z_]"      , "%15[A-Za-z0-9_]%n"   } ,
      { "%1[+/=|&><*^-]"   , "%15[|&=]%n"          }   /* one symbol key words should be checked after all */
};

const int INPUT_FORMATS_NUM_ = sizeof (INPUT_FORMATS_) / (2 * INPUT_FORMAT_MAX_LEN_);

/* input formats for checks */
const int INPUT_FORMAT_IDENT_ = 0;

/* define to free memory and return needed values in case of errors */
#define CRITICAL_ERR( COND_, TO_RET_, TO_DEL_ )  \
if (COND_)                                       \
{                                                \
    if (TO_DEL_ != nullptr)                      \
        free (TO_DEL_);                          \
                                                 \
    cmplp->code_buff_  = code;                   \
    cmplp->error_info_ = { TO_RET_, glob_shift };\
                                                 \
    return TO_RET_;                              \
}

Err_t tokenize( Compiler* cmplp, const char* const code_file_name )
{
    /* code buff */
    char* code = nullptr; 
    /* global buff shift */
    int glob_shift = 0;
    /* curr token buff shift */
    int loc_shift  = 0;

    CRITICAL_ERR (isBadPtr  (code_file_name), BAD_PTR_ , nullptr);
    CRITICAL_ERR (isBadPtr  (cmplp)         , BAD_PTR_ , nullptr);
    FILE* code_file = fopen (code_file_name, "rb");
    CRITICAL_ERR (code_file == nullptr      , FILE_ERR_, nullptr);

    /* reading file */
        fseek (code_file, 0, SEEK_END);
        int num_of_chars = ftell (code_file) + 1;
        rewind (code_file);

        code = (char* )calloc (sizeof (char), num_of_chars + 1);
        if (code == nullptr)
        {
            fclose (code_file);
            return MEM_ERR_;
        }

        fread  (code, sizeof (char), num_of_chars, code_file);
        fclose (code_file);

    /* allocating memory for tokens */
        Token_t* tokens = (Token_t* )calloc (sizeof (Token_t), NUMOF_TOC_);
        CRITICAL_ERR (tokens == nullptr, MEM_ERR_, nullptr);

    /* fill all tokens with POISON_ */
        for (int newToksId = 0; newToksId < NUMOF_TOC_; newToksId++)
            tokens[newToksId] = POISON_;

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
                CRITICAL_ERR (reallp == nullptr, MEM_ERR_, tokens);

            /* new ptr assignation */
                tokens = reallp;

            /* fill all new tokens data with POISON_ */
                for (int newToksId = tok_id; newToksId < numof_toks; newToksId++)
                    tokens[newToksId] = POISON_;
        }

        /* skipping spaces */
            sscanf (code + glob_shift, " %n", &loc_shift);
            glob_shift += loc_shift;
            loc_shift = 0;

            tokens[tok_id].location = glob_shift;

        for (int format_id = 0; format_id < INPUT_FORMATS_NUM_; format_id++)
        {
            /* trying to get symbol sequence, consisting of formats symbols */
            loc_shift = getSequence (tokens[tok_id].data.name, code + glob_shift, format_id);

            /* sequence get success! */
            if (loc_shift != 0)
            {
                /* in-build sequences found (terminal sequences or keywords) ?*/
                switch (tokens[tok_id].data.name[0])
                {
                    #define DEF_TR( NAME_, CODE_FT_PT_, CODE_SD_PT_ )               \
                    case CODE_FT_PT_:                                               \
                        if (!nameCmpr (tokens[tok_id].data.name, TR_##NAME_))       \
                        {                                                           \
                            tokens[tok_id].type = TERM_;                            \
                            glob_shift += loc_shift;                                \
                            break;                                                  \
                        }
                    #include "DSL_TR.h"
                    #undef DEF_TR

                    #define DEF_KEY( NAME_, CODE_FT_PT_, CODE_SD_PT_, OPRS_INFO_, ASM_NAME_ )   \
                    case CODE_FT_PT_:                                                           \
                        if (!nameCmpr (tokens[tok_id].data.name, KEY_##NAME_))                  \
                        {                                                                       \
                            tokens[tok_id].type = KEY_;                                         \
                            glob_shift += loc_shift;                                            \
                            break;                                                              \
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
                        CRITICAL_ERR (format_id == INPUT_FORMATS_NUM_ - 1, SYNT_ERR_, tokens);
                }

                /* parsed token success */
                if (tokens[tok_id].type != UNDF_T_)
                    break;
            }
        }

        /* token parsed => try to get next */
            if (tokens[tok_id].type != UNDF_T_)
                continue;

        /* trying to get number */
            if (sscanf (code + glob_shift, " %d%n ", &(tokens[tok_id].data.number), &loc_shift) > 0)
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
                CRITICAL_ERR (code[glob_shift] != '\0', SYNT_ERR_, tokens);

                /* all is good, tokenization complete */
                cmplp->tokens_ = tokens;
                cmplp->code_buff_ = code;
                return OK_;
            }
    }

    return OK_;
}

#undef CRITICAL_ERR

int getSequence( NamePart_t name[NAME_LEN_], const char* code, const int input_formats_id )
{
    assert (input_formats_id < INPUT_FORMATS_NUM_);

    int     symb_skip = 0;

    if (sscanf (code, INPUT_FORMATS_[input_formats_id][0], name) <= 0)
        return 0;

    sscanf (code + 1, INPUT_FORMATS_[input_formats_id][1], (long long)name + 1, &symb_skip);

    return symb_skip + 1;
}