#define NDBG

#include "CPU.h"
#include "USER_STACK.h"

int main()
{
    const char      log_name1[100]       = "..\\files\\log1_cpu";
    const char      log_name2[100]       = "..\\files\\log2_cpu";
    const char      in_name[100]        = "..\\files\\asm_t.txt";
    
    CPU_t CPU (1, log_name1, log_name2, NO_DBG);

    CPU.upload_program (in_name);
  
    int val = 1;

    for (int i = 0;; i++) 
    {
        CPU.do_program ();
        CPU.error_output ();
    }

    getchar();

    printf ("%i", val);

    return 1;
}