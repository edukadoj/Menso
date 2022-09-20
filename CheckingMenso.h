//for seeing if a prop menso is true or false

#ifndef CheckingMensoH
#define CheckingMensoH
#include "MainLists.h"
#include "MensoCmp.h"

class CheckingMenso
{
    MainLists& ML;

public:
    String Negate(const String& menso)
    {
        /*AnalyzingMenso AM(ML, menso);
        if (AM.unary_operator_type && AM.op == '!')
            return AM.part;*/
        return MakeUnaryOp("!", menso);//String("(!") + menso + ")";
    }

    bool TruthContain(const String& menso)
    {
        MensoCmp MC(ML);
        Unsigned size = ML.truths.Size();
        if (1 <= size)
            for (Unsigned i = size - 1; i != MAXUnsigned; i--)
                if (MC.Equal(ML.truths[i], menso))
                    return true;
        return false;
    }

    bool EqualsSomethingInTruthList(String menso)
    {
        MensoCmp MC(ML);

        Unsigned size = ML.truths.Size();
        if (1 <= size)
            for (Unsigned i = size - 1; i != MAXUnsigned; i--)
            {
                if (MC.Equal(menso, ML.truths[i]))
                    return true;
            }
        return false;
    }

    bool CanBeDeducedFromTruthList(String menso)
    {
        MensoCmp MC(ML);

        Unsigned size = ML.truths.Size();
        if (1 <= size)
            for (Unsigned i = size - 1; i != MAXUnsigned; i--)
            {
                AnalyzingMenso AM(ML, ML.truths[i]);
                if (AM.if_type)
                {
                    if (MC.Equal(menso, Negate(AM.part1)) && IsTrue(Negate(AM.part2)))
                        return true;
                }
            }
        return false;
    }

    bool IsTrue(const String& menso)
    {
        List forbidden_list;
        return IsTrue(menso, forbidden_list);
    }

    bool IsTrue(const String& menso, List forbidden_list)
    {
        if (forbidden_list.Contains(menso)) // to prevent infinite loops
            return false;
        forbidden_list << menso;
        if (EqualsSomethingInTruthList(menso))
            return true;
        if (CanBeDeducedFromTruthList(menso))
            return true;

        MensoCmp MC(ML);
        AnalyzingMenso AM_mainmenso(ML, menso);
        if (AM_mainmenso.if_type)
        {
            AnalyzingMenso AM2(ML, AM_mainmenso.part2);
            if (AM2.binary_operator_type)
            {
                if (AM2.op == "&")
                {
                    if (IsTrue(MakeIf(AM_mainmenso.part1, AM2.part1)) &&
                        IsTrue(MakeIf(AM_mainmenso.part1, AM2.part2))) // if (if p)q1 and (if p) q2 then (if p) q1 & q2
                        return true;
                }
            }

            Unsigned size = ML.truths.Size();
            ML.truths << AM_mainmenso.part1; // temporarily assume part1 is true
            if (IsTrue(AM_mainmenso.part2, forbidden_list)) // check if part2 is true
            {
                ML.truths[size] = 0; // if so remove the temporary assumtion
                return true; // anounce (if part1) part2 is true
            }
            ML.truths[size] = 0; // remove the temporary assumtion

            if (IsTrue(Negate(AM_mainmenso.part1), forbidden_list))
                return true;
            if (IsTrue(AM_mainmenso.part2, forbidden_list))
                return true;
            if (MC.Equal(AM_mainmenso.part1, AM_mainmenso.part2))
                return true;
            if (IsTrue(Negate(MakeBinaryOp(AM_mainmenso.part1, "&", Negate(AM_mainmenso.part2))), forbidden_list)) // if p & !q is false  then (if p)q
                return true;
            if (IsTrue(MakeBinaryOp(Negate(AM_mainmenso.part1), "|", AM_mainmenso.part2), forbidden_list)) // time-consuming // if !p | q  then (if p )q
                return true;
            if (IsTrue(MakeIf(Negate(AM_mainmenso.part2), Negate(AM_mainmenso.part1)), forbidden_list)) // time-consuming // if (if !q)!p then (if p)q
                return true;
        }
        else if (AM_mainmenso.all_type)
        {
            auto size = ML.things.Size();
            ML.things << AM_mainmenso.variable + "`" + AM_mainmenso.variable_kind;
            if (IsTrue(AM_mainmenso.quanprop))
            {
                ML.things[size] = 0;
                return true;
            }
            ML.things[size] = 0;

            Replace R(AM_mainmenso.quanprop);
            if (!R.Contains(AM_mainmenso.variable))
                if (IsTrue(AM_mainmenso.quanprop, forbidden_list))
                    return true;

            if (AM_mainmenso.variable_kind == "prop") // Time ConSuming
            {
                ReplaceMenso RM(ML);
                String menso1 = RM.Substitute(AM_mainmenso.quanprop, AM_mainmenso.variable, "true");
                String menso2 = RM.Substitute(AM_mainmenso.quanprop, AM_mainmenso.variable, "false");
                if (IsTrue(menso1) && IsTrue(menso2))
                    return true;
            }

            AnalyzingMenso AMquanprop(ML, AM_mainmenso.quanprop);
            if (AMquanprop.binary_operator_type)
            {
                if (AMquanprop.op == "&")
                {
                    if (IsTrue(MakeAll(AM_mainmenso.variable, AM_mainmenso.variable_kind, AMquanprop.part1), forbidden_list) &&
                        IsTrue(MakeAll(AM_mainmenso.variable, AM_mainmenso.variable_kind, AMquanprop.part2), forbidden_list)) // if (all x)(p(x)) and (all x)(p(y)) then (all x)(p(x) & p(y))
                        return true;
                }
            }
            else if (AMquanprop.all_type) // re_ordering all quan 
            {
                Replace R1(AMquanprop.quanprop);
                auto temp_variable_name = R1.NewTempName();
                auto quanprop1 = R1.Substitute(AM_mainmenso.variable, temp_variable_name); // (t1,t2) -> (t1,t3)
                Replace R2(quanprop1);
                auto quanprop2 = R2.Substitute(AMquanprop.variable, AM_mainmenso.variable); // (t1, t3) -> (t2,t3)
                Replace R3(quanprop2);
                auto quanprop_variables_interchanged = R3.Substitute(temp_variable_name, AMquanprop.variable); // (t2,t3) -> (t2,t1)

                String internal = MakeAll(AMquanprop.variable, AM_mainmenso.variable_kind, quanprop_variables_interchanged);
                String external = MakeAll(AM_mainmenso.variable, AMquanprop.variable_kind, internal);
                if (IsTrue(external, forbidden_list))
                    return true;
            }

            if (IsTrue(Negate(MakeSome(AM_mainmenso.variable, AM_mainmenso.variable_kind, Negate(AM_mainmenso.quanprop))))) // if !(some x)!p(x)  then (all x)p(x)
                return true;
        }
        else if (AM_mainmenso.some_type)
        {
            auto size = ML.things.Size();
            ML.things << AM_mainmenso.variable + "`" + AM_mainmenso.variable_kind;
            if (IsTrue(AM_mainmenso.quanprop))
            {
                ML.things[size] = 0;
                return true;
            }
            ML.things[size] = 0;

            Replace R(AM_mainmenso.quanprop);
            if (!R.Contains(AM_mainmenso.variable))
                if (IsTrue(AM_mainmenso.quanprop, forbidden_list))
                    return true;

            if (IsTrue(Negate(MakeAll(AM_mainmenso.variable, AM_mainmenso.variable_kind, Negate(AM_mainmenso.quanprop)))))
                return true;

            AnalyzingMenso AMquanprop(ML, AM_mainmenso.quanprop);
            if (AMquanprop.some_type) // re_ordering some quan 
            {
                Replace R1(AMquanprop.quanprop);
                auto temp_variable_name = R1.NewTempName();
                auto quanprop1 = R1.Substitute(AM_mainmenso.variable, temp_variable_name); // (t1,t2) -> (t1,t3)
                Replace R2(quanprop1);
                auto quanprop2 = R2.Substitute(AMquanprop.variable, AM_mainmenso.variable); // (t1, t3) -> (t2,t3)
                Replace R3(quanprop2);
                auto quanprop_variables_interchanged = R3.Substitute(temp_variable_name, AMquanprop.variable); // (t2,t3) -> (t2,t1)

                String internal = MakeSome(AMquanprop.variable, AM_mainmenso.variable_kind, quanprop_variables_interchanged);
                String external = MakeSome(AM_mainmenso.variable, AMquanprop.variable_kind, internal);
                if (IsTrue(external, forbidden_list))
                    return true;
            }
            else if (AMquanprop.all_type)
            {
                AnalyzingKind AK(AM_mainmenso.variable_kind);

                if (AK.IsFunctional() && AK.ParameterNumber() == 1 && AK.nthParameterKind(1) == AMquanprop.variable_kind)
                {
                    Replace R1(AMquanprop.quanprop);
                    auto new_variable_name = R1.NewTempName();
                    //auto new_quanprop = R.Substitute(AM_mainmenso.variable, temp_variable_name);
                    ReplaceMenso RM(ML);

                    String oldmenso = AM_mainmenso.variable + "(" + AMquanprop.variable + ")";
                    String replaced_menso = RM.Substitute(AMquanprop.quanprop, oldmenso, new_variable_name); // replace f(x) in axiom of choice with a name

                    Replace R2(replaced_menso);
                    auto newer_variable_name = R2.NewTempName();
                    auto replaced_menso_check = R2.Substitute(AM_mainmenso.variable, newer_variable_name);
                    if (replaced_menso == replaced_menso_check) // if after replacing f(x)s there is no f remained
                    {
                        Replace RR1(replaced_menso);
                        auto quanprop1 = RR1.Substitute(AMquanprop.variable, AM_mainmenso.variable);

                        Replace RR2(quanprop1);
                        auto quanprop2 = RR2.Substitute(new_variable_name, AMquanprop.variable);

                        auto internal = MakeSome(AMquanprop.variable, AK.ReturnType(), quanprop2);
                        auto external = MakeAll(AM_mainmenso.variable, AMquanprop.variable_kind, internal);
                        if (IsTrue(external, forbidden_list))
                            return true;
                    }
                }


            }
        }
        else if (AM_mainmenso.binary_operator_type)
        {
            if (AM_mainmenso.op == "&")
            {
                if (IsTrue(AM_mainmenso.part1, forbidden_list) && IsTrue(AM_mainmenso.part2, forbidden_list))
                    return true;
                if (IsTrue(Negate(MakeIf(AM_mainmenso.part1, Negate(AM_mainmenso.part2))), forbidden_list))
                    return true;
            }
            else if (AM_mainmenso.op == "|")
            {
                if (MC.Equal(AM_mainmenso.part1, Negate(AM_mainmenso.part2))) // p | !p is true
                    return true;
                if (IsTrue(AM_mainmenso.part1, forbidden_list) || IsTrue(AM_mainmenso.part2, forbidden_list))
                    return true;
                if (IsTrue(MakeIf(Negate(AM_mainmenso.part1), AM_mainmenso.part2), forbidden_list) ||
                    IsTrue(MakeIf(Negate(AM_mainmenso.part2), AM_mainmenso.part1), forbidden_list)) // if (if !p)q or (if !q)p then p | q
                    return true;
            }
            else if (AM_mainmenso.op == "=")
            {
                if (MC.Equal(AM_mainmenso.part1, AM_mainmenso.part2))
                    return true;

                AnalyzingMenso AM1(ML, AM_mainmenso.part1);
                if (AM1.kind == "prop") // then part2 will be prop too
                {
                    bool p1 = IsTrue(AM_mainmenso.part1, forbidden_list);
                    bool p2 = IsTrue(AM_mainmenso.part2, forbidden_list);
                    if (p1 == true && p2 == true) // if same true-falfe                                            
                        return true;

                    bool np1 = IsTrue(Negate(AM_mainmenso.part1), forbidden_list);
                    bool np2 = IsTrue(Negate(AM_mainmenso.part2), forbidden_list);
                    if (np1 == true && np2 == true)
                        return true;

                    if (TruthContain(MakeIf(AM_mainmenso.part1, AM_mainmenso.part2)))
                        if (TruthContain(MakeIf(AM_mainmenso.part2, AM_mainmenso.part1))) // if (if p) q and (if q)p then p =q
                            return true;
                }
            }
        }
        else if (AM_mainmenso.unary_operator_type)
        {
            if (AM_mainmenso.op == "!") // negate
            {
                if (MC.Equal(AM_mainmenso.part, "false"))
                    return true;

                AnalyzingMenso AM(ML, AM_mainmenso.part);

                if (AM.binary_operator_type)
                {
                    if (AM.op == "&") // if mesno is of the form !(p & q)
                    {
                        if (MC.Equal(AM.part1, Negate(AM.part2))) // p & !p is false
                            return true;
                        if (IsTrue(Negate(AM.part1), forbidden_list) || IsTrue(Negate(AM.part2), forbidden_list)) // if !p or !q then !(p & q)
                            return true;
                        if (IsTrue(MakeBinaryOp(Negate(AM.part1), "|", Negate(AM.part2)), forbidden_list)) // if !p | !q then !(p & q)
                            return true;
                        if (IsTrue(MakeIf(AM.part1, Negate(AM.part2)), forbidden_list) ||
                            IsTrue(MakeIf(AM.part2, Negate(AM.part1)), forbidden_list)) // if (if p)!q or similar then !(p & q)
                            return true;
                    }
                    if (AM.op == "|") // if mesno is of the form !(p | q)
                    {
                        if (IsTrue(Negate(AM.part1), forbidden_list) && IsTrue(Negate(AM.part2), forbidden_list)) // if !p and !q then !(p | q)
                            return true;
                        if (IsTrue(MakeBinaryOp(Negate(AM.part1), "&", Negate(AM.part2)), forbidden_list)) // if !p & !q then !(p & q) // this if can be removed because it is covered by other parts but one should consider speed
                            return true;
                        if (IsTrue(Negate(MakeIf(Negate(AM.part1), AM.part2)), forbidden_list) ||
                            IsTrue(Negate(MakeIf(Negate(AM.part2), AM.part1)), forbidden_list)) // if (if !p)q or (if !q)p is false then p | q is false
                            return true;
                    }
                }
                else if (AM.if_type)
                {
                    if (IsTrue(Negate(MakeBinaryOp(Negate(AM.part1), "|", AM.part2)), forbidden_list)) // if !p | q is false then (if p )q is false
                        return true;
                    if (IsTrue(MakeBinaryOp(AM.part1, "&", Negate(AM.part2)), forbidden_list)) // if p & !q  then (if p)q is false
                        return true;
                    if (IsTrue(AM.part1, forbidden_list) && IsTrue(Negate(AM.part2), forbidden_list)) // if p & !q  then (if p)q is false
                        return true;
                    if (IsTrue(Negate(MakeIf(Negate(AM.part2), Negate(AM.part1))), forbidden_list)) // if (if !q)!p is false then (if p)q is false
                        return true;
                }
            } // negate
        }

        // deduce simple props from compounds:
        Unsigned size = ML.truths.Size();
        if (1 <= size)
            for (Unsigned i = size - 1; i != MAXUnsigned; i--)
            {
                if (menso == "false")
                    for (Unsigned j = size - 1; j != MAXUnsigned; j--)
                    {
                        if (MC.Equal(ML.truths[i], Negate(ML.truths[j]))) // if p & !p for some p then false is true
                            return true;
                    }
                AnalyzingMenso AM(ML, ML.truths[i]);
                if (AM.if_type)
                {
                    /* if (MC.Equal(AM.part2, menso))
                         if (IsTrue(AM.part1))
                             return true;*/
                    if (MC.Equal(AM.part2, menso) && !MC.Equal(AM.part1, menso)) // "!MC.Equal(AM.part1, menso)" is necessary to prevent infinite loops
                    {
                        if (IsTrue(AM.part1, forbidden_list))
                            return true;
                    }
                }
                else if (AM.binary_operator_type)
                {
                    if (AM.op == "&")
                    {
                        /*if (MC.Equal("false", menso) && MC.Equal(AM.part1, Negate(AM.part2))) // if p & !p is true then false is true
                            return true;*/ // seems unncessary

                        if (MC.Equal(AM.part1, menso))
                            return true;
                        if (MC.Equal(AM.part2, menso))
                            return true;
                        AnalyzingMenso AM1(ML, AM.part1);
                        if (AM1.if_type)
                        {
                            if (MC.Equal(AM1.part2, menso) && MC.Equal(AM1.part1, AM.part2)) // if (if p)(q) & p then q
                                return true;
                        }
                        else if (AM1.binary_operator_type)
                        {
                            if (AM1.op == "|")
                            {
                                if (MC.Equal(AM1.part2, menso) && MC.Equal(AM1.part1, Negate(AM.part2)))
                                    return true;
                                else if (MC.Equal(AM1.part1, menso) && MC.Equal(AM1.part2, Negate(AM.part2)))
                                    return true;
                            }
                        }

                        AnalyzingMenso AM2(ML, AM.part2);
                        if (AM2.if_type)
                        {
                            if (MC.Equal(AM2.part2, menso) && MC.Equal(AM2.part1, AM.part1)) // if p & (if p)(q) then q
                                return true;
                        }
                        else if (AM2.binary_operator_type)
                        {
                            if (AM2.op == "|")
                            {
                                if (MC.Equal(AM2.part2, menso) && MC.Equal(AM2.part1, Negate(AM.part1)))
                                    return true;
                                else if (MC.Equal(AM2.part1, menso) && MC.Equal(AM2.part2, Negate(AM.part1)))
                                    return true;
                            }
                        }
                    }
                    if (AM.op == "|")
                    {
                        if (MC.Equal(AM.part1, menso))
                        {
                            if (IsTrue(Negate(AM.part2), forbidden_list))
                                return true;
                        }
                        if (MC.Equal(AM.part2, menso))
                        {
                            if (IsTrue(Negate(AM.part1), forbidden_list))
                                return true;
                        }
                    }
                    if (AM.op == "=")
                    {
                        AnalyzingMenso AMprime(ML, menso);
                        if (AMprime.binary_operator_type)
                            if (AMprime.op == "=")
                            {
                                if (MC.Equal(AM.part1, AMprime.part1))
                                    if (MC.Equal(AM.part2, AMprime.part2))
                                        return true;
                                if (MC.Equal(AM.part1, AMprime.part2))
                                    if (MC.Equal(AM.part2, AMprime.part1))
                                        return true;
                            }
                    }
                }
                else if (AM.all_type)
                {
                    Replace R(AM.quanprop);
                    if (!R.Contains(AM.variable))
                        if (MC.Equal(menso, AM.quanprop))
                            return true;
                }
                else if (AM.some_type)
                {
                    Replace R(AM.quanprop);
                    if (!R.Contains(AM.variable))
                        if (MC.Equal(menso, AM.quanprop))
                            return true;
                }
            } // for
        return false;
    }
public:
    CheckingMenso(MainLists& ML)
        :ML(ML)
    {
    }
};

#endif