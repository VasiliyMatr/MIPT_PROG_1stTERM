#include "list.h"

template <typename data_t>
list_t<data_t>::list_t(  const size_t initial_capacity, const char* const dump_file_name  ): dump_file_name_ (dump_file_name)
{
    /* wrong initial capacity */
    if (initial_capacity == 0)
    {
        return;
    }

    /* dump file name check */
    if (isBadReadPtr (dump_file_name))
    {
        printf ("\n     no dump file, bad read ptr to name!     \n");
    }

    /* allocating memory for data units */
    data_units_ = (data_unit_t* )calloc (sizeof (data_unit_t), initial_capacity);
    if (data_units_ == nullptr)
    {
        return;
    }

    capacity_ = initial_capacity;

    /* init all units as free */
    free_head_ = 0;

    for (size_t unit_id = 0; unit_id < initial_capacity - 1; unit_id++)
    {   
        data_units_[unit_id] = data_unit_t{ POISON_, BAD_ID_VALUE_, (output_t)unit_id + 1, true };
    }

    /* init last unit */
    data_units_[initial_capacity - 1] = data_unit_t{ POISON_, BAD_ID_VALUE_, BAD_ID_VALUE_, true };
}

template <typename data_t>
list_t<data_t>::~list_t()
{
    if (!isBadReadPtr (data_units_))
    {
        free (data_units_);
        data_units_ = nullptr;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::attach_to_data_tail( const data_t data )
{
    verify();

    /* to return later */
    size_t new_unit_id = free_head_;

    /* no more free units */
    if (new_unit_id == BAD_ID_VALUE_)
    {
        return LIST_NO_FREE_UNITS;
    }

    /* updating info about free units */
    free_head_ = data_units_[free_head_].next;

    /* there are 0 busy units */
    if (size_ == 0)
    {
        /* updating info about tail and head units */
        data_head_ = new_unit_id;
        data_tail_ = new_unit_id;

        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, BAD_ID_VALUE_, BAD_ID_VALUE_, false };

        size_++;

        return new_unit_id;
    }

    else
    {   
        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, data_tail_, BAD_ID_VALUE_, false };

        /* updating previous tail data */
        data_units_[data_tail_].next = new_unit_id;
        
        /* updating info about tail unit */
        data_tail_ = new_unit_id;
        
        size_++;

        return new_unit_id;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::attach_to_data_head( const data_t data )
{
    verify();

    /* good reference lost now */
    sorted_ = false;

    /* id of allocated unit */
    size_t new_unit_id = free_head_;

    /* no more free units */
    if (new_unit_id == BAD_ID_VALUE_)
    {
        return LIST_NO_FREE_UNITS;
    }

    /* updating info about free units */
    free_head_ = data_units_[free_head_].next;

    /* there are 0 busy units */
    if (size_ == 0)
    {
        /* updating info about tail and head units */
        this->data_head_ = new_unit_id;
        this->data_tail_ = new_unit_id;

        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, BAD_ID_VALUE_, BAD_ID_VALUE_, false };

        size_++;

        return new_unit_id;
    }

    else
    {   
        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, BAD_ID_VALUE_, data_head_, false };

        /* updating previous head data */
        this->data_units_[data_head_].prev = new_unit_id;
        
        /* updating info about head unit */
        data_head_ = new_unit_id;
        
        size_++;

        return new_unit_id;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::attach_next_by_id( const size_t unit_id_2_attach_next_2, const data_t data )
{
    verify();

    /* for fast access */
    size_t new_unit_id = free_head_;

    /* no more free units */
    if (new_unit_id == BAD_ID_VALUE_)
    {
        return LIST_NO_FREE_UNITS;
    }

    if (data_units_[unit_id_2_attach_next_2].free)
    {
        return LIST_FREE_ATTACH_ERROR;
    }

    /* there are 0 busy units or unit_id is tail id */
    if (size_ == 0 || unit_id_2_attach_next_2 == data_tail_)
    {
        return attach_to_data_tail (data);
    }

    /* unit_if is head id */
    if (unit_id_2_attach_next_2 == data_head_)
    {
        return attach_to_data_head (data);
    }

    sorted_ = false;

    /* need to allocate body element */
    {
        /* for easier access */
        size_t unit_id_2_attach_prev_2 = data_units_[unit_id_2_attach_next_2].next;

        /* updating info about free units */
        free_head_ = data_units_[free_head_].next;
        
        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, (output_t)unit_id_2_attach_next_2, (output_t)unit_id_2_attach_prev_2, false };

        /* writing info about new unit in prev and next units */
        data_units_[unit_id_2_attach_next_2].next = new_unit_id;
        data_units_[unit_id_2_attach_prev_2].prev = new_unit_id;

        size_++;

        return new_unit_id;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::attach_prev_by_id( const size_t unit_id_2_attach_prev_2, const data_t data )
{
    verify();

    /* for fast access */
    size_t new_unit_id = free_head_;

    /* no more free units */
    if (new_unit_id == BAD_ID_VALUE_)
    {
        return LIST_NO_FREE_UNITS;
    }

    if (data_units_[unit_id_2_attach_prev_2].free)
    {
        return LIST_FREE_ATTACH_ERROR;
    }

    /* there are 0 busy units or unit_id is tail id */
    if (size_ == 0 || unit_id_2_attach_prev_2 == data_tail_)
    {
        return attach_to_data_tail (data);
    }

    /* unit_id is head id */
    if (unit_id_2_attach_prev_2 == data_head_)
    {
        return attach_to_data_head (data);
    }

    sorted_ = false;

    /* need to allocate body element */
    {
        /* for easier access */
        size_t unit_id_2_attach_next_2 = data_units_[unit_id_2_attach_prev_2].prev;

        /* updating info about free units */
        free_head_ = data_units_[free_head_].next;
        
        /* writing data to new busy unit */
        data_units_[new_unit_id] = data_unit_t { data, (output_t)unit_id_2_attach_next_2, (output_t)unit_id_2_attach_prev_2, false };

        /* writing info about new unit in prev and next units */
        data_units_[unit_id_2_attach_next_2].next = new_unit_id;
        data_units_[unit_id_2_attach_prev_2].prev = new_unit_id;

        size_++;

        return new_unit_id;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::free_unit_on_head()
{
    verify();

    /* cant free head data unit */
    if (size_ == 0)
    {
        return LIST_EMPT_ERROR;
    }

    /* to remember new data head */
    output_t new_data_head = data_units_[data_head_].next;

    /* free curr data head */
    data_units_[data_head_] = data_unit_t{ POISON_, BAD_ID_VALUE_, free_head_, true };
    free_head_ = data_head_;

    /* setup of new data head unit */
    data_units_[new_data_head].prev = BAD_ID_VALUE_;

    /* updating info about data head and capacity*/
    data_head_ = new_data_head;
    size_--;

    /* last element was deleted */
    if (size_ == 0)
    {
        data_tail_ = BAD_ID_VALUE_;
    }
    
    return OK;
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::free_unit_on_tail()
{
    verify();

    /* cant free tail data unit */
    if (size_ == 0)
    {
        return LIST_EMPT_ERROR;
    }

    /* to remember new data tail */
    output_t new_data_tail = data_units_[data_tail_].prev;

    /* free curr data tail */
    data_units_[data_tail_] = data_unit_t{ POISON_, BAD_ID_VALUE_, free_head_, true };
    free_head_ = data_tail_;

    /* setup of new data tail unit */
    data_units_[new_data_tail].next = BAD_ID_VALUE_;

    /* updating info about data tail and capacity*/
    data_tail_ = new_data_tail;
    size_--;

    /* last element was deleted */
    if (size_ == 0)
    {
        data_head_ = BAD_ID_VALUE_;
    }

    return OK;
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::free_unit_by_id( const size_t unit_id )
{
    verify();

    /* cant free data unit */
    if (data_units_[unit_id].free)
    {
        return LIST_FREE_FREE_ERROR;
    }

    /* just need to free head unit */
    if (unit_id == data_head_)
    {
        return free_unit_on_head();
    }
    
    /* just need to free tail unit */
    if (unit_id == data_tail_)
    {
        return free_unit_on_tail();
    }

    sorted_ = false;

    /* need to free body unit */
    {
        /* for easier access */
        size_t next_unit_id = data_units_[unit_id].next;
        size_t prev_unit_id = data_units_[unit_id].prev;

        /* updating info in next and prev elements */
        data_units_[prev_unit_id].next = next_unit_id;
        data_units_[next_unit_id].prev = prev_unit_id;

        /* adding free unit to free list */
        data_units_[unit_id] = data_unit_t{ POISON_, -1, free_head_, true };
        free_head_ = unit_id;

        size_--;
        
        /* last element was deleted */
        if (size_ == 0)
        {
            data_head_ = BAD_ID_VALUE_;
            data_tail_ = BAD_ID_VALUE_;
        }
        
        return OK;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::slow_get_physical_unit_id_by_logical_id( const size_t log_id_2_find )
{
    verify();

    if (log_id_2_find >= size_)
    {
        return LIST_FREE_ATTACH_ERROR;
    }

    if (sorted_)
    {
        return log_id_2_find;
    }

    size_t log_unit_phys_id = data_head_;

    for (size_t log_id = 0; log_id < log_id_2_find; log_id++)
    {
        log_unit_phys_id = data_units_[log_unit_phys_id].next;
    }

    return log_unit_phys_id;
}
 
template <typename data_t>
typename data_t list_t<data_t>::get_data_by_id( const size_t unit_id )
{
    verify();

    if (unit_id > capacity_ || data_units_[unit_id].free)
    {
        return POISON_;
    }

    return data_units_[unit_id].data;
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::resize( const size_t new_size )
{
    verify();

    if (new_size > capacity_)
    {
        /* reallocating memory */
        data_unit_t* new_data_units = (data_unit_t* )realloc (data_units_, new_size * sizeof (data_unit_t));
        if (new_data_units == nullptr)
        {
            return MEM_ERR;
        }

        /* updating ptr */
        data_units_ = new_data_units;
        
        /* interlocking new free units, it works well for free_head_ == BAD_ID_VALUE_ */
        {
            /* last unit will have previous free_head_ as next */
            data_units_[new_size - 1] = data_unit_t{ POISON_, BAD_ID_VALUE_, free_head_, true };

            /* new free head */
            free_head_ = capacity_;

            for (size_t free_unit_id = capacity_; free_unit_id < new_size - 1; free_unit_id++)
            {
                data_units_[free_unit_id] = data_unit_t{ POISON_, BAD_ID_VALUE_, (output_t)free_unit_id + 1, true };
            }
        }

        capacity_ = new_size;

        return OK;
    }

    else
    {
        /* just will sort and cut */
        sort_data_units();
        /* reallocating memory */
        data_unit_t* new_data_units = (data_unit_t* )realloc (data_units_, new_size * sizeof (data_unit_t));
        if (new_data_units == nullptr)
        {
            return MEM_ERR;
        }
        
        /* fixing next id in last element */
        new_data_units[new_size - 1].next = BAD_ID_VALUE_;

        /* updating head and tail info */
        {

            if (size_ >= new_size)
            {
                data_head_ = 0;
                data_tail_ = new_size - 1;
                free_head_ = BAD_ID_VALUE_;
                size_ = new_size;
            }

            else if (size_ == 0)
            {
                data_head_ = BAD_ID_VALUE_;
                data_tail_ = BAD_ID_VALUE_;   
                free_head_ = 0;             
            }

            else
            {
                data_head_ = 0;
                data_tail_ = size_ - 1;
                free_head_ = size_;
            }
        }
        capacity_ = new_size;

        sorted_ = true;

        return OK;
    }
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::qsort_data_units()
{
    verify();

    /* reindexing by logical order to use prev ids in comparation algorithm. also prev ids will be in almost needed order after qsort */
    size_t log_unit_id = 0;

    /* stepping trought all allocated units */
    for (; log_unit_id < capacity_; log_unit_id++)
    {
        data_units_[data_head_].prev = log_unit_id;

        data_head_ = data_units_[data_head_].next;

        if (data_head_ == BAD_ID_VALUE_)
        {
            break;
        }
    }

    /* stepping throught all free units */
    for (size_t free_id = log_unit_id; free_id < capacity_; free_id++)
    {
        data_units_[free_head_].prev = free_id;

        free_head_ = data_units_[free_head_].next;

        if (free_head_ == BAD_ID_VALUE_)
        {
            break;
        }
    }

    /* sorting */
    qsort (data_units_, capacity_, sizeof(data_unit_t), data_units_compare);

    /* indexing data_head_ unit. if there are no busy data units, this indexing is still correct */
    {
        data_units_[0].next = 1;
        data_units_[0].prev = BAD_ID_VALUE_;
    }

    /* reindexing */
    for (size_t unit_id = 1; unit_id < size_; unit_id++)
    {
        data_units_[unit_id].next = unit_id + 1;
        data_units_[unit_id].prev = unit_id - 1;
    }

    /* indexing data_tail_ unit */
    if (size_ != 0)
    {
        data_units_[size_ - 1].next = BAD_ID_VALUE_;
    }

    /* indexng free units */
    for (size_t free_id = size_; free_id < capacity_; free_id++)
    {
        data_units_[free_id].next = free_id + 1;
        data_units_[free_id].prev = BAD_ID_VALUE_;
    }

    /* indexing free tail unit. if there are no free data units, this indexing is still correct  */
    data_units_[capacity_ - 1].next = BAD_ID_VALUE_;

    /* updating head and tail info */
    {
        if (size_ == 0)
        {
            data_head_ = BAD_ID_VALUE_;
            data_tail_ = BAD_ID_VALUE_;
            free_head_ = 0;
        }

        else
        {
            data_head_ = 0;
            data_tail_ = size_;

            if (size_ == capacity_)
            {
                free_head_ = BAD_ID_VALUE_;
            }

            else
            {
                free_head_ = size_;
            }
        }
    }

    return OK;
}

template <typename data_t>
int list_t<data_t>::data_units_compare( const void* first_unit, const void* secnd_unit )
{
    return (*((data_unit_t* )first_unit)).prev - (*((data_unit_t* )secnd_unit)).prev;
}

template <typename data_t>
typename list_t<data_t>::output_t list_t<data_t>::sort_data_units()
{
    data_unit_t* new_data_units = (data_unit_t* )calloc (sizeof (data_unit_t), capacity_);
    /* no more memory -> user can sort with qsort without new memory */
    if (new_data_units == nullptr)
    {
        return MEM_ERR;
    }

    /* copying data and indexng busy units */
    for (size_t unit_id = 0; unit_id < size_; unit_id++)
    {
        new_data_units[unit_id] = data_unit_t{ data_units_[data_head_].data, (output_t)unit_id - 1, (output_t)unit_id + 1, false };

        data_head_ = data_units_[data_head_].next;
        if (data_head_ == BAD_ID_VALUE_)
        {
            break;
        }
    }

    /* indexing 0 and last busy units */
    {
        new_data_units[size_ - 1].next = BAD_ID_VALUE_;
        new_data_units[0].prev = BAD_ID_VALUE_;
    }

    /* indexing free units */
    for (size_t free_id = size_; free_id < capacity_; free_id++)
    {
        new_data_units[free_id] = data_unit_t{ POISON_, BAD_ID_VALUE_, (output_t)free_id + 1, true };
    }

    /* indexing last unit next id */
    new_data_units[capacity_ - 1].next = BAD_ID_VALUE_;

    /* updating head and tail info */
    {
        if (size_ == 0)
        {
            data_head_ = BAD_ID_VALUE_;
            data_tail_ = BAD_ID_VALUE_;
            free_head_ = 0;
        }

        else
        {
            data_head_ = 0;
            data_tail_ = size_;

            if (size_ = capacity_)
            {
                free_head_ = BAD_ID_VALUE_;
            }

            else
            {
                free_head_ = size_;
            }
        }
    }

    /* free prev data and writing addr of new data */
    free (data_units_);
    data_units_ = new_data_units;
}

template <typename data_t>
ERR_t list_t<data_t>::verificator()
{
    if (isBadReadPtr (data_units_))
    {
        return LIST_UNITS_BADREAD_PTR;
    }

    /* to count free units */
    size_t free_units_num = 0;

    for (size_t unit_id = 0; unit_id < capacity_; unit_id++)
    {
        /* veryfying curr unit */
        UNIT_TYPE unit_type = verify_data_unit (unit_id);

        switch (unit_type)
        {
            /* counting num of free units */
            case LIST_FREE_UNIT:
            {
                free_units_num++;
                break;
            }
            /* body unit - ok */
            case LIST_BODY_UNIT:
            {
                break;
            }

            /* need to ckeck head position */
            case LIST_HEAD_UNIT:
            {
                if (unit_id != data_head_)
                {
                    return LIST_VERI_HEAD_ERROR;
                }
                
                break;
            }
            
            /* need to ckeck tail position */
            case LIST_TAIL_UNIT:
            {
                if (unit_id != data_tail_)
                {
                    return LIST_VERI_TAIL_ERROR;
                }

                break;
            }

            /* verificator errors */
            case LIST_WRONG_NEXT_ID_UNIT:
            {
                return LIST_VERI_NEXT_ERROR;
            }

            case LIST_WRONG_PREV_ID_UNIT:
            {
                return LIST_VERI_PREV_ERROR;
            }

            case LIST_UNDF_UNIT:
            {
                return LIST_UNIT_ERROR;
            }
        }
    }

    /* num of free units don't matching */
    if ((int)free_units_num != -(int)size_ + (int)capacity_)
    {
        return LIST_VERI_FREE_ERROR;
    }

    return OK;
}         

#define UNIT_PRINT( color, unit_type )                                                                                               \
{                                                                                                                                    \
    case unit_type:                                                                                                                  \
    {                                                                                                                                \
        buff_shift += sprintf (buff + buff_shift,                                                                                    \
            "{ rank = same \nUNIT%03d[style=\"filled\", fillcolor=\"%s\","                                                          \
            "label=\" <top> UNIT %03d | <prev> PREV  %03d  | <next> NEXT  %03d | %s | data: %9.4lf\"]; \n}",                         \
             unit_id, color, unit_id, data_units_[unit_id].prev, data_units_[unit_id].next, #unit_type, data_units_[unit_id].data);  \
        break;                                                                                                                       \
    }                                                                                                                                \
}                                                                                                                                    

template <typename data_t>
void list_t<data_t>::dump()
{
    /* max size of dump str about one data unit */
    static const int max_dump_str_len = 350;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * capacity_, max_dump_str_len);
    if (buff == nullptr)
    {
        printf ("\n\nNO MEMORY FOR DUMP BUFFER!\n\n");
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info about graph */
    buff_shift += sprintf (buff + buff_shift, "strict digraph G{ \n\
                                               rankdir=LR; \n\
                                               edge[color=\"black\"]; \n\
                                               node[color=\"black\", shape=record];\n");

    for (size_t unit_id = 0; unit_id < capacity_; unit_id++)
    {
        UNIT_TYPE unit_type = verify_data_unit (unit_id);

        switch (unit_type)
        {
            UNIT_PRINT ("#FF1D72", LIST_UNDF_UNIT);
            UNIT_PRINT ("#FF1D72", LIST_WRONG_NEXT_ID_UNIT);
            UNIT_PRINT ("#FF1D72", LIST_WRONG_PREV_ID_UNIT);

            UNIT_PRINT ("#24FF1D", LIST_FREE_UNIT);
            UNIT_PRINT ("#1DFFF5", LIST_HEAD_UNIT);
            UNIT_PRINT ("#1DFFF5", LIST_TAIL_UNIT);

            UNIT_PRINT ("#FFF707", LIST_BODY_UNIT);
        }

        if (data_units_[unit_id].prev != BAD_ID_VALUE_ && data_units_[unit_id].prev < capacity_)
        { 
            buff_shift += sprintf (buff + buff_shift, \
            "UNIT%03d: <prev> -> UNIT%03d: <top>; \n", unit_id, data_units_[unit_id].prev);
        }

        if (data_units_[unit_id].next != BAD_ID_VALUE_ && data_units_[unit_id].next < capacity_)
        { 
            buff_shift += sprintf (buff + buff_shift, \
            "UNIT%03d: <next> -> UNIT%03d: <top>; \n", unit_id, data_units_[unit_id].next);
        }
    }

    buff_shift += sprintf (buff + buff_shift, "}");

    FILE* out_file = fopen (dump_file_name_, "wb");
    if (out_file == nullptr)
    {
        free (buff);
        return;
    }

    fwrite (buff, sizeof (char), buff_shift, out_file);
    free (buff);
    fclose (out_file);

    system ("C:\\Users\\vasil\\Desktop\\Graphviz\\bin\\dot -Tjpg C:\\Users\\vasil\\source\\repos\\T05LIST\\dump_file.dot > C:\\Users\\vasil\\source\\repos\\T05LIST\\dump_file.jpg");

    // system ("dump_file.jpg");
    
    return;
}              

template <typename data_t>
typename list_t<data_t>::UNIT_TYPE list_t<data_t>::verify_data_unit( const size_t unit_id )
{
    /* for easier access */
    data_unit_t unit_2_verify = data_units_[unit_id];

    /* id of next data unit should be lesser or equal capacity */
    if (unit_2_verify.next > (int)capacity_)
    {
        return LIST_WRONG_NEXT_ID_UNIT;
    }
        
    /* id of next data unit should be lesser or equal capacity */
    if (unit_2_verify.prev > (int)capacity_)
    {
        return LIST_WRONG_PREV_ID_UNIT;
    }
    
    /* free unit found */
    if (unit_2_verify.free)
    {
        /* wrong ids */
        if (unit_2_verify.prev != BAD_ID_VALUE_)
        {
            return LIST_WRONG_PREV_ID_UNIT;
        }

        /* there are next unit and it isn't free */
        if (unit_2_verify.next != BAD_ID_VALUE_ && !data_units_[unit_2_verify.next].free)
        {
            return LIST_WRONG_NEXT_ID_UNIT;
        }

        /* all is ok */
        return LIST_FREE_UNIT;
    }
    
    /* body unit found */
    if (unit_2_verify.next != BAD_ID_VALUE_ && unit_2_verify.prev != BAD_ID_VALUE_)
    {
        if (data_units_[unit_2_verify.next].prev != unit_id)
        {
            return LIST_WRONG_PREV_ID_UNIT;
        }

        if (data_units_[unit_2_verify.prev].next != unit_id)
        {
            return LIST_WRONG_NEXT_ID_UNIT;
        }

        return LIST_BODY_UNIT;
    }

    /* head unit found */
    if (unit_2_verify.next != BAD_ID_VALUE_)
    {
        if (data_units_[unit_2_verify.next].prev != unit_id)
        {
            return LIST_WRONG_PREV_ID_UNIT;
        }

        return LIST_HEAD_UNIT;
    }

    /* tail unit found */
    if (unit_2_verify.prev != BAD_ID_VALUE_)
    {
        if (data_units_[unit_2_verify.prev].next != unit_id)
        {
            return LIST_WRONG_NEXT_ID_UNIT;
        }

        return LIST_TAIL_UNIT;
    }
    
    /* only one busy unit */
    if (data_tail_ == data_head_)
    {
        return LIST_HEAD_UNIT;
    }

    return LIST_UNDF_UNIT;
}