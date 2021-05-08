// DEF_CMD( NAME_, STR_, CMD_ID_, CMD_CPU_, CMD_ASM_ )

#define BIN_OP( NAME_, STR_, CMD_ID_, OPR_ )                            \
DEF_CMD (NAME_, STR_, CMD_ID_,                                          \
{                                                                       \
                                                                        \
    int modifier = CPU.code_[++RIP_];                                   \
                                                                        \
    if (modifier & 2)                                                   \
        CPU.Regs_[(modifier >> 16) & 0xFF] OPR_##= CPU.code_[++RIP_];   \
    else                                                                \
        CPU.Regs_[(modifier >> 16) & 0xFF] OPR_##=                      \
        CPU.Regs_[(modifier >> 8) & 0xFF];                              \
                                                                        \
},                                                                      \
{                                                                       \
    Token_t ft_tok = (data->tokens_)[++tok_id];                         \
    Token_t sd_tok = (data->tokens_)[++tok_id];                         \
                                                                        \
    CRITICAL_ERR (ft_tok.type != TERM_ || ft_tok.name[0] != 'R',        \
                  SYNT_ERR, binCode, nullptr);                          \
                                                                        \
    CRITICAL_ERR (sd_tok.type != NUM_  &&                               \
                 (sd_tok.type != TERM_ || sd_tok.name[0] != 'R'),       \
                  SYNT_ERR, binCode, nullptr)                           \
                                                                        \
    unsigned int props =                                                \
    ((sd_tok.type == TERM_) << 1) | (sd_tok.type == NUM_) |             \
    ((ft_tok.id - TR_R00_) << 16) | ((sd_tok.id - TR_R00_) << 8);       \
                                                                        \
    CRITICAL_ERR (props & 0x5 == 0 || props & 0xA == 0,                 \
                  SYNT_ERR, binCode, nullptr);                          \
                                                                        \
    binCode[binShift++] = CMD_ID_;                                      \
    binCode[binShift++] = props;                                        \
                                                                        \
    if (props & 0x1)                                                    \
        binCode[binShift++] = sd_tok.number;                            \
})

    BIN_OP  (ADD    , ADD   , 0x00, +)
    BIN_OP  (SUB    , SUB   , 0x01, -)
    BIN_OP  (MUL    , MUL   , 0x02, *)
    BIN_OP  (DIV    , DIV   , 0x03, /)

#undef BIN_OP

#define STACK_BIN_OP( NAME_, STR_, CMD_ID_, OPR_ )                      \
DEF_CMD (NAME_, STR_, CMD_ID_,                                          \
{                                                                       \
                                                                        \
    int first_val  = CPU.stk_.pop (&error);                             \
    int second_val = CPU.stk_.pop (&error);                             \
                                                                        \
    CPU.stk_.push (second_val OPR_ first_val, &error);                  \
                                                                        \
},                                                                      \
{                                                                       \
    binCode[binShift++] = CMD_ID_;                                      \
})
    
    STACK_BIN_OP (ADDS, ADDS, 0x50, +)
    STACK_BIN_OP (SUBS, SUBS, 0x51, -)
    STACK_BIN_OP (MULS, MULS, 0x52, *)
    STACK_BIN_OP (DIVS, DIVS, 0x53, /)

#undef STACK_BIN_OP

DEF_CMD (PUSH   , PUSH  , 0x04,
{
    if (!CPU.code_[++RIP_])
        CPU.stk_.push (CPU.code_ [++RIP_], &error);
    else
        CPU.stk_.push (CPU.Regs_ [CPU.code_ [RIP_] >> 16], &error);
},
{
    tok = (data->tokens_) [++tok_id];
    binCode[binShift++] = CMD_PUSH_;

    if (tok.type == NUM_)
    {
        binCode[binShift++] = 0;
        binCode[binShift++] = tok.number;
    }

    else if (tok.type == TERM_ && tok.name[0] == 'R')
    {   
        binCode[binShift++] = 1 + ((tok.id - TR_R00_) << 16);
    }

    else
        CRITICAL_ERR (true, SYNT_ERR, binCode, nullptr);
})

DEF_CMD (POP    , POP   , 0x06,
{
    int regId = CPU.code_ [++RIP_];

    if (regId < REG_NUM_)
        CPU.Regs_ [regId] = CPU.stk_.pop (&error);
    else
        CPU.stk_.pop (&error);
},
{
    tok = (data->tokens_)[tok_id + 1];

    binCode[binShift++] = CMD_POP_;

    if (tok.type == TERM_ && tok.name[0] == 'R')
    {
        binCode[binShift++] = tok.id - TR_R00_;
        ++tok_id;
    }

    else 
        binCode[binShift++] = TR_R07_ + 1;

})

DEF_CMD (CMPS    , CMPS   , 0x07,
{
    int first  = CPU.stk_.pop (&error);
    int second = CPU.stk_.pop (&error);

    CPU.EqualFlag_ = first == second;
    CPU.AboveFlag_ = first > second;
    CPU.LessFlag_  = first < second;
},
{
    binCode[binShift++] = CMD_CMPS_;
})

DEF_CMD (CALL   , CALL  , 0x08,
{
    size_t newRip = CPU.code_[++RIP_];

    if (newRip > CPU.code_size_)
        return SEG_FAULT;

    CPU.stk_.push (++RIP_, &error);

    RIP_ = newRip - 1;
},
{
    binCode[binShift++] = CMD_CALL_;
    int proc_id = -1;

    tok = data->tokens_[++tok_id];
    if (tok.type != IDENT_ || ((proc_id = getProc (data->procs_, tok.name)) < 0))
    {
        printf ("%ld" "\n", tok_id);
        free (binCode); 
        return SYNT_ERR;
    }

    binCode[binShift++] = data->procs_[proc_id].offset;
})

DEF_CMD (EXIT   , EXIT  , 0x09,
{
    return OK;
},
{
    binCode[binShift++] = CMD_EXIT_;
})

#define JMP_OP( NAME_, STR_, CMD_ID_, COND_ )                                       \
DEF_CMD (NAME_, STR_, CMD_ID_,                                                      \
{                                                                                   \
    if (COND_)                                                                      \
    {                                                                               \
        CPU.AboveFlag_ = 0;                                                         \
        CPU.LessFlag_ = 0;                                                          \
        CPU.EqualFlag_ = 0;                                                         \
                                                                                    \
        size_t newRip = CPU.code_[++RIP_];                                          \
                                                                                    \
        if (newRip > CPU.code_size_)                                                \
            return SEG_FAULT;                                                       \
                                                                                    \
        RIP_ = newRip - 1;                                                          \
    }                                                                               \
                                                                                    \
    else                                                                            \
        RIP_++;                                                                     \
},                                                                                  \
{                                                                                   \
    binCode[binShift++] = CMD_ID_;                                                  \
    int label_id = -1;                                                              \
                                                                                    \
    tok = data->tokens_[++tok_id];                                                  \
    if (tok.type != IDENT_ || (label_id = getLabel (data->labels_, tok.name)) < 0)  \
    {                                                                               \
        printf ("%ld" "\n", tok_id);                                                \
        free (binCode);                                                             \
        return SYNT_ERR;                                                            \
    }                                                                               \
                                                                                    \
    binCode[binShift++] = data->labels_[label_id].offset;                           \
})

    JMP_OP (JE  , JE    , 0x0A,  CPU.EqualFlag_                     )
    JMP_OP (JNE , JNE   , 0x0B, !CPU.EqualFlag_                     )
    JMP_OP (JAE , JAE   , 0x0C,  CPU.EqualFlag_ || CPU.AboveFlag_   )
    JMP_OP (JLE , JLE   , 0x0D,  CPU.EqualFlag_ || CPU.LessFlag_    )
    JMP_OP (JA  , JA    , 0x0E,  CPU.AboveFlag_                     )
    JMP_OP (JL  , JL    , 0x0F,  CPU.LessFlag_                      )
    JMP_OP (JMP , JMP   , 0x10,  1                                  )

#undef JMP_OP

DEF_CMD (MOV, MOV, 0xFF,
{
    int value = 0;
    int props = CPU.code_[++RIP_];

    if (props & SD_REG_ON_)
        value += CPU.Regs_[props & 0xF];

    if (props & SD_CONST_ON_)
        value += CPU.code_[RIP_ + 1 + (props & FT_CONST_ON_ ? 1 : 0)];

    if (props & SD_MEM_ON_)
    {
        if (value > sizeof (CPU.RAM_) / sizeof (int))
            return SEG_FAULT;

        value = CPU.RAM_[value];
    }

    if (props & FT_MEM_ON_)
    {
        int dest_addr = 0;

        if (props & FT_REG_ON_)
            dest_addr += CPU.Regs_[(props >> 4) & 0xF];

        if (props & FT_CONST_ON_)
            dest_addr += CPU.code_[RIP_ + 1];

        if (dest_addr > sizeof(CPU.RAM_) / sizeof(int))
            return SEG_FAULT;

        CPU.RAM_[dest_addr] = value;
    }

    else
        CPU.Regs_[(props >> 4) & 0xF] = value;

    RIP_ += (bool)(props & FT_CONST_ON_) + (bool)(props & SD_CONST_ON_);
},
{
    unsigned int ft_number = 0;
    unsigned int sd_number = 0;
    unsigned int prop_mask = 0;
    unsigned int second_props = 0;

    prop_mask = prcArg (data->tokens_ + (++tok_id), &tok_id, &ft_number);
    CRITICAL_ERR (prop_mask == 0, SYNT_ERR, binCode, nullptr);

    if (prop_mask & FT_REG_ON_)
        prop_mask = ((prop_mask >> 4) << 4) | ((prop_mask & 0xFF) << 4);

    second_props = prcArg (data->tokens_ + (++tok_id), &tok_id, &sd_number);
    CRITICAL_ERR (second_props == 0, SYNT_ERR, binCode, nullptr);

    prop_mask = (second_props & 0xFF) | ((second_props >> 8) << 7) | prop_mask;

    binCode[binShift++] = CMD_MOV_;
    binCode[binShift++] = prop_mask;

    if (prop_mask & FT_CONST_ON_)
        binCode[binShift++] = ft_number;

    if (prop_mask & SD_CONST_ON_)
        binCode[binShift++] = sd_number;
})

DEF_CMD (IN, IN, 0xEC,
{
    int regId = CPU.code_ [++RIP_];

    scanf ("%d", CPU.Regs_ + regId);
},
{
    tok = data->tokens_[++tok_id];

    CRITICAL_ERR (tok.type != TERM_ || tok.name[0] != 'R', SYNT_ERR, binCode, nullptr);

    binCode[binShift++] = CMD_IN_;
    binCode[binShift++] = tok.id - TR_R00_;
})

DEF_CMD (OUT, OUT, 0xED,
{
    int regId = CPU.code_ [++RIP_];

    printf ("%d" "\n", CPU.Regs_ [regId]);
},
{
    tok = data->tokens_[++tok_id];

    CRITICAL_ERR (tok.type != TERM_ || tok.name[0] != 'R', SYNT_ERR, binCode, nullptr);

    binCode[binShift++] = CMD_OUT_;
    binCode[binShift++] = tok.id - TR_R00_;
})

DEF_CMD (POPA, POPA, 0xEF,
{
    for (int regId = REG_NUM_ - 10; regId >= 0 ; --regId)
        CPU.Regs_[regId] = CPU.stk_.pop (&error);
},
{
    binCode[binShift++] = CMD_POPA_;
})

DEF_CMD (PUSHA, PUSHA, 0xF0,
{
    for (int regId = 0; regId < REG_NUM_ - 9; ++regId)
        CPU.stk_.push (CPU.Regs_[regId], &error);
},
{
    binCode[binShift++] = CMD_PUSHA_;
})

DEF_CMD (RET, RET, 0xF1,
{
    size_t newRip = CPU.stk_.pop (&error);

    if (newRip > CPU.code_size_)
        return SEG_FAULT;

    RIP_ = newRip - 1;
},
{
    binCode[binShift++] = CMD_RET_; 
})