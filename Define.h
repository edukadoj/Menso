#pragma once

#include "MainLists.h"
#include "ReadingKind.h"
#include "Thing.h"
#include "Replace.h"

class Define
{
    MainLists& ML;
    String menso;

    void Run(Unsigned offset);

public:
    Unsigned end;

    Define(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};

