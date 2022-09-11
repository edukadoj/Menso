// for add statement

#ifndef AddH
#define AddH
#include "Space.h"
#include "MainLists.h"
#include "ReadingMenso.h"
#include "AnalyzingMenso.h"

class Add
{
    MainLists& ML;

public:
    // String stdmenso; // menso is an object.

    void Run(Unsigned offset);

public:
    Unsigned end;

    Add(MainLists& ML, Unsigned offset)
        :ML(ML)
    {
        Run(offset);
    }
};

#endif
