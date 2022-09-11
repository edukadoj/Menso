#include "Define.h"
#include "Space.h"
#include "ReplaceMenso.h"

void
Define::
Run(Unsigned offset)
{
    //Unsigned thing_size = ML.things.Size();

    String to_be_name = ML.text_item[offset];

    CheckingName CN(to_be_name);
    if (!CN.is_legal)
        ReportError("a legal name expected", offset);

    if (ML.IsMultipleName(to_be_name) == true)
        ReportError("multiple name", offset);
    String to_be_in = ML.text_item[offset + 1];
    if (to_be_in != "=")
        ReportError("'=' expected", offset + 1);
    ReadingMenso RM(ML, offset + 2);
    ThingOrOperator T(to_be_name, RM.kind);
    ML.things << T.thing;
    ML.truths << String("(") + to_be_name + "=" + RM.std_menso + ")";

    String to_be_semi = ML.text_item[RM.end + 1];
    if (to_be_semi != ";")
        ReportError("';'expected", RM.end + 1);
    Space S(ML, RM.end + 2);
    end = S.end;
}
