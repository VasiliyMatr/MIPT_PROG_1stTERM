#include "stack.h"


/*         DEFINES         */

#if PRJ_STK_DBG_ON_

    /*
        This define makes error ptr and error data verification.
        Methods should be given with correct ptr to stkErr_t
        variable and value of this variable should be equal to
        stkErr_t::OK_. Or user can send nullptr to stack with DBG_OFF_
        mode, then there will be no debug at all (nullptr checked in
        STK_VERIFY_).
    */
    #define ERROR_VERIFY_( ERR_PTR, TO_RET )                        \
    {                                                               \
        /* Ptr validity check */                                    \
        if (isBadPtr (ERR_PTR))                                     \
        {                                                           \
            CRIT_ERROR_OUT_ ("BAD ERROR PTR IN STACK!");            \
            return TO_RET;                                          \
        }                                                           \
                                                                    \
        /* Value validity check. */                                 \
        if (*ERR_PTR != err_t::OK_)                                 \
        {                                                           \
            CRIT_ERROR_OUT_ ("NOT OK ERROR IN STACK!");             \
            return TO_RET;                                          \
        }                                                           \
    }

    /*
        All needed verifications in one define. This define is
        used in all stack methods (if verification is needed there)
    */
    #define STK_VERIFY_( ERR_PTR, TO_RET_EP, TO_RET_VR )            \
    {                                                               \
        if (ERR_PTR != nullptr)                                     \
        {                                                           \
            /* Verifying error ptr. */                              \
            ERROR_VERIFY_  (ERR_PTR, TO_RET_EP);                    \
                                                                    \
            /* Verifying all things in Stack. */                    \
            *ERR_PTR = verify ();                                   \
            if (*ERR_PTR != err_t::OK_)                             \
            {                                                       \
                dump (true);                                        \
                return TO_RET_VR;                                   \
            }                                                       \
        }                                                           \
                                                                    \
        else if (dbgMode_ != dbgMode_t::DBG_OFF_)                   \
        {                                                           \
            /* there is not DBG_OFF_ mode, but ptr is nullptr */    \
            CRIT_ERROR_OUT_ ("NULLPTR ERROR PTR IN STACK!");        \
            return TO_RET_EP;                                       \
        }                                                           \
    }

#else

    /* No debug. */
    #define STK_VERIFY_( ERR_PTR, TO_RET_EP, TO_RET_VR ) ;
    #define ERROR_VERIFY_( ERR_PTR, TO_RET_EP, TO_RET_VR ) ;

#endif

/* *********************** */

template <typename data_t>
Stack<data_t>::Stack( dbgMode_t dbgMode )/* DBG_OFF_ - default value */
#if PRJ_STK_DBG_ON_
    : dbgMode_ (dbgMode)
{

    if (dbgMode_ != dbgMode_t::DBG_OFF_)
        /* Poison to all data in Stack. */
        for (size_t i = 0; i < STK_CAPACITY_; i++)
            data_[i] = getPoison (data_);

    if (dbgMode_ == dbgMode_t::DBG_FULL_)
    {
        /* Hashing inited stack. */
        hash_ = HASH_INIT_;
        hash_ = hashF (this, sizeof (Stack));
    }

}

#else

{}

#endif

template <typename data_t>
typename Stack<data_t>::err_t Stack<data_t>::push( data_t new_data, err_t *errorPtr )/*
                                                                nullptr - default value for 2nd arg, no error
                                                                output in this case (DBG_OFF_ should be choosed)
                                                                error is always returned by this func
                                                             */
{
    /* returns needed error code if smth isn't ok */
    STK_VERIFY_ (errorPtr, err_t::BAD_ERROR_P_, *errorPtr);

    if (size_ == STK_CAPACITY_)
    {
    /* No place for new value in Stack. */
        if (errorPtr != nullptr)
        {
            *errorPtr = err_t::STACK_OVERFLOW_;
            return err_t::STACK_OVERFLOW_;
        }

        else
            return err_t::STACK_OVERFLOW_;
    }

    /* Data update. */
    data_[size_++] = new_data;

#if PRJ_STK_DBG_ON_

    if (dbgMode_ == dbgMode_t::DBG_FULL_)
    {
        /* Hashing Stack. */
        hash_ = HASH_INIT_;
        hash_ = hashF (this, sizeof (Stack));
    }

#endif

    return err_t::OK_;
}

template <typename data_t>
data_t Stack<data_t>::pop( err_t *errorPtr )/*
                                                nullptr - default value for 1st arg, no error
                                                output in this case (DBG_OFF_ should be choosed).
                                                POISON_ val returned if stack is empty (in all cases).
                                            */
{
    STK_VERIFY_ (errorPtr, 0, 0);

    if (size_ == 0)
    {
    /* No value to pop */
        if (errorPtr != nullptr)
        {
            *errorPtr = err_t::STACK_EMPTY_;
            return getPoison (data_);
        }

        else
            return getPoison (data_);
    }

    /* Getting data. */
    data_t toRet = data_[--size_];

#if PRJ_STK_DBG_ON_

    /* Poison to fread space. */
    if (dbgMode_ != dbgMode_t::DBG_OFF_)
        data_[size_] = getPoison (data_);

    if (dbgMode_ == dbgMode_t::DBG_FULL_)
    {
        /* Hashing Stack. */
        hash_ = HASH_INIT_;
        hash_ = hashF (this, sizeof (Stack));
    }

#endif

    return toRet;
}

template <typename data_t>
AbstractStack::err_t Stack<data_t>::verify()
{

#if PRJ_STK_DBG_ON_

    if (dbgMode_ == dbgMode_t::DBG_OFF_)
        /* No checks. */
        return err_t::OK_;

    if (size_ > STK_CAPACITY_)
        /* Size can't be bigger than capacity. */
        return err_t::SIZE_CAP_ERR_;

    err_t error = canCheck ();

    if (error != err_t::OK_)
        return error;

    if (dbgMode_ == dbgMode_t::DBG_FULL_)
        error = hashCheck ();

    return error;

#else

    return err_t::OK_;

#endif
}

template <typename data_t>
AbstractStack::err_t Stack<data_t>::canCheck()
{

#if PRJ_STK_DBG_ON_

    /* Just checking all canaries values. */

    if (STK_TOP_CAN_ != CANARY_INIT_)
        return err_t::TOP_STK_CAN_DEAD_;
    if (STK_BOT_CAN_ != CANARY_INIT_)
        return err_t::BOT_STK_CAN_DEAD_;

    if (DATA_TOP_CAN_ != CANARY_INIT_)
        return err_t::TOP_DATA_CAN_DEAD_;
    if (DATA_BOT_CAN_ != CANARY_INIT_)
        return err_t::BOT_DATA_CAN_DEAD_;

    return err_t::OK_;

#else

    /* Shouldn't get here. */
    CRIT_ERROR_OUT_ ("verification func called in release mode!");
    exit (0);
    return err_t::OK_;

#endif
}

template <typename data_t>
typename Stack<data_t>::err_t Stack<data_t>::hashCheck()
{
#if PRJ_STK_DBG_ON_

    /* Remember old hash value. */
    hash_t oldHash = hash_;

    /* Hasing Stack. */
    hash_ = HASH_INIT_;
    hash_ = hashF (this, sizeof (Stack));

    /* Comparing values and returning needed value. */
    return hash_ == oldHash ? err_t::OK_ : err_t::HASH_IS_BAD_;

#else

    /* Shouldn't get here. */
    CRIT_ERROR_OUT_ ("verification func called in release mode!");
    exit (0);
    return err_t::OK_;

#endif
}


/* undefs */
    #undef STK_VERIFY_
    #undef ERROR_VERIFY_

template <typename data_t>
void Stack<data_t>::dump( bool needCheck )
{

    FILE* outFile = fopen("dump.txt", "wb");
    if (outFile == nullptr)
        return;

#if PRJ_STK_DBG_ON_

    err_t error = err_t::OK_;

    if (needCheck)
        error = verify ();

    fprintf (outFile, "Stack :"     "\n"
                      "Addr  : %p"  "\n"
                      "Error : %X"  "\n"
                      "Hash  : %X"  "\n"
                      "", this, (unsigned int) error, hash_);

#else

    fprintf(outFile, "Stack :"     "\n"
                     "Addr  : %X"  "\n"
                     "", this);

#endif

    for (size_t dataId = 0; dataId < STK_CAPACITY_; dataId++)
    {
        fprintf (outFile, "       [%012lX] > %08X" "\n", dataId, data_[dataId]);
    }

    fclose (outFile);
}