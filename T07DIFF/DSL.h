
#define LITTLE_ENDIAN

/* info about operands */
#define BIN 0x3 /* 11 in bin */
#define UNL 0x2 /* 10 in bin */
#define UNR 0x1 /* 01 in bin */

#ifdef LITTLE_ENDIAN
    #define code( name )                                                                                                \
     (NAME_)name[0]        | ((NAME_)name[1] << 8)  | ((NAME_)name[2] << 16) | ((NAME_)name[3] << 24) |                 \
    ((NAME_)name[4] << 32) | ((NAME_)name[5] << 40) | ((NAME_)name[6] << 48) | ((NAME_)name[7] << 56)
#else
    #define code( name )                                                                                                \
     (NAME_)name[7]        | ((NAME_)name[6] << 8)  | ((NAME_)name[5] << 16) | ((NAME_)name[4] << 24) |                 \
    ((NAME_)name[3] << 32) | ((NAME_)name[2] << 40) | ((NAME_)name[1] << 48) | ((NAME_)name[0] << 56)
#endif

/* common operation defines
*           name                code              operands info    priority         calc action                to do in latex       latex name  */
DEF_OP (PLUS        , code ("+\0\0\0\0\0\0\0") ,       BIN       ,   0      , left_val + righ_val       ,{{#1}    +       {#2}    }, \\plusf )
DEF_OP (MINUS       , code ("-\0\0\0\0\0\0\0") ,       BIN       ,   0      , left_val - righ_val       ,{{#1}    -       {#2}    }, \\minf  )
DEF_OP (MULTIPLY    , code ("*\0\0\0\0\0\0\0") ,       BIN       ,   10     , left_val * righ_val       ,{{#1}    \\cdot  {#2}    }, \\mulf  )
DEF_OP (DIVIDE      , code ("/\0\0\0\0\0\0\0") ,       BIN       ,   10     , left_val / righ_val       ,{        \\frac  {#1}{#2}}, \\divf  )
DEF_OP (POWER       , code ("^\0\0\0\0\0\0\0") ,       BIN       ,   100    , pow (left_val, righ_val)  ,{{#1}    ^       {#2}    }, \\powf  )

/* rval funcs defines
*           name                code              operands info    priority         calc action                to do in latex       latex name  */
DEF_OP (LN          , code ("ln\0\0\0\0\0\0" ) ,       UNR       ,  MAX_PRI , logf  (righ_val)          ,{{#1}    \\ln    {#2}    }, \\lnf  )

DEF_OP (SIN         , code ("sin\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , sin (righ_val)            ,{{#1}      sin   {#2}    }, \\sinf  )
DEF_OP (COS         , code ("cos\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , cos (righ_val)            ,{{#1}      cos   {#2}    }, \\cosf  )
DEF_OP (TAN         , code ("tan\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , tan (righ_val)            ,{{#1}      tan   {#2}    }, \\tanf  )
DEF_OP (CTG         , code ("ctg\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , 1 / tan (righ_val)        ,{{#1}      ctg   {#2}    }, \\ctgf  )

DEF_OP (ASIN        , code ("asin\0\0\0\0"   ) ,       UNR       ,  MAX_PRI , asin (righ_val)            ,{{#1}      asin   {#2}  }, \\asinf )
DEF_OP (ACOS        , code ("acos\0\0\0\0"   ) ,       UNR       ,  MAX_PRI , acos (righ_val)            ,{{#1}      acos   {#2}  }, \\acosf )
DEF_OP (ATAN        , code ("atan\0\0\0\0"   ) ,       UNR       ,  MAX_PRI , atan (righ_val)            ,{{#1}      atan   {#2}  }, \\atanf )
DEF_OP (ACTG        , code ("actg\0\0\0\0"   ) ,       UNR       ,  MAX_PRI , Pi / 2 - atan (righ_val)   ,{{#1}      actg   {#2}  }, \\actgf )

DEF_OP (SH          , code ("sh\0\0\0\0\0\0" ) ,       UNR       ,  MAX_PRI , sinh (righ_val)            ,{{#1}      sh     {#2}  }, \\shf   )
DEF_OP (CH          , code ("ch\0\0\0\0\0\0" ) ,       UNR       ,  MAX_PRI , cosh (righ_val)            ,{{#1}      ch     {#2}  }, \\chf   )
DEF_OP (TGH         , code ("tgh\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , tanh (righ_val)            ,{{#1}      th     {#2}  }, \\thf   )
DEF_OP (CTH         , code ("cth\0\0\0\0\0"  ) ,       UNR       ,  MAX_PRI , 1 / tanh (righ_val)        ,{{#1}      cth    {#2}  }, \\cthf  )


/* bad value */
DEF_OP (UNDEFINED   , code ("undf\0\0\0\0"   ) ,        0        ,  MAX_PRI ,            NAN            ,{{#1}    undef   {#2}    }, \\undef )

#undef code()
#undef LITTLE_ENDIAN