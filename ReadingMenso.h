// for reading a menso

#ifndef ReadingMensoH
#define ReadingMensoH
//#include "General.h"
#include "List.h"
#include "Thing.h"
#include "Common Files\\File.h"
#include "String.h"
#include "MainLists.h"
#include "AnalyzingKind.h"
#include "ReadingKind.h"
#include "Replace.h"
#include "New.h"

extern void ReportError(const char* c, Unsigned offset);

/*class ReadingSimpleMenso
{
    MensoList& ML;

    void Recursive(String kind, Unsigned offset);

    void Run(Unsigned offset);

public:
    String std_menso;
    String kind;
    Unsigned end; // last offset used

    void GetArgList_And_SetKind(String kind, Unsigned offset);

    ReadingSimpleMenso(MensoList& ML, Unsigned offset);

    ReadingSimpleMenso(MensoList& ML) // for just using GetArgList_And_SetKind method
        : ML(ML)
    {}
};*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/*class ReadingQuan
{
    MensoList& ML;

    void Run(Unsigned offset);
public:
    String std_menso;
    String kind;
    Unsigned end;


    ReadingQuan(MensoList& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*class ReadingIf
{
    MensoList& ML;


    void Run(Unsigned offset);
public:
    String std_menso;
    String kind;
    Unsigned end;


    ReadingIf(MensoList& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*class ReadingUnit // a unit is a smallest menso that can be read from an offset, e.g. x + y is not a unit, but (x + y) is a unit
{                 // in fact a unit is a menso with no outer binary operators
    MensoList& ML;
    void Run(Unsigned offset);

public:
    String std_menso;
    String kind;
    Unsigned end; // last offset used

    void GetArgList_And_SetKind(String kind, Unsigned offset);

    ReadingUnit(MensoList& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class ReadingMenso
{
    MainLists& ML;
    struct MensoKindEnd_ReturnType
    {
        String menso;
        String kind;
        Unsigned end;
    };
       
    MensoKindEnd_ReturnType GetArgList_And_Kind(const String& kind, Unsigned offset) const;
    
    MensoKindEnd_ReturnType ReadIf(Unsigned offset) const;
    
    MensoKindEnd_ReturnType ReadQuan(Unsigned offset) const;
    
    MensoKindEnd_ReturnType ReadUnit(Unsigned offset) const; // a unit is a smallest menso that can be read from an offset, e.g. x + y is not a unit, but (x + y) is a unit in fact a unit is a menso with no outer binary operators

    void ConstructMensoWithOperators(List& Units, List& Kinds, List& Operators);

    void Run(Unsigned offset);

public:
    String std_menso;
    String kind;
    Unsigned end; // last offset used

    ReadingMenso(MainLists& ML, Unsigned offset)
        : ML(ML)
    {
        Run(offset);
    }
};

#endif
