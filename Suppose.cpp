#pragma hdrstop
#include "Suppose.h"
#include "Space.h"


void
Suppose::
Run(Unsigned offset)
{
    CheckingMenso CM(ML);

    /*if (CM.IsTrue("false"))
        ReportError("'suppose' is not allowed because false is true", offset);*/

    List truths;
    truths << ML.truths;
    ReadingMenso RM(ML, offset);
    if (RM.kind != "prop")
        ReportError("menso must be a prop", RM.end);

    if (CM.IsTrue(RM.std_menso))
        ReportError("software can prove this menso, you don't need to suppose it", RM.end);
    ML.truths << RM.std_menso;
    end = RM.end;
    String to_be_LB = ML.text_item[end + 1];
    if (to_be_LB != "{")
        ReportError("'{' expected", end + 1);
    Space S(ML, end + 2);
    end = S.end;

    if (CM.IsTrue("false"))
    {
        ML.truths.Empty();
        ML.truths << truths;
        auto menso = CM.Negate(RM.std_menso);
        // ML.truths << menso;
        ML.AddTruthIfNotTrivial(menso);
    }
    else
    {
        Unsigned new_truth_size = ML.truths.Size();
        vector<Unsigned> v;
        MensoCmp MC(ML);
        for (Unsigned i = 0; i < new_truth_size; i++)
        {
            if (!truths.Contains(ML.truths[i]))
            {
                if (!MC.Equal(RM.std_menso, ML.truths[i])) // to prevent adding (if p)p
                {
                    auto menso = MakeIf(RM.std_menso, ML.truths[i]);
                    if (!truths.Contains(menso))
                        ML.truths << menso;
                    //ML.AddTruthIfNotTrivial(menso);
                    //ML.CarefulAddTruthDirectlyInTruthList(menso);
                }
                v.push_back(i);
            }
        }

        Unsigned size = v.size();
        //for (Unsigned i = 0; i < size; i++)
        //    ML.truths[v[i]] = 0;
        for(Unsigned i = size - 1; i != MAXUnsigned; i--)
            ML.truths[v[i]] = 0;
    }

    Space SS(ML, end + 1);
    end = SS.end;
}
