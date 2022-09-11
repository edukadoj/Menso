// to see if 2 mensos are equal

#ifndef MensoCmpH
#define MensoCmpH
#include "MainLists.h"
#include "AnalyzingMenso.h"
#include "ReplaceMenso.h"


class MensoCmp
{
public:
    const MainLists& ML;
    List thepart1;
    List thepart2;

    bool StrictEqual(String menso1, String menso2)
    {
        if (menso1 == menso2)
            return true;

        AnalyzingMenso AM1(ML, menso1);
        AnalyzingMenso AM2(ML, menso2);

        if (AM1.kind != AM2.kind)
            return false;

        if (AM1.unary_operator_type) // !!p = p
            if (AM1.op == "!")
            {
                AnalyzingMenso AT(ML, AM1.part);
                if (AT.unary_operator_type)
                    if (AT.op == "!")
                        return StrictEqual(AT.part, menso2);
            }

        if (AM2.unary_operator_type) // p = !!p
            if (AM2.op == "!")
            {
                AnalyzingMenso AM(ML, AM2.part);
                if (AM.unary_operator_type)
                    if (AM.op == "!")
                        return StrictEqual(menso1, AM.part);
            }

        if (AM1.binary_operator_type != AM2.binary_operator_type)
            return false;
        if (AM1.unary_operator_type != AM2.unary_operator_type)
            return false;
        if (AM1.all_type != AM2.all_type)
            return false;
        if (AM1.if_type != AM2.if_type)
            return false;
        if (AM1.some_type != AM2.some_type)
            return false;
        if (AM1.simple_with_args != AM2.simple_with_args)
            return false;
        if (AM1.simple_without_args != AM2.simple_without_args)
            return false;

        if (AM1.binary_operator_type)
        {
            if (AM1.op != AM2.op)
                return false;
            if (StrictEqual(AM1.part1, AM2.part1) && StrictEqual(AM1.part2, AM2.part2))
                return true;
            if (AM1.op == "&" || AM1.op == "|" || AM1.op == "=")
                if (StrictEqual(AM1.part1, AM2.part2) && StrictEqual(AM1.part2, AM2.part1))
                    return true;

            return false;
        }
        else if (AM1.unary_operator_type)
        {
            if (AM1.op != AM2.op)
                return false;
            if (StrictEqual(AM1.part, AM2.part))
                return true;
            return false;
        }
        else if (AM1.if_type)
        {
            if (StrictEqual(AM1.part1, AM2.part1) && StrictEqual(AM1.part2, AM2.part2))
                return true;
        }
        else if (AM1.all_type)
        {
            if (AM1.variable_kind != AM2.variable_kind)
                return false;
            Replace R(AM2.quanprop);
            String AM2quanmenso = R.Substitute(AM2.variable, AM1.variable); // same variable;
            if (StrictEqual(AM1.quanprop, AM2quanmenso))
                return true;
            return false;
        }
        else if (AM1.some_type)
        {
            if (AM1.variable_kind != AM2.variable_kind)
                return false;
            Replace R(AM2.quanprop);
            String AT2quanmenso = R.Substitute(AM2.variable, AM1.variable); // same variable;
            if (StrictEqual(AM1.quanprop, AT2quanmenso))
                return true;
            return false;
        }
        else if (AM1.simple_with_args)
        {
            Unsigned size1 = AM1.namearg.Size();
            Unsigned size2 = AM2.namearg.Size();
            if (size1 != size2)
                return false;
            for (Unsigned j = 0; j < size1; j++)
                if (!StrictEqual(AM1.namearg[j], AM2.namearg[j]))
                    return false;
            return true;
        }
        else if (AM1.simple_without_args)
        {
            if (AM1.name == AM2.name)
                return true;
            return false;
        }
        return false;
    }

    bool DirectEqual(String menso1, String menso2)
    {
        if (StrictEqual(menso1, menso2))
            return true;

        if (menso1.Size() > menso2.Size())
        {
            String temp = menso1;
            menso1 = menso2;
            menso2 = temp;
        }

        if (StrictEqual(menso1, "true") && StrictEqual(menso2, "(!false)"))
            return true;
        if (StrictEqual(menso1, "false") && StrictEqual(menso2, "(!true)"))
            return true;

        if (StrictEqual(menso1, "true") && StrictEqual(menso2, "(true&true)"))
            return true;
        if (StrictEqual(menso1, "true") && StrictEqual(menso2, "(true|true)"))
            return true;

        if (StrictEqual(menso1, "false") && StrictEqual(menso2, "(false&false)"))
            return true;
        if (StrictEqual(menso1, "false") && StrictEqual(menso2, "(false|false)"))
            return true;
        if (StrictEqual(menso1, "true") && (StrictEqual(menso2, "(true|false)") || StrictEqual(menso2, "(false|true)")))
            return true;
        if (StrictEqual(menso1, "false") && (StrictEqual(menso2, "(true&false)") || StrictEqual(menso2, "(false&true)")))
            return true;

        if (StrictEqual(menso1, "true"))
        {
            AnalyzingMenso AM2(ML, menso2);
            if (AM2.binary_operator_type)
                if (AM2.op == "=")
                    if (Equal(AM2.part1, AM2.part2))
                        return true;
        }

        Unsigned size = thepart1.Size();
        for (Unsigned i = 0; i < size; i++)
        {
            if (StrictEqual(menso1, thepart1[i]) && StrictEqual(menso2, thepart2[i]))
                return true;
            if (StrictEqual(menso1, thepart2[i]) && StrictEqual(menso2, thepart1[i]))
                return true;
        }

        AnalyzingMenso AM1(ML, menso1);
        if (AM1.binary_operator_type)
            if (AM1.op == "&" || AM1.op == "|")
            {
                if (Equal(AM1.part1, AM1.part2))
                    if (Equal(AM1.part1, menso2) || Equal(AM1.part2, menso2))
                        return true;
            }
        AnalyzingMenso AM2(ML, menso2);
        if (AM2.binary_operator_type)
            if (AM2.op == "&" || AM2.op == "|")
            {
                if (Equal(AM2.part1, AM2.part2))
                    if (Equal(AM2.part1, menso1) || Equal(AM2.part2, menso1))
                        return true;
            }


        return false;
    }

    bool Equal(String menso1, String menso2)
    {
        if (DirectEqual(menso1, menso2))
            return true;
       /* MainLists ML_copy = ML; // Time Consuming
        ReplaceMenso RM(ML_copy);              
        menso1 = RM.SubstituteTrue(menso1);
        menso2 = RM.SubstituteTrue(menso2);
        if(DirectEqual(menso1,menso2))
            return true;   */
        return false;
    }

    void Run()
    {
        Unsigned size = ML.truths.Size();

        if (1 <= size)
            for (Unsigned i = size - 1; i != MAXUnsigned; i--)
            {
                String menso = ML.truths[i];
                AnalyzingMenso AT(ML, menso);
                if (AT.binary_operator_type)
                    if (AT.op == "=")
                        if (!StrictEqual(AT.part1, AT.part2))
                        {
                            if (AT.part1.Size() > AT.part2.Size())
                            {
                                String temp = AT.part1;
                                AT.part1 = AT.part2;
                                AT.part2 = temp;
                            }
                            thepart1 << AT.part1;
                            thepart2 << AT.part2;
                        }
            }
    }

    MensoCmp(const MainLists& ML)
        : ML(ML)
    {
        Run();
    }
};


#endif
