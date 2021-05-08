
#ifndef INCLUDED_STACK_
#define INCLUDED_STACK_

/* dbg mode choose stuff */

    #ifndef PRJ_STK_DBG_ON_
        #error PRJ_STK_DBG_ON_ should be defined as true or false
    #endif

/* ********************* */

/* capacity stuff */
    #define STK_CAPACITY_   512

class AbstractStack
{

/* types */
public:

    /* for errors output */

        enum class err_t
        {
            /* no errors */
            OK_ = 0,

            /* canaries problems */
            TOP_STK_CAN_DEAD_ = -0xFFFF,
            BOT_STK_CAN_DEAD_,
            TOP_DATA_CAN_DEAD_,
            BOT_DATA_CAN_DEAD_,

            /* hash is currupted */
            HASH_IS_BAD_,

            /* size is bigger than capacity */
            SIZE_CAP_ERR_,

            /* error with errorPtr */
            BAD_ERROR_P_,

            /* stack is full */
            STACK_OVERFLOW_ = 0xFFFF,
            /* stack is empty */
            STACK_EMPTY_,
        };

    /* ***************** */

    /* For dbg mode choose */

        enum class dbgMode_t
        {
            /* no debug */
            DBG_OFF_ = 0x0FFF0FFF,
            /* only canaries */
            DBG_CAN_ = 0xC0C0C0,
            /* full dbg with hashes and canaries */
            DBG_FULL_ = 0xDB6F011
        };

    /* ******************* */

};

template <typename data_t>
class Stack : public AbstractStack
{
public:

/* FIELDS */

/* Define to create canaries. */

    #if PRJ_STK_DBG_ON_

    #define CAN_( NAME ) canary_t NAME = CANARY_INIT_

    #else

    #define CAN_( NAME ) 

    #endif

/* ************************** */

private:

    /* stack top canary */
    CAN_ (STK_TOP_CAN_);

    /* num of elements in stack */
    size_t size_ = 0;

    /* stack data */

        /* stack data top canary */
        CAN_ (DATA_TOP_CAN_);

        /* data */
        data_t data_[STK_CAPACITY_];

        /* stack data bot canary */
        CAN_ (DATA_BOT_CAN_);

    /* ********** */

    /* this stack DBG MODE */
    #if PRJ_STK_DBG_ON_
        const dbgMode_t dbgMode_;
    #endif

    /* This stack hash */
    #if PRJ_STK_DBG_ON_
        hash_t hash_ = HASH_INIT_;
    #endif

    /* Stack bot canary. */
    CAN_ (STK_BOT_CAN_);

/* ****** */

/* METHODS */

public:
    /*
        Ctor init all debug features.
        Do nothing if debug off
    */
    Stack( dbgMode_t dbgMode = dbgMode_t::DBG_OFF_ );

    /*
        errorPtr - ptr to variable (used for errors output).
        Should be inited with err_t::OK_; BUT, if there is a
        default value and debug mode is DBG_OFF_ - it isn't used
    */
    data_t  pop ( err_t *errorPtr = nullptr );
    err_t   push( data_t new_data, err_t *errorPtr = nullptr );

    /* dump func */
    void    dump( bool needCheck );
    /* verification func */
    err_t   verify();

private:

    /* Internal verification funcs */
    err_t canCheck();
    err_t hashCheck();

/* ******* */
};

/* including methods realization */
    #include "../source/stack.cpp"

/* undefs */
    #undef CAN_
    #undef STK_CAPACITY_

#endif