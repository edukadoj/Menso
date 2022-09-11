#ifndef SelectH
#define SelectH
#include "MainLists.h"
#include "ReadingKind.h"
#include "Thing.h"
#include "Replace.h"

class Select
{
    MainLists& ML;
    String menso;

    void Run(Unsigned offset);

public:
    Unsigned end;

    Select(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};

#endif
