
#include "cpu.h"

err_t cpu_load( Processor* CPU, const char* const in_file_name )
{
    if (isBadPtr (CPU) || isBadPtr (in_file_name)) return BAD_PTR;

    FILE* in_file = fopen (in_file_name, "rb");
    if (in_file == nullptr) return FILE_ERR;

    /* reading file */
        fseek (in_file, 0, SEEK_END);
        CPU->code_size_ = ftell (in_file) / sizeof(int);
        rewind (in_file);

        CPU->code_ = (int* )calloc (sizeof (int), CPU->code_size_);
        if (CPU->code_ == nullptr)
        {
            fclose (in_file);
            return MEM_ERR;
        }

        fread  (CPU->code_, sizeof (int), CPU->code_size_, in_file);
        fclose (in_file);

    return OK;
}

err_t cpu_exec( Processor CPU )
{
    if (isBadPtr (CPU.code_)) return CMD_ERR;

    auto error = AbstractStack::err_t::OK_;

    for (;;)
    {
        CPU.stk_.pop (&error);
        if (error != AbstractStack::err_t::OK_) break;
    }

    if (error != AbstractStack::err_t::STACK_EMPTY_) return STK_ERR;

    error = AbstractStack::err_t::OK_;

    for (size_t RIP_ = 0;; RIP_++)
    {
        switch (CPU.code_[RIP_])
        {
            #define DEF_CMD( NAME_, STR_, CMD_ID_, CMD_CPU_, CMD_ASM_ ) \
            case CMD_ID_:                                               \
            {                                                           \
                CMD_CPU_;                                               \
            }                                                           \
                                                                        \
            break;

            #include "DSL_CMD.h"

            #undef DEF_CMD

            default:
                return UNDF_CMD;

        }
    }

    return OK;
}