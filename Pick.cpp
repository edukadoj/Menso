//#pragma hdrstop
#include "Pick.h"
#include "space.h"


void
Pick::
Run(Unsigned offset)
{
    ReadingMenso RM1(ML, offset);
    end = RM1.end;
    String next = ML.text_item[end + 1];
    if (next != "in")
        ReportError("'in' expected", end + 1);
    ReadingMenso RM2(ML, end + 2);
    end = RM2.end;
    AnalyzingMenso AM2(ML, RM2.std_menso);
    CheckingMenso CM(ML);

    /*auto Add = [&](const String& prop)
    {
        if (!ML.IsOriginalTruth(prop))
            ML.truths.FindAndDelIfAny(prop);
        auto error_code = ML.AddTruthIfNotTrivial(prop);
        if (error_code == MensoList::ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST)
            ML.truths << prop; // exact add
    };*/

    if (AM2.all_type)
    {
        if (AM2.variable_kind != RM1.kind)
            ReportError("kind of this menso is not the same as kind of quatifier variable", RM1.end);

        if (!CM.IsTrue(RM2.std_menso))
            ReportError("sofware failed to prove this all quantifier", end);

        ReplaceMenso RM(ML);
        String replaced_menso = RM.Substitute(AM2.quanprop, AM2.variable, RM1.std_menso);

        ML.truths.FindAndDelIfAny(replaced_menso);
        ML.truths << replaced_menso;
    }
    else if (AM2.some_type)
    {
        if (AM2.variable_kind != RM1.kind)
            ReportError("kind of this menso is not the same as kind of quatifier variable", RM1.end);
        ReplaceMenso RM(ML);
        String replaced_menso = RM.Substitute(AM2.quanprop, AM2.variable, RM1.std_menso);

        if (!CM.IsTrue(replaced_menso))
            ReportError("sofware fails to prove replaced menso", end);
        ML.truths << RM2.std_menso;
    }
    else if (AM2.if_type)
    {
        if (!CM.IsTrue(RM1.std_menso))
            ReportError("software failed to deduce this prop", RM1.end);

        if (!CM.IsTrue(RM2.std_menso))
            ReportError("software failed to deduce this if_type menso", RM2.end);

        if (!CM.IsTrue(AM2.part1))
            ReportError("software failed to deduce the if condition of the if prop", RM2.end);

        ML.CarefulAddTruthDirectlyInTruthList(AM2.part2);
        /*if (!ML.IsOriginalTruth(AM2.part2))
            ML.truths.FindAndDelIfAny(AM2.part2);
        auto error_code = ML.ManagedAddTruth(AM2.part2);
        if (error_code == MensoList::ErrorEnum::ALREADY_IN_TRUTH_LIST)
            ML.truths << AM2.part2; // exact add*/
    }
    else if(AM2.binary_operator_type)
    {
        if (AM2.op == "&")
        {
            if (!CM.IsTrue(RM2.std_menso))
                ReportError("software failed to deduce this prop", RM1.end);
            MensoCmp MC(ML);
            if (MC.Equal(RM1.std_menso, AM2.part1) || MC.Equal(RM1.std_menso, AM2.part2))
                ML.CarefulAddTruthDirectlyInTruthList(RM1.std_menso);
        }
        else if (AM2.op == "|")
        {
            if (!CM.IsTrue(RM1.std_menso))
                ReportError("software failed to deduce this prop", RM1.end);
            if (!CM.IsTrue(RM2.std_menso))
                ReportError("software failed to deduce this prop", RM2.end);
            MensoCmp MC(ML);
            String negate = CM.Negate(RM1.std_menso);
            if (MC.Equal(negate, AM2.part1) || MC.Equal(RM1.std_menso, CM.Negate(AM2.part1)))
                ML.CarefulAddTruthDirectlyInTruthList(AM2.part2);
            else if (MC.Equal(negate, AM2.part2) || MC.Equal(RM1.std_menso, CM.Negate(AM2.part2)))
                ML.CarefulAddTruthDirectlyInTruthList(AM2.part1);
        }
        else
            ReportError("a 'quantifier', 'if', '&' or '|' prop expected", end);
    }
    else
        ReportError("a 'quantifier', 'if', '&' or '|' prop expected", end);

    String to_be_semi = ML.text_item[end + 1];
    if (to_be_semi != ";")
        ReportError("';' expected", end + 1);
    // end = end + 1;
    end++;
    Space S(ML, end + 1);
    end = S.end;
}
