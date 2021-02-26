
#include "akin.h"

tree_t* akin_get_treep( const akin_t* const akin )
{
    return (tree_t* )((size_t)akin + AKIN_TR_SHT_);
}

PARSED_FILE_t* akin_get_filep( const akin_t* const akin )
{
    return (PARSED_FILE_t* )((size_t)akin + AKIN_PF_SHT_);
}

err_t akin_ctor( akin_t* const akin, const char* const  dump_file_name )
{
    if (isBadPtr (akin) || akin->buffs_ != nullptr || akin->buff_shift_ != 0 || akin->curr_buff_id_ != 1)
    {
        return BAD_INIT_ERR;
    }

    err_t error = OK;

    if (tree_ctor (akin_get_treep (akin), &error, dump_file_name))
    {
        return error;
    }

    akin->buffs_ = (char** )calloc (sizeof (char**), INIT_NUMOF_BUFFS_);
    if (akin->buffs_ == nullptr)
    {
        return MEM_ERR;
    }

    akin->buffs_[1] = (char* )calloc (sizeof (char) * BUFF_ALL_MUL_, MAX_STR_LEN_);
    if (akin->buffs_[1] == nullptr)
    {
        free (akin->buffs_);
        akin->buffs_ = nullptr;
        return MEM_ERR;
    }

    akin->buff_shift_ += sprintf (akin->buffs_[1], DEF_ANS_) + 1;
    tree_set_data (akin_get_treep (akin), akin_get_treep (akin)->root_, akin->buffs_[1], &error);

    return error;
}

err_t akin_dstr( akin_t* const akin ) 
{
    verificator();

    err_t error = OK;

    if (tree_dstr(akin_get_treep (akin), &error))
    {
        return error;
    }

    /* free buffs */
    if (!isBadPtr (akin->buffs_))
    {
        for (size_t buff_id = 0; buff_id <= akin->curr_buff_id_; buff_id++)
        {
            if (!isBadPtr(akin->buffs_[buff_id]))
            {
                free (akin->buffs_[buff_id]);
            }

            akin->buffs_[buff_id] = nullptr;
        }

        free (akin->buffs_);
        akin->buffs_ = nullptr;
    }

    /* free strs ptrs */
    if (!isBadPtr(akin->file_.strs))
    {
        free (akin->file_.strs);
        akin->file_.strs = nullptr;
    }

    return OK;
}

/* method to start akinator session */
err_t akin_menu( akin_t* const akin )
{
    verificator();

    printf ("\n\n" "THIS IS THE AKINATOR GAME!" "\n\n");

    char curr_char = '\0';
    err_t error = OK;

    while (true)
    {
        if (error)
        {
            fprintf (stderr, "\n\n" "error code:");
            error_prt (error);
        }

        printf ("Choose what to do:\n\n"
                "1 - play the game round\n\n"
                "2 - get descr for smthn\n\n"
                "3 - compare objects\n\n"
                "4 - dump the tree\n\n"
                "5 - print in file\n\n"
                "6 - print in file and exit the game\n\n"
                "7 - clear tree\n\n"
                "8 - scan new tree form file\n\n"
                "9 - exit the game\n\n");

        scanf (" %c", &curr_char);

        switch (curr_char)
        {
            case '1':
            {
                printf ("\n" "y - yes\n"
                             "n - no\n");
                error = akin_play_round (akin);
                break;
            }

            case '2':
            {
                error = akin_describe (akin);
                break;
            }

            case '3':
            {
                error = akin_compare (akin);
                break;
            }

            case '4':
            {
                tree_dump (akin_get_treep (akin));
                break;
            }
            
            case '5':
            {
                error = akin_get_name_n_write_2_file (akin);

                break;
            }

            case '6':
            {
                error = akin_get_name_n_write_2_file (akin);

                if (error)
                {
                    break;
                }

                return OK;
            }

            case '7':
            {
                if (!get_confirmation())
                {
                    error = akin_clear_tree (akin);
                }

                break;
            }

            case '8':
            {
                if (!get_confirmation())
                {
                    printf ("enter the file name:");
                    char file_name[MAX_STR_LEN_] = "\0";

                    get_str (file_name, MAX_STR_LEN_);
                    error = akin_upload_new (akin, file_name);
                }

                break;
            }

            case '9':
            {
                if (!get_confirmation())
                {
                    return OK;
                }

                break;
            }

            default:
            {
                printf ("\n\n" "unknown cmd!!!" "\n\n");
                break;
            }
        }
    }
}

err_t akin_play_round( akin_t* const akin )
{
    verificator ();

    /* curr user cmd */
    char curr_char = '\0';
    /* for errors output */
    err_t error = OK;
    /* ptr to tree */
    tree_t* treep = akin_get_treep (akin);
    /* node that is active now */
    node_t* curr_node = treep->root_;

    while (true)
    {
        data_t data = tree_get_data (treep, curr_node, &error);

        /* error check */
        if (error != OK) return error;

        /* asking question */
        printf ("\n\n" "%s?" "\n\n", data);

        /* getting user ans */
        scanf(" %c", &curr_char);

        switch (curr_char)
        {
            /* there was true statement */
            case 'y':
            case 'Y':
            {
                if (tree_get_righ (treep, curr_node, &error) == nullptr)
                {
                    printf ("\n\n" "I WON, YEEEEEEEEES!!!!" "\n\n");
                    return OK;
                }
                else
                {
                    curr_node = tree_get_righ (treep, curr_node, &error);
                    break;
                }
            }

            /* there was false statement */
            case 'n':
            case 'N':
            {
                if (tree_get_left (treep, curr_node, &error) == nullptr)
                {
                    printf ("\n\n" "add new object please!" "\n\n");
                    return akin_add_nodes (akin, curr_node);
                }
                else
                {
                    curr_node = tree_get_left (treep, curr_node, &error);
                    break;
                }
            }

            default:
            {
                printf ("\n\n" "unknown cmd!!!" "\n\n");
                break;
            }
        }
    }
}

err_t akin_add_nodes( akin_t* const akin, node_t* const nodep )
{
    verificator ();

    err_t error = OK;

    /* tree ptr */
    tree_t* treep = akin_get_treep (akin);

    /* attaching left node */
    node_t* left  = tree_add (treep, nodep, TREE_LEFT_, &error);
    if (error) return error;
    
    /* attaching righ node */
    node_t* righ  = tree_add (treep, nodep, TREE_RIGH_, &error);
    if (error) return error;

    /* no error -> valid ptrs */
    assert (!isBadPtr(left) && !isBadPtr(righ));

    /* data form parent node is in left child node now */
    error = tree_set_data (treep, left, tree_get_data (treep, nodep, &error), &error);

    /* there are no more space */
    if (akin->buff_shift_ > MAX_STR_LEN_ * (BUFF_ALL_MUL_ - 2))
    {
        if (akin->curr_buff_id_ >= akin->numof_buffs_)
        {
            /* reallocating */
            char** new_buffs_ = (char** )realloc (akin->buffs_, sizeof (char**) * 2 * akin->numof_buffs_);
            if (new_buffs_ == nullptr)
            {
                return MEM_ERR;
            }

            akin->buffs_ = new_buffs_;
            akin->numof_buffs_ *= 2;
        }

        akin->curr_buff_id_++;

        /* allocating new buff */
        akin->buffs_[akin->curr_buff_id_] = (char* )calloc (sizeof (char), MAX_STR_LEN_ * BUFF_ALL_MUL_);
        if (akin->buffs_[akin->curr_buff_id_] == nullptr)
        {
            return MEM_ERR;
        }

        akin->buff_shift_ = 0;
    }

    char* data_to_set = akin->buffs_[akin->curr_buff_id_] + akin->buff_shift_;

    printf ("\n\ninput new criteria: ");
    tree_set_data (treep, nodep, data_to_set, &error);
    akin->buff_shift_ += get_str (akin->buffs_[akin->curr_buff_id_] + akin->buff_shift_, MAX_STR_LEN_);

    data_to_set = akin->buffs_[akin->curr_buff_id_] + akin->buff_shift_;

    printf ("\n\nprint new answer: ");
    tree_set_data (treep, righ, data_to_set, &error);
    akin->buff_shift_ += get_str (akin->buffs_[akin->curr_buff_id_] + akin->buff_shift_, MAX_STR_LEN_);

    return OK;
}

err_t akin_upload_new (akin_t* akin, const char* scan_file_name)
{
    verificator();

    akin_clear_tree (akin);

    /* file buff */
    get_parsed_text (akin_get_filep(akin), scan_file_name);
    if (akin->file_.data == nullptr)
    {
        return FILE_ERR;
    }

    akin->buffs_[0] = akin->file_.data;

    if (akin->file_.num_of_c)
    {
        /* recursion func call */
        int error = akin_sub_t_scan (akin, akin->tree_.root_, 0);

        if (error < 0)
        {
            return (err_t)error;
        }

        /* ends of strs */
        for (size_t char_id = 0; char_id < akin->file_.num_of_c; char_id++)
        {
            if (akin->file_.data[char_id] == '?' || akin->file_.data[char_id] == '`')
            {
                akin->file_.data[char_id] = 0;
            }
        }

        /* don't need this ptrs now */
        free (akin->file_.strs);
        akin->file_.strs = nullptr;
    }

    return OK;
}

err_t akin_clear_tree( akin_t* const akin )
{
    verificator ();

    err_t error = OK;

    tree_t* treep = akin_get_treep (akin);

    /* there are smth in tree */
    if (akin->tree_.size_ > 1)
    {
        node_t* left = tree_get_left (treep, akin->tree_.root_, &error);
        node_t* righ = tree_get_righ (treep, akin->tree_.root_, &error);

        if (left != nullptr)
        {
            tree_del_subt (treep, left, &error);
        }

        if (righ != nullptr)
        {
            tree_del_subt (treep, righ, &error);
        }
    }

    /* clearing file buff */
    if (akin->buffs_[0] != nullptr)
    {
        free (akin->buffs_[0]);
        akin->buffs_[0] = nullptr;
    }

    /* resetting counters */
    akin->curr_buff_id_ = 1;
    akin->buff_shift_ = 0;

    /* adding def ansver */
    akin->buff_shift_ += sprintf (akin->buffs_[1], DEF_ANS_) + 1;
    tree_set_data (treep, treep->root_, akin->buffs_[1], &error);

    return error;
}

int akin_sub_t_scan( akin_t* const akin, node_t* const nodep, int str_id )
{
    if (isBadPtr (akin) || isBadPtr (nodep))
    {
        return BAD_PTR;
    }

    tree_t* treep = akin_get_treep (akin);
    err_t error = OK;

    /* can't finde node end */
    if (str_id >= akin->file_.num_of_s || str_id < 0) return SYNT_ERR;

    /* str with data and next str that should be with with bracket */
    STR curr_str = akin->file_.strs[++str_id];

    /* ptr to ptr to massive with data */
    char* data_to_set = curr_str.str + 1;

    /* obj found */
    if (curr_str.str[0] == '`')
    {
        /* data is written to node */
        tree_set_data (treep, nodep, data_to_set, &error);

        /* searching for brace */
        int br_pose = (int)strstr (curr_str.str, "]");

        if (!br_pose || br_pose - (size_t)curr_str.str > curr_str.length)
        {
            return SYNT_ERR;
        }

        /* returning error or str id */
        return error ? error : str_id;
    }

    /* question found */
    else if (curr_str.str[0] == '?')
    {
        /* data is written to node */
        tree_set_data (treep, nodep, data_to_set, &error);

        node_t* left = tree_add (treep, nodep, TREE_LEFT_, &error);
        node_t* righ = tree_add (treep, nodep, TREE_RIGH_, &error);

        if (error) return error;

        /* scanning sub trees */
        str_id = akin_sub_t_scan (akin, righ, str_id);
        if (str_id < 0)
        {
            return str_id;
        }

        str_id = akin_sub_t_scan (akin, left, str_id);
        if (str_id < 0)
        {
            return str_id;
        }

        STR next_str =  akin->file_.strs[str_id];

        int br_pose = (size_t)strstr (next_str.str, "]");
        if (!br_pose || br_pose - (size_t)next_str.str > next_str.length)
        {
            return SYNT_ERR;
        }

        return str_id;
    }

    return SYNT_ERR;
}

err_t akin_get_name_n_write_2_file( const akin_t* const akin )
{
    printf ("\n\n" "input file name:");
    char file_name[MAX_STR_LEN_] = "\0";
    get_str (file_name, MAX_STR_LEN_);

    if (akin_print (akin, file_name))
    {
        printf ("\n\n" "error while printing file data!");
        return FILE_ERR;
    }

    return OK;
}

err_t akin_print( const akin_t* const akin, const char* const print_file_name )
{
    verificator();

    /* max size of prt str about one data unit */
    static const int MAX_PRT_STR_LEN_ = 200;

    tree_t* treep = akin_get_treep (akin);

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * MAX_PRT_STR_LEN_, treep->size_ + 1);
    if (buff == nullptr)
    {
        printf ("\n\nNO MEMORY FOR PRINT BUFFER!\n\n");
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info about graph */
    buff_shift += sprintf (buff + buff_shift, "{vasiliy base}\n");

    /* calling recoursion method for sub trees */
    int error = akin_sub_t_print (treep, treep->root_, buff + buff_shift);
    if (error < 0)
    {
        return (err_t)error;
    }

    buff_shift += error;

    FILE* out_file = fopen (print_file_name, "wb");
    if (out_file == nullptr)
    {
        free (buff);
        return err_t::MEM_ERR;
    }

    /* writing buff */
    fwrite (buff, sizeof (char), buff_shift, out_file);
    free (buff);
    fclose (out_file);

    return err_t::OK;
}

int akin_sub_t_print( const tree_t* const treep, const node_t* const nodep, char* const buff )
{
    if (isBadPtr (treep) || isBadPtr (nodep))
    {
        return BAD_PTR;
    }

    int buff_shift = 0;

    err_t error = OK;

    int     depth   = tree_get_dept (treep, nodep, &error);
    data_t  data    = tree_get_data (treep, nodep, &error);

    if (error) return error;

    /* [ */
    buff_shift += sprintf (buff + buff_shift, "%*c\n", depth * 2, '[');

    /* there are doughters */
    if (tree_get_left (treep, nodep, &error))
    {

        /* quest syntaxis */
        buff_shift += sprintf (buff + buff_shift, "%*c%s?\n", depth * 2, '?', data);

        /* calling this method for sub trees */
        int rec_out = 0;

        rec_out = akin_sub_t_print (treep, tree_get_righ (treep, nodep, &error), buff + buff_shift);
        if (rec_out < 0)
        {
            return rec_out;
        }
        buff_shift += rec_out;

        rec_out = akin_sub_t_print (treep, tree_get_left (treep, nodep, &error), buff + buff_shift);
        if (rec_out < 0)
        {
            return rec_out;
        }

        buff_shift += rec_out;
    }

    else
    {
        /* obj syntaxis */
        buff_shift += sprintf (buff + buff_shift, "%*c%s`\n", depth * 2, '`', data);
    }

    /* ] */
    buff_shift += sprintf (buff + buff_shift, "%*c\n", depth * 2, ']');

    return buff_shift;
}

err_t akin_describe( const akin_t* const akin )
{
    printf ("\n\n" "input name:    ");
    char name[MAX_STR_LEN_] = "\0";
    tree_t* treep = akin_get_treep (akin);
    err_t error = OK;

    get_str (name, MAX_STR_LEN_);
    descr_t node_descr = {};

    error = akin_form_descr (akin, &node_descr, name);

    if (error)
    {
        return error;
    }

    if (node_descr.true_prop_found)
    {
        printf ("\n" "%s is:    ", tree_get_data (treep, node_descr.node_, &error));

        for (int prop_id = DESCR_MAX_SIZE_ - 1; prop_id >= 0; prop_id--)
        {
            if (node_descr.prop_mask_[prop_id])
            {
                printf ("%s  ", node_descr.properties_[prop_id]);
            }
        }
    }

    if (node_descr.fals_prop_found)
    {
        if (node_descr.true_prop_found)
        {
            printf ("\n" "But, is not:\t");
        }
        else
        {
            printf ("%s is not:\t", tree_get_data (treep, node_descr.node_, &error));
        }
    }

    for (int prop_id = DESCR_MAX_SIZE_ - 1; prop_id >= 0; prop_id--)
    {
        if (!node_descr.prop_mask_[prop_id])
        {
            if (node_descr.properties_[prop_id] != nullptr)
            {
                printf ("%s  ", node_descr.properties_[prop_id]);
            }
        }
    }

    printf ("\n\n");

    return OK;
}

err_t akin_form_descr( const akin_t* const akin, descr_t* const descr, const char* const name )
{
    verificator ();

    tree_t* treep = akin_get_treep (akin);
    err_t error = OK;

    node_t* nodep = tree_slow_search_for_data (treep, (data_t*)&name, &error);
    descr->node_ = nodep;

    if (nodep == nullptr)
    {
        return BAD_PTR;
    }

    if (tree_get_left (treep, nodep, &error) != nullptr)
    {
        return LEAF_ERR;
    }

    node_t* parent = nullptr;

    /* skipping to depth properties */
    while (tree_get_dept (treep, nodep, &error) >= DESCR_MAX_SIZE_)
    {
        nodep = tree_get_prnt (treep, nodep, &error);
    }

    int dept = tree_get_dept (treep, nodep, &error);

    /* getting properties */
    for (int prop_id = 0; prop_id < dept; prop_id++)
    {
        parent = tree_get_prnt (treep, nodep, &error);

        descr->properties_[prop_id] = tree_get_data (treep, parent, &error);

        /* wrong */
        if (tree_get_left (treep, parent, &error) == nodep)
        {
            descr->fals_prop_found = true;
            descr->prop_mask_ [prop_id] = false;
        }

        /* true */
        else
        {
            descr->true_prop_found = true;
            descr->prop_mask_ [prop_id] = true;
        }

        nodep = parent;
    }

    return OK;
}

err_t akin_compare( const akin_t* const akin )
{
    descr_t ft_descr = {};
    descr_t sd_descr = {};
    err_t error = OK;

    tree_t* treep = akin_get_treep (akin);

    char name[MAX_STR_LEN_] = "\0";

    /* descr for compar nodes */
    printf ("\n\n" "input first name:    ");
    get_str (name, MAX_STR_LEN_);
    error = akin_form_descr (akin, &ft_descr, name);
    if ((bool)error) return error;

    printf ("\n\n" "input second name:    ");
    get_str (name, MAX_STR_LEN_);
    error = akin_form_descr (akin, &sd_descr, name);
    if ((bool)error) return error;

    printf ("\n\n");

    bool flag_prop = false;

    /* define to printf correct properties */
    #define PRINT_PROPS( condition, descr, to_prt )                                     \
    for (int prop_id = DESCR_MAX_SIZE_ - 1; prop_id >= 0; prop_id--)                    \
        {                                                                               \
            if (condition)                                                              \
            {                                                                           \
                if (!flag_prop)                                                         \
                {                                                                       \
                    printf (to_prt, tree_get_data (treep, descr.node_, &error));        \
                    flag_prop = true;                                                   \
                }                                                                       \
                                                                                        \
                printf ("%s ", descr.properties_[prop_id]);                             \
            }                                                                           \
        }


    /* first obj properties */
    if (ft_descr.true_prop_found)
    {
        PRINT_PROPS(ft_descr.prop_mask_[prop_id] && (!sd_descr.prop_mask_[prop_id] || (ft_descr.properties_[prop_id] != sd_descr.properties_[prop_id])),
                    ft_descr, "\n" "%s is\t\t");

        if (flag_prop)
        {
            printf("\n" "But %s is not" "\n\n", tree_get_data (treep, sd_descr.node_, &error));
        }
    }

    flag_prop = false;

    /* second obj properties */
    if (sd_descr.true_prop_found)
    {
        PRINT_PROPS(sd_descr.prop_mask_[prop_id] && (!ft_descr.prop_mask_[prop_id] || (ft_descr.properties_[prop_id] != sd_descr.properties_[prop_id])),
                    sd_descr, "%s is\t\t");

        if (flag_prop)
        {
            printf("\n" "But %s is not" "\n\n", tree_get_data (treep, ft_descr.node_, &error));
        }
    }

    flag_prop = false;// сначала сходства, потом различия  ?в другом порядке?

    /* same properties */
    PRINT_PROPS(ft_descr.properties_[prop_id] == sd_descr.properties_[prop_id] && ft_descr.prop_mask_[prop_id] && sd_descr.prop_mask_[prop_id],
                ft_descr, "And they both are\t");

    #undef PRINT_PROPS()

    printf ("\n\n");

    return OK;
}