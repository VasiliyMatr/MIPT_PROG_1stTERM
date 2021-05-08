// DEF_TR( NAME_, STR_, TR_ID_ )

/* ALL TERM SYMBOLS */
    DEF_TR (LABL    , :       , 0x00)
    DEF_TR (PROC    , PROC    , 0x01)
    DEF_TR (ENDP    , ENDP    , 0x02)
    DEF_TR (L_SQ_BR , [       , 0x03)
    DEF_TR (R_SQ_BR , ]       , 0x04)
    DEF_TR (PLUS    , +       , 0x05)
    DEF_TR (MINUS   , -       , 0x06)
    DEF_TR (SEMCOL  , ;       , 0x07)

/* REGISTERS - EASIER TO COUNT THEM AS TERMINALS */
    DEF_TR (R00     , R00     , 0x17)
    DEF_TR (R01     , R01     , 0x18)
    DEF_TR (R02     , R02     , 0x19)
    DEF_TR (R03     , R03     , 0x1A)
    DEF_TR (R04     , R04     , 0x1B)
    DEF_TR (R05     , R05     , 0x1C)
    DEF_TR (R06     , R06     , 0x1D)
    DEF_TR (R07     , R07     , 0x1E)