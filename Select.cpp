#include "Select.h"
#include "Space.h"
#include "ReplaceMenso.h"

void
Select::
Run(Unsigned offset)
{
    Unsigned thing_size = ML.things.Size();
 
    String name = ML.text_item[offset];
    if (ML.IsMultipleName(name) == true)
        ReportError("multiple name", offset);
    String to_be_in = ML.text_item[offset + 1];
    if (to_be_in != "in")
        ReportError("'in' expected", offset + 1);
    ReadingMenso RM(ML, offset + 2);
    if (RM.kind != "prop")
        ReportError("menso must be a prop", RM.end);
    
    AnalyzingMenso AM(ML, RM.std_menso);
    if(!AM.some_type)
        ReportError("prop must a some quantifier", RM.end);

    CheckingMenso CM(ML);
    if (!CM.IsTrue(RM.std_menso))
        ReportError("sofware failed to prove this some quantifier", RM.end);

    ThingOrOperator T(name, AM.variable_kind);
    ML.things << T.thing;

    ReplaceMenso RepM(ML);
    String replaced_menso = RepM.Substitute(AM.quanprop, AM.variable, name);

    ML.truths << replaced_menso;

    String to_be_LB_or_semi = ML.text_item[RM.end + 1];
    
    if (to_be_LB_or_semi == "{")
    {
        bool inside_space = ML.new_allowed;
        ML.new_allowed = false;
        Space S(ML, RM.end + 2);
        ML.new_allowed = inside_space;

        Unsigned new_truth_size = ML.truths.Size();
        vector<Unsigned> v;
        for (Unsigned i = 0; i < new_truth_size; i++)
        {
            Replace R(ML.truths[i]);
            if (R.Contains(name))
            {
                v.push_back(i);
                auto std_quan_variable = R.NewTempName();
                auto std_quanprop = R.Substitute(name, std_quan_variable);
                auto _menso = MakeSome(std_quan_variable, T.kind, std_quanprop);
                ML.AddTruthIfNotTrivial(_menso);
            }
        }
        Unsigned v_size = v.size();
        for (Unsigned i = v_size - 1; i != MAXUnsigned; i--)
        {
            ML.truths[v[i]] = 0;
        }
        ML.things[thing_size] = 0;

        Space SS(ML, S.end + 1);
        end = SS.end;
    }
    else if (to_be_LB_or_semi == ";") // if unique some no need to be a space
    {
        auto& std_quan_variable1 = AM.variable;
        auto& std_quanprop1 = AM.quanprop;

        Replace R(std_quanprop1);
        auto std_quan_variable2 = R.NewTempName();
        auto std_quanprop2 = R.Substitute(AM.variable, std_quan_variable2);

        auto if_part1 = MakeBinaryOp(std_quanprop1, "&", std_quanprop2); // String("(") + std_quanprop1 + "=" + std_quanprop2 + ")";
        auto if_part2 = MakeBinaryOp(std_quan_variable1, "=", std_quan_variable2); //String("(") + std_quan_variable1 + "=" + std_quan_variable2 + ")";

        auto std_quanprop = MakeIf(if_part1, if_part2);

        auto internal_new_quantifier = MakeAll(std_quan_variable1, T.kind, std_quanprop);
        auto external_new_quantifier = MakeAll(std_quan_variable2, T.kind, internal_new_quantifier);

        CheckingMenso _CM(ML);
        if (!_CM.IsTrue(external_new_quantifier))
            //ReportError("sofware failed to prove the uniqueness of this some quantifier", RM.end);
            ReportError(CS(external_new_quantifier), RM.end);

        Space S(ML, RM.end + 2);
        end = S.end;
    }
    else
        ReportError("'{' or ';' expected", end + 1);

}


