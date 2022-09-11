// for set statement

#ifndef SetH
#define SetH
#include "MainLists.h"
#include "ReadingKind.h"
#include "Thing.h"
#include "Replace.h"

class Set
{
    MainLists& ML;
    String menso;

    void Run(Unsigned offset);

public:
    Unsigned end;

    Set(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};

#endif
