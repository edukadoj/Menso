// for check statement

#ifndef CheckH
#define CheckH
#include "MainLists.h"
#include "ReplaceMenso.h"
#include "Check.h"
#include "MensoCmp.h"
#include "ReadingMenso.h"
#include "String.h"
#include "CheckingMenso.h"

class Check
{
    MainLists& ML;
    String menso;

    void Run(Unsigned offset);

public:
    Unsigned end;

    Check(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};

#endif
