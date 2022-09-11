// for replace statement

#ifndef ReplaceWithInH
#define ReplaceWithInH
#include "ReadingMenso.h"
#include "MensoCmp.h"
#include "CheckingMenso.h"


class ReplaceWithIn
{
    MainLists& ML;

    void Run(Unsigned offset);


public:
    Unsigned end;

    ReplaceWithIn(MainLists& ML, Unsigned offset)
        :ML(ML)
    {
        Run(offset);
    }
};


#endif
