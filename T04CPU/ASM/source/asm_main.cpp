
#include "asm.h"

int main( int argc, char* argv[] )
{
    char* inName  = nullptr;
    char* outName = nullptr;

    /* version info */
    printf ("\n"
            "\xCF\x80" "Is`MaK assembler compiler, version - 1.3.3.7 release" "\n"
            "\n");

    /* args parce */
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            switch (argv[i][1])
            {
              /* Out file name */
                case 'o':

                if ((++i) == argc)
                    break;

                outName = argv[i];
                break;
            }
        else
            inName = argv[i];
    }

    /* assembler data init */
    AsmData_t asmData;
    asmData.in_name_  = inName;
    asmData.out_name_ = outName;

    run (&asmData);

    return 0;

}