// for running in a space (general or suppose or set space)


#ifndef SpaceH
#define SpaceH


//#include "Common Files\\General.h"
#include "List.h"
#include "Common Files\\File.h"
#include "Decompose.h"
#include "ReadingKind.h"
#include "CheckingName.h"
#include "Thing.h"
#include "New.h"
#include "Add.h"
#include "MainLists.h"
#include "Check.h"
#include "ReplaceWithIn.h"
#include "Pick.h"
#include "Set.h"
#include "Suppose.h"
#include "Select.h"
#include "Define.h"

//using namespace std;

class Space
{
    MainLists& ML;
    bool new_allowed;
    void Run(Unsigned offset);

    void CheckIfFalseIsTrue(Unsigned offset);

public:
    Unsigned end;
    static bool no_adds;

    Space(MainLists& ML, Unsigned start_offset)
        : ML(ML), new_allowed(new_allowed)
    {
        Run(start_offset);
    }
};

#endif