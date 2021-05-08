#include "cpu.h"

int main ( int argc, char* argv[] )
{
    char* inName  = argv [argc - 1];

    /* version info */
    printf ("\n"
            "\xCF\x80" "Is`MaK assembler cpu, version - 2.2.8 release" "\n"
            "\n");

    /* cpu data init */
    Processor CPU;

    cpu_load (&CPU, inName);
    cpu_exec (CPU);

    return 0;
}