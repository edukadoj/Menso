// get info about a kind eg object*(prop)

#ifndef AnalyzingKindH
#define AnalyzingKindH
#include "Common Files\\String.h"
#include "List.h"
//#include "General.h"


class AnalyzingKind
{
    String kind;

    bool is_the_kind_functional;
    // Unsigned last_outer_asterisk_offset; // CheckIfReturnTypeIsFunctional will assign a value to to it; last_outer_asterisk_offset == -1 if no outer asterisks

    Unsigned number_of_parameters;
    List return_and_parameter_list_parts;
    //--------------------------------------------------------------------------
    /*void FillList()
    {
        Unsigned paren_number = 0;
        //parameter_list_parts << kind.CutOff("*"); // returnee
        
        //if (CheckIfReturnTypeIsFunctional(&last_outer_asterisk_offset))
        return_and_parameter_list_parts << kind.CutOff(last_outer_asterisk_offset); // cut off all if last_outer_asterisk_offset == -1
        //else
        //    return_and_parameter_list_parts << kind.CutOff("*");

        Unsigned size = kind.Size();
        Unsigned sign;
        for (Unsigned i = 0; i < size; i++)
        {
            char ch = kind[i];
            switch (ch)
            {
            case '(':
                paren_number++;
                if (paren_number == 1)
                {
                    is_main_type_functional = true;
                    sign = i + 1;
                }
                break;

            case ')':
                paren_number--;
                if (paren_number == 0)
                {
                    number_of_parameters++;
                    return_and_parameter_list_parts << kind[sign][i - 1];
                    sign = i + 1;
                    return;
                }
                break;

            case ',':
                if (paren_number == 1)
                {
                    number_of_parameters++;
                    return_and_parameter_list_parts << kind[sign][i - 1];
                    sign = i + 1;
                }
                break;
            } // switch
        } // for
    }

    void CheckIfReturnTypeIsFunctional() // does returntype have parameters?
    {
        last_outer_asterisk_offset = unsigned2_extreme_end; // last_outer_asterisk_offset == -1 if no outer asterisks

        Unsigned number_of_outer_asterisks = 0;
        Unsigned paren_number = 0;
        Unsigned size = kind.Size();
        for (Unsigned i = 0; i < size; i++)
        {
            char ch = kind[i];
            switch (ch)
            {
            case '(':
                paren_number++;
                break;

            case ')':
                paren_number--;
                break;

            case '*':
                if (paren_number == 0)
                {
                    number_of_outer_asterisks++;
                    last_outer_asterisk_offset = i;
                    if (number_of_outer_asterisks == 2)
                    {
                        is_return_type_functional = true;
                        return; // because there are no more than 2 outer astersisks in a std type
                    }
                }
                break;
            } // switch
        } // for
        is_return_type_functional = false;
    }*/

    //////////////////////////

    Unsigned GetLastOuterAsteriskOffset() // returns -1 if nothing found
    {
        Unsigned ret = MAXUnsigned; // last outer asterisk offset == -1 if no outer asterisks
        Unsigned paren_number = 0;
        Unsigned size = kind.Size();
        for (Unsigned i = 0; i < size; i++)
        {
            char ch = kind[i];
            switch (ch)
            {
            case '(':
                paren_number++;
                break;

            case ')':
                paren_number--;
                break;

            case '*':
                if (paren_number == 0)
                    ret = i;
                break;
            } // switch
        } // for
        return ret;
    }

    void Run()
    {
        Unsigned last_outer_asterisk_offset = GetLastOuterAsteriskOffset();
        if (last_outer_asterisk_offset == MAXUnsigned)
        {
            return_and_parameter_list_parts << kind;
            return;
        }
        is_the_kind_functional = true;

        // kind.Reverse();
        // String last_part = kind.CutOff("*");
        // last_part.Reverse();

        /*if (kind == "")
        {
            return_and_parameter_list_parts << last_part;
            return;
        }
        else
        {
            kind.Reverse();
            return_and_parameter_list_parts << kind;
        }*/
        
        String return_type = kind.CutOff((Unsigned)last_outer_asterisk_offset);
        return_and_parameter_list_parts << return_type;

        Unsigned open_paren_number = 0;
        Unsigned size = kind.Size();
        Unsigned mark = 0;
        for (Unsigned i = 0; i < size; i++)
        {
            char ch = kind[i];
            switch (ch)
            {
            case '(':
                open_paren_number++;
                if (open_paren_number == 1)
                    mark = i + 1;
                break;

            case ')':
                open_paren_number--;
                if (open_paren_number == 0)
                {
                    number_of_parameters++;
                    return_and_parameter_list_parts << kind[mark][i - 1];
                    mark = i + 1;
                    return;
                }
                break;

            case ',':
                if (open_paren_number == 1)
                {
                    number_of_parameters++;
                    return_and_parameter_list_parts << kind[mark][i - 1];
                    mark = i + 1;
                }
                break;
            } // switch
        } // for
    }

public:
    //--------------------------------------------------------------------------
    bool IsFunctional() // e.g. object*(object) is functional but prop is not functional because it does not have parameters
    {
        return is_the_kind_functional;
    }
    //--------------------------------------------------------------------------
    /*bool IsReturnTypeFunctional()
    {
        return is_return_type_functional;
    }*/
    //--------------------------------------------------------------------------
    Unsigned ParameterNumber()
    {
        return number_of_parameters;
    }
    //--------------------------------------------------------------------------
    String nthParameterKind(Unsigned n)
    {
        if (n <= number_of_parameters) // note that return_and_parameter_list_parts[0] is return type and return_and_parameter_list_parts[1] is first parameter
            return return_and_parameter_list_parts[n];
        else
            print_error("bad range for argmunet for nthParameter(Unsigned n)");
        return ""; // to remove return needed warning
    }
    //--------------------------------------------------------------------------
    String ReturnType()
    {
        return return_and_parameter_list_parts[0];
    }
    //--------------------------------------------------------------------------
    AnalyzingKind(String kind) :
        kind(kind), is_the_kind_functional(false), number_of_parameters(0)
    {
        Run();
        /*
#ifdef _DEBUG
        if (kind.IsSuperset("`"))
            print_error("\n" "thing supplied, kind expected");
#endif
        CheckIfReturnTypeIsFunctional();
        FillList();
        */
    }
};

#endif
