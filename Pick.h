// for pick statement

#ifndef PickH
#define PickH
#include "ReadingMenso.h"
#include "AnalyzingMenso.h"
#include "ReplaceMenso.h"
#include "MainLists.h"

class Pick
{
    MainLists& ML;
    void Run(Unsigned offset);

public:
    Unsigned end;
    Pick(MainLists& TL, Unsigned offset)
        :ML(TL)
    {
        Run(offset);
    }
};

#endif
