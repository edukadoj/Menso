//#pragma hdrstop
#include "ReplaceMenso.h"
#include "MensoCmp.h"

String
ReplaceMenso::
Substitute
(
    const String& basemenso,
    const String& oldmenso,
    const String& newmenso
)
{
    MensoCmp MC(ML);
    if (MC.StrictEqual(oldmenso, newmenso))
        return basemenso;
    //if (MensoCmp(ML).MLDirectEqual(basemenso, oldmenso)) // it failed for "replace a with f(a) in  f(a)= a;"
    if (MensoCmp(ML).StrictEqual(basemenso, oldmenso))
    {
        changed = true;
        return newmenso;
    }

    AnalyzingMenso AM(ML, basemenso);

    if (AM.binary_operator_type)
    {
        return  String("(") +
            Substitute(AM.part1, oldmenso, newmenso) +
            AM.op +
            Substitute(AM.part2, oldmenso, newmenso) +
            ")";

    }
    else if (AM.unary_operator_type)
    {
        return  String("(") +
            AM.op +
            Substitute(AM.part, oldmenso, newmenso) +
            ")";
    }
    else if (AM.if_type)
    {
        return  String("(if ") +
            Substitute(AM.part1, oldmenso, newmenso) +
            ")(" +
            Substitute(AM.part2, oldmenso, newmenso) +
            ")";

    }
    else if (AM.all_type)
    {
        return  String("(all ") +
            AM.variable +
            "`" +
            AM.variable_kind +
            ")(" +
            Substitute(AM.quanprop, oldmenso, newmenso) +
            ")";
    }
    else if (AM.some_type)
    {
        return  String("(some ") +
            AM.variable +
            "`" +
            AM.variable_kind +
            ")(" +
            Substitute(AM.quanprop, oldmenso, newmenso) +
            ")";
    }
    else if (AM.simple_with_args)
    {
        String ret;
        Unsigned size = AM.namearg.Size();
        ret += Substitute(AM.namearg[0], oldmenso, newmenso) + "(" +
            Substitute(AM.namearg[1], oldmenso, newmenso);
        for (Unsigned j = 2; j < size; j++)
            ret += String(",") + Substitute(AM.namearg[j], oldmenso, newmenso);
        ret += ")";
        return ret;
    }
    else if (AM.simple_without_args)
    {
        return AM.name;
    }
    return basemenso; // never executed
}
