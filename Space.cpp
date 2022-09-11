#pragma hdrstop
#include "Space.h"

bool Space::no_adds = false;



void
Space::
CheckIfFalseIsTrue(Unsigned offset)
{
    CheckingMenso CM(ML);
    if (CM.EqualsSomethingInTruthList("false"))
        ReportError("'false' is true, no more commands are allowed in this space", offset);
}

void
Space::
Run(Unsigned offset)
{
    String gate = ML.text_item[offset];

    if (gate != "" && gate != "}" && gate != "check")
        CheckIfFalseIsTrue(offset);

    if (gate == "")
    {
        if (ML.new_allowed == false)
            ReportError("a set, suppose or select space is not closed yet", offset);
        end = offset;
        no_adds = false;
        return;
    }
    else if (gate == "new")
    {
        if (ML.new_allowed)
        {
            New N(ML, offset + 1);
            end = N.end;
        }
        else
            ReportError("'new' or 'define' is not allowed inside a set, suppose or select space", offset + 1);
    }
    else if (gate == "add")
    {
        if (no_adds == false)
        {
            Add A(ML, offset + 1);
            end = A.end;
        }
        else
            ReportError("no more 'add's can be used after 'noadds'", offset + 1);
    }
    else if (gate == "check")
    {
        Check C(ML, offset + 1);
        end = C.end;
    }
    else if (gate == "replace")
    {
        ReplaceWithIn RWI(ML, offset + 1);
        end = RWI.end;
    }
    else if (gate == "pick")
    {
        Pick P(ML, offset + 1);
        end = P.end;
    }
    else if (gate == "set")
    {
        Set S(ML, offset + 1);
        end = S.end;
    }
    else if (gate == "suppose")
    {
        Suppose S(ML, offset + 1);
        end = S.end;
    }
    else if (gate == "select")
    {
        Select S(ML, offset + 1);
        end = S.end;
    }
    else if (gate == "define")
    {
        if (ML.new_allowed)
        {
            Define D(ML, offset + 1);
            end = D.end;
        }
        else
            ReportError("'new' or 'define' is not allowed inside a set, suppose or select space", offset + 1);
    }
    else if (gate == "noadds")
    {
        no_adds = true;
        String to_be_semi = ML.text_item[offset + 1];
        if (to_be_semi != ";")
            ReportError("';'expected", offset + 1);
        Space SS(ML, offset + 2);
        end = SS.end;
    }
    else if (gate == "}")
    {
        end = offset;
        no_adds = false;
        return;
    }
    else
        ReportError("unexpected word", offset + 1);

    /*for(int i = 1; i < deco.GetNumberOfPacks(); i++)
        cout<< deco[i] << endl;
    cout<< deco.GetNumberOfPacks();// */
}


