// for replacing a menso with another menso in a base menso

#ifndef ReplaceMensoH
#define ReplaceMensoH
#include "String.h"
#include "AnalyzingMenso.h"

inline String MakeAll(String var, String var_kind, String quanmenso)
{
    return String("(all ") + var + "`" + var_kind + ")(" + quanmenso + ")";
}

inline String MakeSome(String var, String var_kind, String quanmenso)
{
    return String("(some ") + var + "`" + var_kind + ")(" + quanmenso + ")";
}

inline String MakeIf(String menso1, String menso2)
{
    return String("(if ") + menso1 + ")(" + menso2 + ")";
}

inline String MakeBinaryOp(String part1, String op, String part2)
{
    String* p1,* p2;
    if (op == "&" || op == "|" || op == "=")
    {
        if (part1 < part2)
            p1 = &part1, p2 = &part2;
        else
            p2 = &part1, p1 = &part2;
    }
    else
        p1 = &part1, p2 = &part2;
    return String("(") + *p1 + op + *p2 + ")";
}
inline String MakeUnaryOp(const String& op, const String& part)
{
    if (op == "!")
    {
        Unsigned size = part.Size();
        if (4 <= size)
        {
            if (part[0] == '(' && part[1] == '!' && part[size - 1] == ')')
                return part[2][size - 2];
        }
        if (3 <= size)
        {
            if (part[0] == '!')
                return part[1][size - 1];
        }
    }
    return String("(") + op + part + ")";
}

inline String MakeSingleOp(String op, String part)
{
    return String("(") + op + part + ")";
}

class ReplaceMenso
{
    MainLists& ML;
public:
    bool changed;

public:
    String Substitute
    (
        const String& basemenso,
        const String& oldmenso,
        const String& newmenso
    );

    String SubstituteTrue(const String& menso)
    {
        Unsigned size = ML.truths.Size();
        String ret = menso;
        for (Unsigned i = 0; i < size; i++)
            ret = Substitute(ret, ML.truths[i], "true");
        return ret;
    }

public:
    ReplaceMenso(MainLists& TL)
        : ML(TL)
    {
        changed = false;
    }
};

#endif
