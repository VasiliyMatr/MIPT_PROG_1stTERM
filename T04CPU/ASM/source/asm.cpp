
#include "asm.h"

#define CRITICAL_ERR( COND_, TO_RET_, TO_FREE1_, TO_FREE2_ )    \
if (COND_)                                                      \
{                                                               \
    if (TO_FREE1_ != nullptr)                                   \
        free (TO_FREE1_);                                       \
                                                                \
    if (TO_FREE2_ != nullptr)                                   \
        free (TO_FREE2_);                                       \
                                                                \
                                                                \
    printf ("%ld" "\n", tok_id);                                \
    return TO_RET_;                                             \
}

err_t run( AsmData_t* asmData )
{
    err_t error = tokenize (&(asmData->tokens_), &(asmData->tok_num_), asmData->in_name_);
    if (error != OK) return error;

    error = assemble (asmData);

    return error;
}

err_t assemble( AsmData_t* data )
{
    int* binCode = (int*) calloc (sizeof (int) * 4, data->tok_num_);
    if (binCode == nullptr) return MEM_ERR;

    Token_t tok;
    size_t  binShift = 0;
    size_t tok_id = 0;

    err_t error = firstPass (data);
    CRITICAL_ERR (error != OK, error, binCode, nullptr);

    for (; tok_id < data->tok_num_; tok_id++)
    {
        tok = data->tokens_[tok_id];

        switch (tok.type)
        {
            case NUM_:
                free (binCode);
                return SYNT_ERR;

            case CMD_:
                switch (tok.id)
                {
                    #define DEF_CMD( NAME_, STR_, CMD_ID_, CMD_CPU_, CMD_ASM_ ) \
                    case CMD_ID_:                                               \
                        CMD_ASM_;                                               \
                        break;

                    #include "DSL_CMD.h"
                    #undef DEF_CMD
                }

                break;

            case TERM_:
                break;

            case IDENT_:
               if (data->tokens_[++tok_id].type   == TERM_ &&
                  (data->tokens_[tok_id  ].id     == TR_PROC_ ||
                   data->tokens_[tok_id  ].id     == TR_LABL_))
                   break;

                free (binCode);
                return SYNT_ERR;
        }

    }

    FILE* out_file = fopen (data->out_name_, "wb");
    
    CRITICAL_ERR (out_file == nullptr, FILE_ERR, binCode, nullptr);

    if (fwrite (binCode, sizeof (int), binShift, out_file) != binShift)
    {
        fclose (out_file);
        free (binCode);

        return FILE_ERR;
    }

    fclose (out_file);
    free (binCode);
    free (data->tokens_);

    return OK;
}

err_t firstPass( AsmData_t* data )
{
    size_t tok_id   = 0;
    size_t label_id = 0;
    size_t proc_id  = 0;

    size_t offset = 0;

    bool in_proc_flag = false;

    Token_t tok;
    Token_t prev_tok;

    for (; tok_id < data->tok_num_; tok_id++)
    {
        tok = (data->tokens_)[tok_id];

        switch (tok.type)
        {

        case TERM_:

            switch (tok.id)
            {

            case TR_LABL_:

                if (label_id >= NUMOF_LABELS_)
                    return SYNT_ERR;

                if (tok_id == 0 || (prev_tok = (data->tokens_)[tok_id - 1]).type != IDENT_)
                    return SYNT_ERR;

                if (getLabel(data->labels_, prev_tok.name) >= 0)
                    return SYNT_ERR;

                memcpy(&((data->labels_)[label_id]), &(prev_tok.name), NAME_SIZE_);
                (data->labels_)[label_id++].offset = offset;

                break;

            case TR_PROC_:

                if (in_proc_flag)
                    return SYNT_ERR;

                if (proc_id >= NUMOF_PROCES_)
                    return SYNT_ERR;

                if (tok_id == 0 || (prev_tok = (data->tokens_)[tok_id - 1]).type != IDENT_)
                    return SYNT_ERR;

                if (getProc (data->procs_, prev_tok.name) >= 0)
                    return SYNT_ERR;

                memcpy(&((data->procs_)[proc_id]), &(prev_tok.name), NAME_SIZE_);
                (data->procs_)[proc_id++].offset = offset;

                in_proc_flag = true;

                break;

            case TR_ENDP_:

                if (!in_proc_flag)
                    return SYNT_ERR;

                (data->procs_)[proc_id - 1].end = offset;

                in_proc_flag = false;

                break;

            }

            break;

        case CMD_:

            offset ++;

            if (tok.name[0] == 'J')
                offset++;

            else if (tok.id == CMD_CALL_ || tok.id == CMD_MOV_ ||
                     tok.id == CMD_IN_   || tok.id == CMD_OUT_ ||
                     tok.id == CMD_POP_  || tok.id == CMD_PUSH_)
                offset++;
                
            break;

        case NUM_:

            offset++;
            break;
        }
    }

    if (in_proc_flag) return SYNT_ERR;

    return OK;
}

int getLabel( Label_t labels[NUMOF_LABELS_], char name[NAME_SIZE_] )
{
    for (size_t label_id = 0; label_id < NUMOF_LABELS_; label_id++)
    {
        if (labels[label_id].name[0] == '\0')
            break;

        if (!strcmp (name, labels[label_id].name))
            return label_id;
    }

    return -1;
}

int getProc( Proc_t proces[NUMOF_PROCES_], char name[NAME_SIZE_] )
{
    for (size_t proc_id = 0; proc_id < NUMOF_PROCES_; proc_id++)
    {
        if (proces[proc_id].name[0] == '\0')
            break;

        if (!strcmp (name, proces[proc_id].name))
            return proc_id;
    }

    return -1;
}

/* to parce complicated arguments, such as [ R00 + 10000 ] */
unsigned int prcArg( Token_t* toks, size_t* toks_shift, unsigned int* arg_const )
{
    size_t toks_local_shift = 0;
    Token_t tok = toks[toks_local_shift];
    unsigned int CMD_MODIFIER = 0;

    if (tok.id == TR_L_SQ_BR_)
    {
        toks_local_shift++;

        CMD_MODIFIER = getRegNCnst (toks + 1, &toks_local_shift );
        if (CMD_MODIFIER == 0) return 0;

        tok = toks[toks_local_shift];

        if (CMD_MODIFIER & FT_CONST_ON_)
        {
            *arg_const = tok.number;
            tok = toks[++toks_local_shift];
        }

        if (tok.type != TERM_ || tok.id != TR_R_SQ_BR_) return 0;

        CMD_MODIFIER = CMD_MODIFIER | FT_MEM_ON_;
        toks_local_shift++;
    }

    else
    {
        CMD_MODIFIER = getRegNCnst (toks, &toks_local_shift);
        if (CMD_MODIFIER == 0) return 0;

        if (CMD_MODIFIER & FT_CONST_ON_)
        {
            *arg_const = toks[toks_local_shift].number;
            toks_local_shift++;
        }
    }

    *toks_shift += toks_local_shift - 1;
    return CMD_MODIFIER;
}

/* To parce next types of tok sequences: REG; REG + CONST; REG - CONST; CONST */
unsigned int getRegNCnst( Token_t* toks, size_t* toks_shift )
{
    size_t toks_local_shift = 0;
    Token_t tok = toks[toks_local_shift++];
    unsigned int CMD_MODIFIER = 0;

    if (tok.type == TERM_ && tok.name[0] == 'R')
    {
        CMD_MODIFIER = FT_REG_ON_ | (tok.id - TR_R00_);
        tok = toks[toks_local_shift++];

        if (tok.type == TERM_ && tok.id == TR_PLUS_)
        {
            CMD_MODIFIER = CMD_MODIFIER | FT_SIGN_PLUS_ | FT_CONST_ON_;
            tok = toks[toks_local_shift++];

            if (tok.type != NUM_) return 0;
        }

        else if (tok.type == TERM_ && tok.id == TR_MINUS_)
        {
            CMD_MODIFIER = CMD_MODIFIER | FT_CONST_ON_;
            tok = toks[toks_local_shift++];

            if (tok.type != NUM_) return 0;
        }
    }

    else if (tok.type == NUM_) CMD_MODIFIER = FT_CONST_ON_;

    *toks_shift += toks_local_shift - 1;

    return CMD_MODIFIER;
}