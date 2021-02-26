#include "akinator.h"

akinator_t::akinator_t( const char const*  dump_file_name ) : data_tree_(dump_file_name)
{
    buffs_ = (char** )calloc (sizeof (char**), initial_buff_num);
    if (buffs_ == nullptr)
    {
        return;
    }

    buffs_[1] = (char* )calloc (buff_alloc_multiplayer, max_akin_str_len_);
    if (buffs_[1] == nullptr)
    {
        free (buffs_);
        buffs_ = nullptr;
        return;
    }

    buff_shift_ += sprintf (buffs_[1], default_ans) + 1;
    data_tree_.set_data(data_tree_.get_root(), &(buffs_[1]));
}

akinator_t::~akinator_t() 
{
    /* free buffs */
    if (!isBadReadPtr (buffs_))
    {
        for (size_t buff_id = 0; buff_id <= curr_buff_id; buff_id++)
        {
            if (!isBadReadPtr(buffs_[buff_id]))
            {
                free (buffs_[buff_id]);
            }

            buffs_[buff_id] = nullptr;
        }

        free(buffs_);
        buffs_ = nullptr;
    }

    /* free strs ptrs */
    if (!isBadReadPtr(file_.strs))
    {
        free (file_.strs);
        file_.strs = nullptr;
    }
}

err_t akinator_t::menu()
{
    verificator();

    printf ("\n\n" "THIS IS THE AKINATOR GAME!" "\n\n");

    char curr_char = '\0';
    err_t error = err_t::OK;

    while (true)
    {
        if ((bool)error)
        {
            fprintf (stderr, "\n\n" "error code:%d" "\n\n", (int)error);
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
                error = play_round ();
                break;
            }

            case '2':
            {
                error = describe();
                break;
            }

            case '3':
            {
                error = compare();
                break;
            }

            case '4':
            {
                data_tree_.dump ();
                break;
            }
            
            case '5':
            {
                error = get_name_n_write_2_file ();

                break;
            }

            case '6':
            {
                error = get_name_n_write_2_file();

                if (!(bool)error)
                {
                    break;
                }

                return err_t::OK;
            }

            case '7':
            {
                error = get_confirmation();

                if (!(bool)error)
                {
                    clear_tree ();
                }

                break;
            }

            case '8':
            {
                error = get_confirmation();

                if (!(bool)error)
                {
                    printf ("enter the file name:");
                    char file_name[max_akin_str_len_] = "\0";

                    get_str (stdin, file_name, max_akin_str_len_);
                    upload_new (file_name);
                }

                break;
            }

            case '9':
            {
                error = get_confirmation ();

                if (!(bool)error)
                {
                    return err_t::OK;
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

err_t akinator_t::get_confirmation()
{
    char ch = '\0';

    printf ("\n\n" "are you shure?" "\n\n" "y - yes\n");

    /* getting the confirmation */
    scanf (" %c", &ch);
    if (ch == 'y' || ch == 'Y')
    {
        return err_t::OK;
    }

    return err_t::CONF_ERR;
}

err_t akinator_t::upload_new (const char* scan_file_name)
{
    clear_tree();

    /* file buff */
    get_parsed_text(&file_, scan_file_name);
    if (file_.data == nullptr)
    {
        return err_t::MEM_ERR;
    }

    buffs_[0] = file_.data;

    if (file_.num_of_c)
    {
        /* recursion func call */
        int error = sub_tree_scan (data_tree_.get_root(), 0);

        if (error < 0)
        {
            return (err_t)error;
        }

        /* ends of strs */
        for (size_t char_id = 0; char_id < file_.num_of_c; char_id++)
        {
            if (file_.data[char_id] == '?' || file_.data[char_id] == '`')
            {
                file_.data[char_id] = 0;
            }
        }

        /* don't need this ptrs now */
        free (file_.strs);
        file_.strs = nullptr;
    }
}

int akinator_t::sub_tree_scan( node_ptr<data_t> root, int str_id )
{
    if (root.isBad())
    {
        return (int)err_t::BAD_PTR;
    }

    /* can't finde node end */
    if (str_id >= file_.num_of_s || str_id < 0) return (int)err_t::SYNT_ERR;

    /* str with data and next str that should be with with bracket */
    STR curr_str = file_.strs[++str_id];

    /* ptr to ptr to massive with data */
    char* data_to_set = curr_str.str + 1;

    /* obj found */
    if (curr_str.str[0] == '`')
    {
        /* data is written to node */
        data_tree_.set_data (root, &data_to_set);

        err_chk (int);

        /* searching for brace */
        int br_pose = (int)strstr (curr_str.str, "]");

        if (!br_pose || br_pose - (size_t)curr_str.str > curr_str.length)
        {
            return (int)err_t::SYNT_ERR;
        }

        return str_id;
    }

    /* question found */
    else if (curr_str.str[0] == '?')
    {
        /* data is written to node */
        data_tree_.set_data (root, &data_to_set);

        data_tree_.add_left (root);
        data_tree_.add_right(root);

        err_chk (int);
        
        /* scanning sub trees */
        str_id = sub_tree_scan (data_tree_.get_right (root), str_id);
        if (str_id < 0)
        {
            return str_id;
        }

        str_id = sub_tree_scan (data_tree_.get_left (root) , str_id);
        if (str_id < 0)
        {
            return str_id;
        }

        STR next_str =  file_.strs[str_id];

        int br_pose = (size_t)strstr (next_str.str, "]");
        if (!br_pose || br_pose - (size_t)next_str.str > next_str.length)
        {
            return (int)err_t::SYNT_ERR;
        }

        return str_id;
    }

    return (int)err_t::SYNT_ERR;
}

err_t akinator_t::print( const char* const print_file_name )
{
    verificator();

    /* max size of prt str about one data unit */
    static const int max_prt_str_len = 200;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * max_prt_str_len, data_tree_.get_size() + 1);
    if (buff == nullptr)
    {
        err_chk (err_t);
        printf ("\n\nNO MEMORY FOR PRINT BUFFER!\n\n");
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info about graph */
    buff_shift += sprintf (buff + buff_shift, "{vasiliy base}\n");

    /* calling recoursion method for sub trees */
    int error = sub_tree_print (data_tree_.get_root(), buff + buff_shift);
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

int akinator_t::sub_tree_print( node_ptr<data_t> root, char* buff )
{
    int buff_shift = 0;

    /* [ */
    buff_shift += sprintf (buff + buff_shift, "%*c\n", data_tree_.get_depth (root) * 2, '[');

    /* there are doughters */
    if (!data_tree_.get_left(root).isBad())
    {
        int i = data_tree_.get_depth(root) * 2;
        char* j = *data_tree_.get_data(root);

        /* quest syntaxis */
        buff_shift += sprintf (buff + buff_shift, "%*c%s?\n", data_tree_.get_depth (root) * 2, '?', *data_tree_.get_data (root));

        /* calling this method for sub trees */
        int error = 0;

        error = sub_tree_print (data_tree_.get_right (root), buff + buff_shift);
        if (error < 0)
        {
            return error;
        }
        buff_shift += error;

        error = sub_tree_print (data_tree_.get_left (root), buff + buff_shift);
        if (error < 0)
        {
            return error;
        }

        buff_shift += error;
    }

    else
    {
        /* obj syntaxis */
        buff_shift += sprintf (buff + buff_shift, "%*c%s`\n", data_tree_.get_depth (root) * 2, '`', *data_tree_.get_data (root));
    }

    /* ] */
    buff_shift += sprintf (buff + buff_shift, "%*c\n", data_tree_.get_depth (root) * 2, ']');

    err_chk (int);

    return buff_shift;
}

err_t akinator_t::clear_tree ()
{
    /* there are smth in tree */
    if (data_tree_.get_size () > 1)
    {
        data_tree_.del_sub_tree (data_tree_.get_right (data_tree_.get_root ()));
        data_tree_.del_sub_tree (data_tree_.get_left  (data_tree_.get_root ()));
    }

    err_chk (err_t);

    /* clearing file buff */
    if (buffs_[0] != nullptr)
    {
        free (buffs_[0]);
        buffs_[0] = nullptr;
    }

    /* resetting counters */
    curr_buff_id = 1;
    buff_shift_ = 0;

    /* adding def ansver */
    buff_shift_ += sprintf (buffs_[1], default_ans) + 1;
    data_tree_.set_data(data_tree_.get_root(), &(buffs_[1]));

    err_chk (err_t);

    return err_t::OK;
}

err_t akinator_t::describe()
{
    printf ("\n\n" "input name:");
    char* name = (char* )calloc (sizeof (char), max_akin_str_len_);

    get_str (stdin, name, max_akin_str_len_);
    descr_t node_descr = {};

    err_t error = form_descr (node_descr, name);
    free (name);

    if ((bool)error)
    {
        return error;
    }

    if (node_descr.true_prop_found)
    {
        printf ("%s is:\t", *data_tree_.get_data (node_descr.node_));

        for (int prop_id = descr_max_size_ - 1; prop_id >= 0; prop_id--)
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
            printf ("%s is not:\t", *data_tree_.get_data (node_descr.node_));
        }
    }

    for (int prop_id = descr_max_size_ - 1; prop_id >= 0; prop_id--)
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
}

err_t akinator_t::compare()
{
    descr_t ft_descr = {};
    descr_t sd_descr = {};
    err_t error = err_t::OK;

    char* name = (char* )calloc (sizeof (char), max_akin_str_len_);
    if (name == nullptr)
    {
        return err_t::MEM_ERR;
    }

    /* descr for compar nodes */
    printf ("\n\n" "input first name:");
    get_str (stdin, name, max_akin_str_len_);
    error = form_descr (ft_descr, name);
    if ((bool)error) { free(name);  return error; }

    printf ("\n\n" "input second name:");
    get_str (stdin, name, max_akin_str_len_);
    error = form_descr (sd_descr, name);
    free(name);
    if ((bool)error) { return error; }

    printf ("\n\n");

    bool flag_prop = false;

    if (ft_descr.true_prop_found)
    {
        for (int prop_id = descr_max_size_ - 1; prop_id >= 0; prop_id--)
        {
            if (ft_descr.prop_mask_[prop_id] && (!sd_descr.prop_mask_[prop_id] || (ft_descr.properties_[prop_id] != sd_descr.properties_[prop_id])))
            {
                if (!flag_prop)
                {
                    printf ("%s is:\t", *data_tree_.get_data (ft_descr.node_));
                    flag_prop = true;
                }

                printf ("%s  ", ft_descr.properties_[prop_id]);
            }
        }

        if (flag_prop)
        {
            printf("\n" "But %s is not" "\n\n", *data_tree_.get_data(sd_descr.node_));
        }
    }

    flag_prop = false;

    if (sd_descr.true_prop_found)
    {

        for (int prop_id = descr_max_size_ - 1; prop_id >= 0; prop_id--)
        {
            if (sd_descr.prop_mask_[prop_id] && (!ft_descr.prop_mask_[prop_id] || (ft_descr.properties_[prop_id] != sd_descr.properties_[prop_id])))
            {
                if (!flag_prop)
                {
                    printf ("%s is:\t", *data_tree_.get_data (sd_descr.node_));
                    flag_prop = true;
                }

                printf ("%s  ", sd_descr.properties_[prop_id]);
            }
        }

        if (flag_prop)
        {
            printf("\n" "But %s is not" "\n\n", *data_tree_.get_data(sd_descr.node_));
        }
    }

    if (sd_descr.true_prop_found && ft_descr.true_prop_found)
    {
        for (int prop_id = descr_max_size_ - 1; prop_id >= 0; prop_id--)
        {
            if (ft_descr.properties_[prop_id] == sd_descr.properties_[prop_id] && ft_descr.prop_mask_[prop_id] && sd_descr.prop_mask_[prop_id])
            {
                if (!flag_prop)
                {
                    printf ("And they both are:\t");
                    flag_prop = true;
                }

                printf ("%s  ", sd_descr.properties_[prop_id]);
            }
        }
    }

    printf ("\n\n");
}

err_t akinator_t::form_descr( descr_t& descr, char* name )
{
    node_ptr<data_t> node = data_tree_.slow_search_for_data (&name);
    descr.node_ = node;

    if (node.isBad())
    {
        return err_t::BAD_PTR;
    }

    if (!data_tree_.get_left(node).isBad())
    {
        return err_t::LEAF_ERR;
    }

    node_ptr<data_t> father;

    /* skipping to depth properties */
    while (data_tree_.get_depth (node) >= descr_max_size_)
    {
        node = data_tree_.get_fath (node);
        err_chk (err_t);
    }

    /* getting properties */
    for (int prop_id = data_tree_.get_depth (node) - 1; prop_id >= 0; prop_id--)
    {
        father = data_tree_.get_fath (node);

        descr.properties_[prop_id] = *data_tree_.get_data(father);

        /* wrong */
        if (data_tree_.get_left(father) == node)
        {
            descr.fals_prop_found = true;
            descr.prop_mask_ [prop_id] = false;
        }

        /* true */
        else
        {
            descr.true_prop_found = true;
            descr.prop_mask_ [prop_id] = true;
        }

        node = father;
    }

    err_chk (err_t);

    return err_t::OK;
}

err_t akinator_t::get_name_n_write_2_file()
{
    printf ("\n\n" "input file name:");
    char file_name[max_akin_str_len_] = "\0";
    get_str (stdin, file_name, max_akin_str_len_);

    if ((bool)print (file_name))
    {
        printf ("\n\n" "error while printing file data!");
        return err_t::FILE_ERR;
    }

    return err_t::OK;
}

err_t akinator_t::play_round()
{
    verificator ();

    char curr_char = '\0';

    err_t error = err_t::OK;

    /* node that is active now */
    node_ptr<data_t> curr_node = (data_tree_.get_root());
    err_chk (err_t);

    while (true)
    {
        /* asking question */
        printf ("\n\n" "%s?" "\n\n", *(data_tree_.get_data (curr_node)));
        err_chk (err_t);

        scanf(" %c", &curr_char);

        switch (curr_char)
        {
            case 'y':
            case 'Y':
            {
                if (data_tree_.get_right (curr_node).isBad())
                {
                    err_chk (err_t);
                    printf ("\n\n" "I WON, YEEEEEEEEES!!!!" "\n\n");
                    return err_t::OK;
                }
                else
                {
                    curr_node = data_tree_.get_right (curr_node);
                    err_chk (err_t);
                    break;
                }
            }

            case 'n':
            case 'N':
            {
                if (data_tree_.get_left (curr_node).isBad())
                {
                    err_chk (err_t);
                    printf ("\n\n" "add new object please!" "\n\n");
                    return add_nodes (curr_node);
                }
                else
                {
                    curr_node = data_tree_.get_left (curr_node);
                    err_chk (err_t);
                    break;
                }
            }

            default:
            {
                printf ("\n\n" "unknown cmd!!!" "\n\n");
            }
        }
    }
}

err_t akinator_t::add_nodes( node_ptr<data_t> const node )
{
    static size_t buff_num = initial_buff_num;

    /* cant attach new node */
    node_ptr<data_t> left  = data_tree_.add_left (node);

    if (left.isBad())
    {
        err_chk (err_t);
        fprintf (stderr, "\n\n" "UNDEF SITUATION" "\n\n");
        return err_t::OK;
    }
    
    /* cant attach new node */
    node_ptr<data_t> right =  data_tree_.add_right (node);
    if (right.isBad())
    {
        err_chk (err_t);
        fprintf (stderr, "\n\n" "UNDEF SITUATION" "\n\n");
        return err_t::OK;
    }

    /* data form father node is in left node now */
    data_tree_.set_data (left, data_tree_.get_data (node));
    err_chk (err_t);

    /* there are no more space */
    if (buff_shift_ > max_akin_str_len_ * (buff_alloc_multiplayer - 2))
    {
        if (curr_buff_id >= buff_num)
        {
            /* reallocating */
            char** new_buffs_ = (char** )realloc (buffs_, sizeof (char**) * 2 * buff_num);
            if (new_buffs_ == nullptr)
            {
                return err_t::MEM_ERR;
            }

            buffs_ = new_buffs_;
            buff_num *= 2;
        }

        curr_buff_id++;

        /* allocating new buff */
        buffs_[curr_buff_id] = (char* )calloc (sizeof (char), max_akin_str_len_ * buff_alloc_multiplayer);
        if (buffs_[curr_buff_id] == nullptr)
        {
            return err_t::MEM_ERR;
        }

        buff_shift_ = 0;
    }

    char* data_to_set = buffs_[curr_buff_id] + buff_shift_;

    printf ("\n\ninput new criteria: ");
    data_tree_.set_data (node, &data_to_set);
    err_chk (err_t);
    buff_shift_ += get_str (stdin, buffs_[curr_buff_id] + buff_shift_, max_akin_str_len_);

    data_to_set = buffs_[curr_buff_id] + buff_shift_;

    printf ("\n\nprint new answer: ");
    data_tree_.set_data (right, &data_to_set);
    err_chk (err_t);
    buff_shift_ += get_str (stdin, buffs_[curr_buff_id] + buff_shift_, max_akin_str_len_);

    return err_t::OK;
}

size_t akinator_t::get_str( FILE* frame, char* buff, size_t max_len )
{
    size_t char_id = 0;

    /* getting str from sended frame until succsess */
    while (true)
    {
        /* skipping trash */
        fscanf(frame, " ");

        /* scaning symbols */
        for (; char_id < max_len; char_id++)
        {
            fscanf (frame, "%c", buff + char_id);

            /* end of str found */
            if (*(buff + char_id) == '\n')
            {
                break;
            }
        }

        /* str found */
        if (char_id)
        {
            /* zero in the end of str */
            *(buff + char_id) = '\0';

            /* return num of readen symbols */
            return char_id + 1;
        }

        /* stupid user */
        printf("\n\ninput normal str:");

    }
}