// #pragma hdrstop
#include "New.h"

/*
String
New::
New_(Unsigned offset)
{
    String ret;
    String name = ML.deco[offset + 1];
    ML.deco[offset + 1] = "*"; // temporary change for ReadingKind
    ReadingKind RK(ML, offset);
    ML.deco[offset + 1] = name;
    

    Thing T(name, RK.kind);

    if (ML.IsMultipleName(T.name) == false)
        ret = T.thing;
    else
        ReportError("multiple name", offset + 1);

    if (RK.parens_found_for_main_type == true)  // if it has parameters
        end = RK.end + 1; // because in "prop p(object);" RK.end is ")", one more step to be ';'
    else
        end = RK.end + 2; // because in "prop p;" RK.end is "prop", two more steps to be ';'

    String next = ML.deco[end];
    if (next != ";")
        ReportError("';' expected", end);
    end = end + 1;
    return ret;
}
*/

void
New::
NewOperator(Unsigned offset)
{
    ReadingKind RK1(ML, offset); // just to determine the offset of name
    String to_be_operator = ML.text_item[RK1.end + 1];
    CheckingOperator CN(to_be_operator);
    if (!CN.is_legal)
        ReportError("a legal name expected", RK1.end + 1);
    if (ML.IsMultipleOperator(to_be_operator))
        ReportError("multiple operator", RK1.end + 1);

    String after_operator = ML.text_item[RK1.end + 2];
    if (after_operator != "(")
        ReportError("'(' expected", RK1.end + 1);

    ML.text_item[RK1.end + 1] = "*"; // temporary change to get ReadingKind working
    ReadingKind RK2(ML, offset);
    ML.text_item[RK1.end + 1] = to_be_operator;
    end = RK2.end;

    ThingOrOperator T(to_be_operator, RK2.kind);
    AnalyzingKind AK(T.kind);

    switch (AK.ParameterNumber())
    {
    case 1:
    case 2:
        ML.operators << T.thing;
        break;
    default:
        ReportError("operators can have one or two parameters", end);
    }


    /*String to_be_operator = ML.deco[offset + 1];
    CheckingOperator CN(to_be_operator);
    if (CN.is_legal == true)
    {
        if (ML.IsMultipleOperator(to_be_operator))
            ReportError("multiple operator", offset + 1);

        Unsigned changing_offset = offset;
        String retted = New_(changing_offset); // offset changes
        changing_offset = end;
        Thing T(retted);

        AnalyzingKind AK(T.kind);
        Unsigned parameter_number = AK.ParameterNumber();

        if ((1 <= parameter_number) && (parameter_number <= 2))
        {
            ML.operators << retted;
            Space s(ML, changing_offset);
        }
        else
            ReportError("operators can have one or two parameters", offset + 1);
    }
    else
        ReportError("a legal name expected", offset + 1);*/
}

ThingOrOperator
New::
NewThing(Unsigned offset)
{
    ReadingKind RK1(ML, offset); // just to determine the offset of name
    String to_be_name = ML.text_item[RK1.end + 1];
    CheckingName CN(to_be_name);
    if (!CN.is_legal)
        ReportError("a legal name expected", RK1.end + 1);
    if (ML.IsMultipleName(to_be_name))
        ReportError("multiple name", RK1.end + 1);

    String after_name = ML.text_item[RK1.end + 2];
    if (after_name == "(")
    {
        ML.text_item[RK1.end + 1] = "*"; // temporary change to get ReadingKind working
        ReadingKind RK2(ML, offset);
        ML.text_item[RK1.end + 1] = to_be_name;

        ThingOrOperator T(to_be_name, RK2.kind);
        ML.things << T.thing;

        end = RK2.end;
        return T;
    }
    else
    {
        ThingOrOperator T(to_be_name, RK1.kind);
        ML.things << T.thing;
        end = RK1.end + 1;
        return T;
    }

    /*String to_be_name = ML.deco[offset + 1];
    CheckingName CN(to_be_name);
    if (!CN.is_legal)
        ReportError("a legal name expected", offset + 1);

    //Unsigned changing_offset = offset;
    ML.things << New_(offset);
    // offset = end;
    Space S(ML, end);
    end = S.end;*/
}

void
New::
Run(Unsigned offset)
{
    String type_or_operator = ML.text_item[offset];
    if (type_or_operator == "operator")
        NewOperator(offset + 1);
    else
        NewThing(offset);

    String next = ML.text_item[end + 1];
    if (next != ";")
        ReportError("';' expected", end);

    Space S(ML, end + 2);
    end = S.end;
}

New::
New(MainLists& ML, Unsigned offset)
    : ML(ML)
{
    Run(offset);
}

