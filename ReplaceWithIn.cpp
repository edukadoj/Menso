#pragma hdrstop
#include "ReplaceWithIn.h"
#include "Space.h"

void
ReplaceWithIn::Run(Unsigned offset)
{
    ReadingMenso RM1(ML, offset);
    end = RM1.end;
    String to_be_with = ML.text_item[end + 1];
    if (to_be_with != "with")
        ReportError("'with' expected", end + 1);
    ReadingMenso RM2(ML, end + 2);
    end = RM2.end;
    MensoCmp MC(ML);
    if (!MC.Equal(RM1.std_menso, RM2.std_menso))
        ReportError("this menso must be equal to previous one", end);
    String to_be_in = ML.text_item[end + 1];
    if (to_be_in != "in")
        ReportError("'in' expected", end + 1);
    ReadingMenso RM3(ML, end + 2);
    end = RM3.end;
    if (RM3.kind != "prop")
        ReportError("this menso must be prop", end);
    CheckingMenso CM(ML);
    if (!CM.IsTrue(RM3.std_menso))
        ReportError("this menso is not proved", end);

    String to_be_semi = ML.text_item[end + 1];
    if (to_be_semi != ";")
        ReportError("';' expected", end + 1);
    end = end + 1;
    ReplaceMenso RM(ML);
    String replaced_menso = RM.Substitute(RM3.std_menso, RM1.std_menso, RM2.std_menso);

    ML.truths.FindAndDelIfAny(replaced_menso);
    ML.truths << replaced_menso;

    Space S(ML, end + 1);
    end = S.end;
}
