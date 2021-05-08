
#include "compiler.hpp"

int main( int argc, char* argv[] )
{
    bool needTreeFlag = false;
    char* inName  = nullptr;
    char* outName = nullptr;

    /* version info */
    printf ("\n"
            "\xCF\x80" "Is`MaK programming language compiler, version - 1.0 release" "\n"
            "\n");

    /* args parce */
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            switch (argv[i][1])
            {
              /* Need tree dump */
                case 'T':

                needTreeFlag = true;
                break;

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

    compile (inName, outName, needTreeFlag);

    return 0;
}