#pragma hdrstop
#include "Add.h"

void
Add::
Run(Unsigned offset)
{
    ReadingMenso RM(ML, offset);
    if (RM.kind != "prop")
        ReportError("only props can be added", RM.end);
    
    auto menso = RM.std_menso;

    auto error_code = ML.AddTruthIfNotTrivial(menso);
    if(error_code == MainLists::ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST)
        ReportError("you cannot add already true props, use 'check' instead", RM.end);
    else if (error_code == MainLists::ErrorEnum::CONSIDERED_TRIVIUALLY_TRUE)
        ReportError("So trivial to be added", RM.end);

    String to_be_semi = ML.text_item[RM.end + 1];
    if (to_be_semi != ";")
        ReportError("';'expected", RM.end + 1);

    //writef((CS)(String)AM.quanprop);
    Space S(ML, RM.end + 2);
    end = S.end;
}
