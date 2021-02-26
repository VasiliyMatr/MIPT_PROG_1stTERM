
/*           name             code ft part           code sd part      operands info  */
/* common operation defines */
  DEF_KEY (PLUS_        , CODE ("+\0\0\0\0\0\0\0") ,         0       ,       BIN       )
  DEF_KEY (MINUS_       , CODE ("-\0\0\0\0\0\0\0") ,         0       ,       BIN       )
  DEF_KEY (MULTIPLY_    , CODE ("*\0\0\0\0\0\0\0") ,         0       ,       BIN       )
  DEF_KEY (DIVIDE_      , CODE ("/\0\0\0\0\0\0\0") ,         0       ,       BIN       )
  DEF_KEY (POWER_       , CODE ("^\0\0\0\0\0\0\0") ,         0       ,       BIN       )
/* righ operand common funcs */
  DEF_KEY (LN_          , CODE ("ln\0\0\0\0\0\0" ) ,         0       ,       UNR       )
  DEF_KEY (SIN_         , CODE ("sin\0\0\0\0\0"  ) ,         0       ,       UNR       )
  DEF_KEY (COS_         , CODE ("cos\0\0\0\0\0"  ) ,         0       ,       UNR       )
  DEF_KEY (TAN_         , CODE ("tan\0\0\0\0\0"  ) ,         0       ,       UNR       )
  DEF_KEY (CTG_         , CODE ("ctg\0\0\0\0\0"  ) ,         0       ,       UNR       )
  DEF_KEY (ASIN_        , CODE ("asin\0\0\0\0"   ) ,         0       ,       UNR       )
  DEF_KEY (ACOS_        , CODE ("acos\0\0\0\0"   ) ,         0       ,       UNR       )
  DEF_KEY (ATAN_        , CODE ("atan\0\0\0\0"   ) ,         0       ,       UNR       )
  DEF_KEY (ACTG_        , CODE ("actg\0\0\0\0"   ) ,         0       ,       UNR       )
  DEF_KEY (SH_          , CODE ("sh\0\0\0\0\0\0" ) ,         0       ,       UNR       )
  DEF_KEY (CH_          , CODE ("ch\0\0\0\0\0\0" ) ,         0       ,       UNR       )
  DEF_KEY (TGH_         , CODE ("tgh\0\0\0\0\0"  ) ,         0       ,       UNR       )
  DEF_KEY (CTH_         , CODE ("cth\0\0\0\0\0"  ) ,         0       ,       UNR       )
/* common lang operations */
  DEF_KEY (IF_          , CODE ("root\0\0\0\0"   ) ,         0       ,       BIN       )
  DEF_KEY (WHILE_       , CODE ("findroot"       ) ,         0       ,       BIN       )
  DEF_KEY (RETURN_      , CODE ("answer\0\0"     ) ,         0       ,       UNR       )
  DEF_KEY (BREAK_       , CODE ("fuckthis"       ) ,         0       ,        0        )
  DEF_KEY (VAR_DEC_     , CODE ("set\0\0\0\0\0"  ) ,         0       ,       UNR       )