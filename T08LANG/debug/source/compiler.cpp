
#include "compiler.hpp"
/* compiler FUNCS */

tree_t* cmpl_getTreep( const Compiler* const cmpl )
{
    /* check calctor.h for CMPL_TR_SHT_ constand definition*/
    return (tree_t* )((size_t)cmpl + CMPL_TR_SHT_);
}

Err_t cmpl_ctor( Compiler* const cmpl )
{
    Err_t error = OK_;
    /* just constructing data tree */
    tree_ctor (cmpl_getTreep (cmpl), &error);

    return error;
}

Err_t cmpl_dstr( Compiler* const cmpl )
{
    Err_t error = OK_;

    /* destructing data tree */
    tree_dstr (cmpl_getTreep (cmpl), &error);

    /* free code if neede */
    if (cmpl->code_buff_ != nullptr)
    {
        free (cmpl->code_buff_);
        cmpl->code_buff_ = nullptr;
    }

    if (cmpl->tokens_ != nullptr)
    {
        free (cmpl->tokens_);
        cmpl->tokens_ = nullptr;
    }

    return error;
}

Err_t cmpl_verify( const Compiler* const cmpl )
{
    return OK_;
}

void compile( const char* inName, const char* outName, bool needTreeFlag )
{
    Compiler cmpl;
    Err_t error = OK_;
    cmpl_ctor (&cmpl);

    error = tokenize (&cmpl, inName);
    if (error)
    {
        cmplErrOut (&cmpl);
        cmpl_dstr (&cmpl);
        return;
    }

    setData (&cmpl);
    error = getTree ();
    if (error)
    {
        getError   (&cmpl);
        cmplErrOut (&cmpl);
        cmpl_dstr  (&cmpl);
        return;
    }

    if (needTreeFlag)
        tree_dump (cmpl_getTreep (&cmpl));

    error = putTree (&cmpl, outName);
    if (error != Err_t::OK_)
    {
        cmplErrOut (&cmpl);
        cmpl_dstr (&cmpl);
        return;
    }

    cmpl_dstr (&cmpl);
}