// for reading a kind

#ifndef ReadingKindH
#define ReadingKindH
//#include "General.h"
#include "Common Files\\String.h"
#include "Common Files\\File.h"
#include "List.h"
#include "MainLists.h"

// this unit Gets the kind of a thing from the deco


class ReadingKind
{
private:
    MainLists& ML;
    Unsigned open_paren;
    //String return_type_part_of_kind;
    //String parameter_list_part_of_kind;
public:
    //bool parens_found_for_main_type;
    //bool parens_found_for_return_type;
    String kind;
    Unsigned end;        // offset of last deco in kind
private:
    /*void Finalize(Unsigned offset);

    void Initialize(Unsigned offset);

    void GetKind(Unsigned offset);*/

    void ReadKind(Unsigned offset);

public:

    ReadingKind(MainLists& ML, Unsigned offset);
};

/*
example:
    Decompose deco("G:\\Important Files\\Desktop\\Menso.txt");
    List types;
    types << "prop" << "object";
    GettingKind gk(deco.pack,types,1ULL);
*/


#endif
