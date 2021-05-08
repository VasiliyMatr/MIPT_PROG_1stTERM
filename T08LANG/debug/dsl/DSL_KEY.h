
/* DEF_KEY( NAME_, CODE_FT_PT_, CODE_SD_PT_, OPRS_INFO_, ASM_NAME_ ) */
/*           name             code ft part           code sd part      operands info     asm name */
/* common operation defines */
  DEF_KEY (PLUS_        , CODE ("+\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "ADDS"     )
  DEF_KEY (MINUS_       , CODE ("-\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "SUBS"     )
  DEF_KEY (MULTIPLY_    , CODE ("*\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "MULS"     )
  DEF_KEY (DIVIDE_      , CODE ("/\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "DIVS"     )
  DEF_KEY (POWER_       , CODE ("^\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "POWS"     )
  DEF_KEY (ASSIGN_      , CODE ("=\0\0\0\0\0\0\0") ,            0             ,       BIN_       , "POP "     )

/* common lang operations */
  DEF_KEY (IF_          , CODE ("root\0\0\0\0"   ) ,            0             ,       BIN_       , ""        )
  DEF_KEY (WHILE_       , CODE ("inductio"       ) , CODE ("n\0\0\0\0\0\0\0") ,       BIN_       , ""        )
  DEF_KEY (RETURN_      , CODE ("answer\0\0"     ) ,            0             ,       UNR_       , ""        )
  DEF_KEY (BREAK_       , CODE ("fuckThis"       ) ,            0             ,        0         , ""        )
  DEF_KEY (VAR_DEC_     , CODE ("forEach\0"      ) ,            0             ,       UNR_       , ""        )
  DEF_KEY (INPUT_       , CODE ("exists\0\0"     ) ,            0             ,       UNR_       , ""        )
  DEF_KEY (OUTPUT_      , CODE ("consider"       ) ,            0             ,       UNR_       , ""        )