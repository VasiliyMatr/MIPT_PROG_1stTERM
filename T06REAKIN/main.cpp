
#include "akin.h"

int main()
{
    akin_t akin;

    akin_ctor (&akin);

    akin_menu (&akin);

    akin_dstr (&akin);

    return 0;
}
