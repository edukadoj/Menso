// for suppose statement


#ifndef SupposeH
#define SupposeH
#include "MainLists.h"
#include "ReadingMenso.h"
#include "CheckingMenso.h"


class Suppose
{
    MainLists& ML;

    void Run(Unsigned offset);

public:
    Unsigned end;

    Suppose(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};


#endif
