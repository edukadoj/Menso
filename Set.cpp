// #pragma hdrstop
#include "Set.h"
#include "Space.h"

void
Set::
Run(Unsigned offset)
{
    Unsigned thing_size = ML.things.Size();

    //new set object

    New N(ML);
    ThingOrOperator T = N.NewThing(offset); // This also adds T.thing to ML.things
    end = N.end;

    /*ReadingKind RK(ML, offset);
    String name = ML.deco[offset + 1];
    Thing T(name, RK.kind);

    if (ML.IsMultipleName(T.name) == true)
        ReportError("multiple name", offset + 1);
    ML.things << T.thing;
    if (RK.parens_found_for_main_type == true)  // if it has parameters
        end = RK.end;
    else
        end = RK.end + 1; // because in "prop p;" the end is "prop" */


    String to_be_LB = ML.text_item[end + 1];
    if (to_be_LB != "{")
        ReportError("'{'expected", end + 1);

    bool inside_space = ML.new_allowed;
    ML.new_allowed = false;
    Space S(ML, end + 2);
    ML.new_allowed = inside_space;
    end = S.end;

    Unsigned new_truth_size = ML.truths.Size();
    vector<Unsigned> v;
    for (Unsigned i = 0; i < new_truth_size; i++)
    {
        Replace R(ML.truths[i]);
        if (R.Contains(T.name))
        {
            v.push_back(i);
            //ML.truths << MakeAll(name, T.kind, ML.truths[i]);
            auto std_quan_variable = R.NewTempName();
            auto std_quanprop = R.Substitute(T.name, std_quan_variable);
            auto _menso = MakeAll(std_quan_variable, T.kind, std_quanprop);
            // ML.truths << menso;
            ML.AddTruthIfNotTrivial(_menso);
        }
    }
    Unsigned v_size = v.size();
    for (Unsigned i = v_size - 1; i != MAXUnsigned; i--)
    {
        ML.truths[v[i]] = 0;
    }
    ML.things[thing_size] = 0;

    Space SS(ML, end + 1);
    end = SS.end;
}
