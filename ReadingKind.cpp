#pragma hdrstop
#include "ReadingKind.h"

extern void ReportError(const char* c, Unsigned offset);

/*
void
ReadingKind::
Finalize(Unsigned offset)
{
    String* pkind;
    if (parens_found_for_return_type == false)
        pkind = &parameter_list_part_of_kind;
    else
        pkind = &return_type_part_of_kind;

    String current = ML.deco[offset];
    if (open_paren != 0)
    {
        if (current == ",")
        {
            *pkind += ",";
            end = offset;
            GetKind(offset + 1);
        }
        else if (current == ")")
        {
            open_paren--;
            end = offset;
            *pkind += ")";
        }
        else if (current == "*")
        {
            //parens_found_for_main_type = true; // it has parentheses
            String next = ML.deco[offset + 1];
            if(next != "(")
                ReportError("'(' expected", offset);
            //open_paren++;
            parameter_list_part_of_kind += "*(";
            //end = offset + 1;
            GetKind(offset + 2);
        }
        else
            ReportError("',' or ')' or '*' expected", offset);
    }
    else if (current == "(")
    {
        if (parens_found_for_return_type == true)
            ReportError("'(' unexpected", offset);
        parens_found_for_return_type = true;
        open_paren++;
        return_type_part_of_kind += "*(";
        GetKind(offset + 1);
    }
}

void
ReadingKind::
Initialize(Unsigned offset)
{
    String to_be_type = ML.deco[offset];
    if (!ML.types.Contains(to_be_type)) // if it's not a type
        ReportError("a type expected", offset);
    end = offset;

    if (parens_found_for_main_type && !parens_found_for_return_type)
        parameter_list_part_of_kind += to_be_type;
    else
        return_type_part_of_kind += to_be_type;

    String next = ML.deco[offset + 1];
    String nextnext = ML.deco[offset + 2];
    if (next == "(")
    {
        parens_found_for_main_type = true; // it has parentheses
        open_paren++;
        parameter_list_part_of_kind += "*(";
        end = offset + 1;
        GetKind(offset + 2);
    }
    else if (next != ")" && nextnext == "(")
    {
        parens_found_for_main_type = true; // it has parentheses
        open_paren++;
        parameter_list_part_of_kind += "*(";
        end = offset + 2;
        GetKind(offset + 3);
    }
    else
        Finalize(offset + 1);
}


void
ReadingKind::
GetKind(Unsigned offset)
{
    Initialize(offset);
    Finalize(end + 1);
}

*/

void 
ReadingKind::
ReadKind(Unsigned offset)
{
    end = offset;
    String to_be_type = ML.text_item[offset];
    if (!ML.types.Contains(to_be_type)) // if it's not a type
        ReportError("a type expected", offset);
    kind += to_be_type;
    while (true)
    {
        String next = ML.text_item[end + 1];
        if (next != "*")
            return;

        next = ML.text_item[end + 2];
        if (next != "(")
            ReportError("'(' type expected", end + 2);
        kind += "*(";
        end = end + 2;
        while (true)
        {
            ReadKind(end + 1); // it changes end
            next = ML.text_item[end + 1];
            if (next == ',')
            {
                kind += ",";
                end = end + 1;
                continue;
            }
            else if (next == ')')
            {
                kind += ")";
                end = end + 1;
                break;
            }
        }
    }
}

ReadingKind::
ReadingKind(MainLists& ML,
    Unsigned offset   // the first deco after 'new'
) :
    ML(ML), end(0), open_paren(0)/*, parens_found_for_main_type(false), parens_found_for_return_type(false)*/
{
    ReadKind(offset);
    /*GetKind(offset);
    kind = return_type_part_of_kind + parameter_list_part_of_kind;*/
}

/*
example:
    Decompose deco("G:\\Important Files\\Desktop\\Menso.txt");
    List types;
    types << "prop" << "object";
    GettingKind gk(deco.pack,types,1ULL);
*/







