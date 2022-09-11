// for new statement

#ifndef NewH
#define NewH


//#include "General.h"
#include "List.h"
#include "ReadingKind.h"
#include "String.h"
#include "CheckingName.h"
#include "Space.h"
#include "AnalyzingKind.h"
#include "MainLists.h"


class New
{
    MainLists& ML;

    // String New_(Unsigned offset);

    void NewOperator(Unsigned);

    void Run(Unsigned offset);

public:
    ThingOrOperator NewThing(Unsigned offset);

    Unsigned end;

    New(MainLists& ML, Unsigned offset);

    New(MainLists& ML) : // just to let use NewThing method
        ML(ML)
    {} 
};

#endif
