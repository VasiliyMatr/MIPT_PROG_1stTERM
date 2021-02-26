
/* to choose byte order depending on machine arch */
#define IS_LITTLE_ENDIAN_ true

#include "compiler.h"

int main()
{
    char CODE_[] = "code.txt";

    Compiler cmpl;

    cmpl_ctor (&cmpl);

    cmpl_tokinize (&cmpl, CODE_);

    setData (&cmpl);

    getTree();

    return 0;
}