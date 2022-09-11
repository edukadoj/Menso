// #pragma hdrstop
#include "Check.h"
#include "Space.h"


void
Check::
Run(Unsigned offset)
{
    ReadingMenso RM(ML, offset);
    if (RM.kind != "prop")
        ReportError("only props can be checked", RM.end);
    menso = RM.std_menso;
    CheckingMenso CM(ML);
    if (!CM.IsTrue(menso))
        ReportError("software failed to deduce the menso", RM.end);
    String next = ML.text_item[RM.end + 1];
    if (next != ";")
        ReportError("';' extected", RM.end + 1);
    end = RM.end + 1;

    /*if (!ML.IsOriginalTruth(menso))
        ML.truths.FindAndDelIfAny(menso);

    //ML.truths << menso; // bad code because: repetitive adds, (if p)p add etc.
    auto error_code = ML.AddTruthIfNotTrivial(menso);
    if (error_code == MensoList::ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST)
        ML.truths << menso; // exact add*/
    ML.CarefulAddTruthDirectlyInTruthList(menso);

    AnalyzingMenso AM(ML, menso);
    MensoCmp MC(ML);

    if (AM.binary_operator_type)
        if (AM.op == "&")
            if (MC.Equal(AM.part1, CM.Negate(AM.part2)))
                ML.truths << "false";

    Space S(ML, end + 1);
    end = S.end;
}
