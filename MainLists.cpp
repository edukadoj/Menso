//#pragma hdrstop
#include "MainLists.h"
#include "Thing.h"
#include "ReadingMenso.h"
#include "AnalyzingMenso.h"
#include "CheckingMenso.h"

bool
MainLists::
IsMultipleName(String name)
{
    Unsigned size = things.Size();
    for (Unsigned i = 0; i < size; i++)
    {
        String thing_i = things[i];
        ThingOrOperator T_i(thing_i);
        if (T_i.name == name)
            return true;
    }
    return false;
}

bool
MainLists::
IsMultipleOperator(String name) const
{
    Unsigned size = operators.Size();
    for (Unsigned i = 0; i < size; i++)
    {
        String operator_i = operators[i];
        ThingOrOperator O_i(operator_i);
        if (O_i.name == name)
            return true;
    }
    return false;
}

MainLists::ErrorEnum
MainLists::
AddTruthIfNotTrivial(String menso) // add a truth to tuth only if it is not considered trivial
{
    auto& ML = *this;
    CheckingMenso CM(ML);

    if (CM.EqualsSomethingInTruthList(menso))
    {
        if(IsOriginalTruth(menso))
            return ErrorEnum::CONSIDERED_TRIVIUALLY_TRUE;
        return ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST;
    }

    AnalyzingMenso AM(ML, menso);
    if (AM.if_type)
    {
        if (CM.IsTrue(AM.part1))
        {
            auto error_code = AddTruthIfNotTrivial(AM.part2);
            if (error_code == ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST)
                return ErrorEnum::CONSIDERED_TRIVIUALLY_TRUE;
            return error_code;
        }
        else
        {
            MensoCmp MC(ML);
            if (MC.Equal(AM.part1, AM.part2))
                return ErrorEnum::CONSIDERED_TRIVIUALLY_TRUE;
        }
    }
    ML.truths << menso;
    return ErrorEnum::NO_ERRORS;
}
