// get info anout a read menso

#ifndef AnalyzingMensoH
#define AnalyzingMensoH
#include "String.h"
#include "MainLists.h"
#include "Replace.h"
#include "Thing.h"
#include "AnalyzingKind.h"


class AnalyzingMenso
{
public:
    String menso;

public:
    String kind;

    bool binary_operator_type;
    bool if_type;
    String part1;
    String op;
    String part2;


    bool unary_operator_type;
    String part;

    bool all_type;
    bool some_type;
    String variable;
    String variable_kind;
    String quanprop;

    bool simple_with_args;
    List namearg;

    bool simple_without_args;
    String name;

private:
    const MainLists& ML;

    String DoubleOperator() // double operator
    {
        Unsigned size = menso.Size();
        int paren_number = 0;
        for (Unsigned i = 1; i < size - 1; i++)
        {
            char ch = menso[i];
            if (ch == '(')
            {
                paren_number++;
                continue;
            }
            if (ch == ')')
            {
                paren_number--;
                continue;
            }
            if (paren_number == 0)
                if (ML.IsMultipleOperator(ch))
                {
                    binary_operator_type = true;
                    part2 = menso[1][size - 2]; // size is at least 3
                    part1 = part2.CutOff((Unsigned)(i - 1));
                    op = ch;
                    return ch;
                }
        }
        return "";
    }

public:
    String Identify()
    {
        if (menso[0] == '(')  // quan or op
        {
            Unsigned size = menso.Size();
            if (3 <= size)
                if (menso[1][2] == "if")
                {
                    if_type = true;
                    Replace R(menso);
                    size = R.Size();

                    int paren_number = 1;
                    Unsigned j;
                    for (j = 3; 0 < paren_number; j++)
                    {
                        if (String(R[j]) == "(")
                            paren_number++;
                        if (String(R[j]) == ")")
                            paren_number--;
                        if (paren_number != 0)
                            part1 += R[j];
                    }

                    for (Unsigned k = j + 1; k < size - 1; k++)
                        part2 += R[k];
                    return "if";
                }

            if (4 <= size)
                if (menso[1][3] == "all")
                {
                    all_type = true;
                    Replace R(menso);
                    variable = R[3];

                    size = R.Size();

                    int paren_number = 1;
                    Unsigned j;
                    for (j = 5; 0 < paren_number; j++)
                    {
                        if (String(R[j]) == "(")
                            paren_number++;
                        if (String(R[j]) == ")")
                            paren_number--;
                        if (paren_number != 0)
                            variable_kind += R[j];
                    }

                    for (Unsigned k = j + 1; k < size - 1; k++)
                        quanprop += R[k];

                    return "all";
                }

            if (5 <= size)
                if (menso[1][4] == "some")
                {
                    some_type = true;
                    Replace R(menso);
                    variable = R[3];

                    size = R.Size();

                    int paren_number = 1;
                    Unsigned j;
                    for (j = 5; 0 < paren_number; j++)
                    {
                        if (String(R[j]) == "(")
                            paren_number++;
                        if (String(R[j]) == ")")
                            paren_number--;
                        if (paren_number != 0)
                            variable_kind += R[j];
                    }

                    for (Unsigned k = j + 1; k < size - 1; k++)
                        quanprop += R[k];

                    return "some";
                }
            String maybe_op = menso[1];
            if (ML.IsMultipleOperator(maybe_op)) // single operator
            {
                unary_operator_type = true;
                part = menso[2][size - 2]; // size is at least 4
                op = maybe_op;
                return maybe_op;
            }
            return DoubleOperator();   // double operator
        }
        else // simple with or woithout args
        {
            Replace R(menso);
            Unsigned size = R.Size();
            if (size == 1)
            {
                simple_without_args = true;
                name = R[0];
                return name;
            }
            else if (size >= 2) // with args
            {
                simple_with_args = true;
                namearg << R[0];

                Unsigned paren_number = 0;
                Unsigned start = 2;

                Unsigned i;
                for (i = 2; i <= size - 2; i++)
                {
                    if (String(R[i]) == "(")
                    {
                        paren_number++;
                        continue;
                    }
                    if (String(R[i]) == ")")
                    {
                        paren_number--;
                        continue;
                    }
                    if (paren_number == 0)
                        if (String(R[i]) == ",")
                        {
                            String arg;
                            for (Unsigned j = start; j < i; j++)
                                arg += R[j];
                            namearg << arg;
                            start = i + 1;
                        }
                }
                String arg;
                for (Unsigned j = start; j < i; j++)
                    arg += R[j];
                namearg << arg;
                return R[0];
            }
        }
        return "undefined"; // this line will never be executed
    }

    void Run()
    {
        String str = Identify();
        if (binary_operator_type)
        {
            IsOperator IO(ML, str);
            AnalyzingKind AK(IO.kind);
            kind = AK.ReturnType();
        }
        else if (unary_operator_type)
        {
            IsOperator IO(ML, str);
            AnalyzingKind AK(IO.kind);
            kind = AK.ReturnType();
        }
        else if (if_type)
        {
            kind = "prop";
        }
        else if (all_type)
        {
            kind = "prop";
        }
        else if (some_type)
        {
            kind = "prop";
        }
        else if (simple_with_args)
        {
            IsThing IT(ML, str);
            AnalyzingKind AK(IT.kind);
            kind = AK.ReturnType();
        }
        else if (simple_without_args)
        {
            IsThing IT(ML, str);
            kind = IT.kind;
        }
    }

public:
    AnalyzingMenso(const MainLists& ML, String menso)
        : ML(ML), menso(menso)
    {
        binary_operator_type = false;
        unary_operator_type = false;
        if_type = false;
        all_type = false;
        some_type = false;
        simple_with_args = false;
        simple_without_args = false;
        Run();
    }
};


#endif
