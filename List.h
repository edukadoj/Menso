// for list of Strings

// این unit یک لیست با قابلیت های خیلی خوب در اختیار
// ما میگذارد.

#ifndef ListH
#define ListH


#include <vector>
#include "Common Files\\General.h"
#include "Common Files\\String.h"


using namespace std;

class BaseList
{
private:
    typedef vector<String> DATA;
    typedef DATA::iterator ITERATOR;

    DATA data;

    void Reset();

public:
    void Add(String item);

    BaseList();

  /*  BaseList(const BaseList&)
    {
        print_error("List or BaseList cannot be copied");
    }*/

    void DeleteItem(Unsigned offset);

    void Empty();

    Unsigned GetSize() const;

    void Insert(Unsigned offset, String item);

    String GetItem(Unsigned offset) const;

    void Swap(Unsigned i, Unsigned j);


    void operator = (BaseList& )
    {
        print_error("using '=' for a List or BaseList is not allowed");
    }
};

//==============================================================================

class Item;

//-------------

class List : private BaseList
{
    friend class Item;
public:
    List();

    void Empty();

    Unsigned FindItemOffset(String item) const;

    bool FindAndDelIfAny(String item);

    bool Contains(String item) const;

    Unsigned Size() const;

    List& operator << (String item);

    List& operator << (List& l);

    List& operator = (int a);

    Item operator [](Unsigned offset);

    Item operator [](Unsigned offset) const;
};

//------------------------------------------------------------------------------

class Item
{
public:
    friend class List;
    List* list;
    const List* const_list;
    Unsigned offset;

    Item(List* list, Unsigned offset);

    Item(const List* list, Unsigned offset);

    operator String () const;

    Item& operator = (String item);

    Item& operator = (List& list);

    Item& operator = (int a);

    void operator ^ (Item item);
};

ostream& operator << (ostream&, Item item);

#endif


/* example:

    List l;
    l << "hello" << "222" << "333";
    l = 0;
    l<< "111" << "222";
    for(int i = 0; i < l.Size();i++)
        cout<< l[i] << endl;
*/
