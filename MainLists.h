// providing main lists for most classes

#ifndef MensoListH
#define MensoListH

//#include "Common Files\\General.h"
#include "Decompose.h"
#include "List.h"

class MainLists
{
    Decompose decodeco;
public:
    Pack& text_item;
    List types;
    List things;
    List operators;
    List truths;
    enum class ErrorEnum {NO_ERRORS, EQUAL_TO_SOMETHING_IN_TRUTH_LIST, CONSIDERED_TRIVIUALLY_TRUE};

    bool new_allowed;

    bool IsMultipleName(String name);

    bool IsMultipleOperator(String name) const;

    bool IsOriginalTruth(const String& std_menso)
    {
        return std_menso == "true" || std_menso == "(!false)" || std_menso == "(true = (!false))" || std_menso == "((!true)=false)";
    }

    ErrorEnum AddTruthIfNotTrivial(String menso); // add a truth to tuth only if it is not considered trivial // used by add command

    void CarefulAddTruthDirectlyInTruthList(const String& std_menso) // used by check command
    {
        if (!IsOriginalTruth(std_menso))
            truths.FindAndDelIfAny(std_menso);
        auto error_code = AddTruthIfNotTrivial(std_menso);
        if (error_code == MainLists::ErrorEnum::EQUAL_TO_SOMETHING_IN_TRUTH_LIST)
            truths << std_menso; // exact add
    }

    MainLists(WString menso_file_address)
        : decodeco(menso_file_address), text_item(decodeco.pack)
    {
        new_allowed = true;

        types << "prop" << "object";
        operators << "=`prop*(.,.)"
            << "&`prop*(prop,prop)"
            << "|`prop*(prop,prop)"
            << "!`prop*(prop)";
        things << "true`prop" << "false`prop";
        truths << "true" << "(!false)"
            << "(true=(!false))" << "((!true)=false)";
    }

    MainLists(const MainLists&) : decodeco(""), text_item(decodeco.pack)
    {
        print_error("MensoList object cannot be copied");
    }
};

#endif
