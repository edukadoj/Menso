// این unit یک لیست با قابلیت های خیلی خوب در اختیار
// ما میگذارد.

#pragma hdrstop

#include "List.h"


void
BaseList::
Reset()
{
    data.resize(0);
}
//برای شروع باید اجرا شود

//__________________________________________________________________________

void
BaseList::
Add(String item)
{
    data.push_back(val(item));
}
// عضو جدید اضافه میکند
//__________________________________________________________________________
BaseList::
BaseList()
{
    Reset();
}
//constructor
//__________________________________________________________________________
void
BaseList::
DeleteItem(Unsigned offset)
{
    data.erase(data.begin() + val(offset));
}
// عضوی را که در محل پارامتر قرار دارد را حذف میکند
//__________________________________________________________________________
void
BaseList::
Empty()
{
    data.erase(data.begin(), data.end());
}
//_____________________________________________________________________________

Unsigned
BaseList::
GetSize() const
{
    return data.size();
}
// تعداد اعضا را برمیگرداند
//__________________________________________________________________________
void
BaseList::
Insert(Unsigned offset, String item)
{
    data.insert(data.begin() + val(offset), val(item));
}
// عضو جدید در محل آفست اضافه میکند
//__________________________________________________________________________
String
BaseList::
GetItem(Unsigned offset) const
{
    return data[offset];
}
// با گرفتن آفست عضوی را که درمکان آفست قراردادرد برمیگرداند
//__________________________________________________________________________
void
BaseList::
Swap(Unsigned i, Unsigned j)
{
    if (i == j)
        return;
    String temp = data[val(i)];
    data[val(i)] = data[val(j)];
    data[val(j)] = temp;
}
// اعضای آی و جی را عوض میکند

//==============================================================================

List::
List() : BaseList()
{
}

//__________________________________________________________________________

void
List::
Empty()
{
    BaseList::Empty();
}

//__________________________________________________________________________

Unsigned
List::
FindItemOffset(String item) const
{
    Unsigned i;
    Unsigned len = BaseList::GetSize();
    if (1 <= len)
        for (i = len - 1; i != MAXUnsigned; i--)
            if (val(item) == GetItem(val(i)))
                return val(i);
    return MAXUnsigned;
}
// offset عنصر item رو اگه پیدا کرد برمیگردونه
// اگه پیداش نکرد مقدار بی علامت -1 رو برمیگردونه
//__________________________________________________________________________
bool
List::
FindAndDelIfAny(String item)
{
    Unsigned offset = FindItemOffset(val(item));
    if (val(offset) == MAXUnsigned)
        return false;
    DeleteItem(val(offset));
    return true;
}
// دنبال عنصر item میگرده
// اگه پیداش نکرد false برمیگردونه
// اگه پیداش کرد حذفش میکنه و true برمیگردونه
//__________________________________________________________________________
bool
List::
Contains(String item) const
{
    Unsigned offset = FindItemOffset(val(item));
    if (val(offset) == MAXUnsigned)
        return false;
    return true;
}
//میبینه آیا عنصر item در لیست هست یا نه
//__________________________________________________________________________

Unsigned
List::
Size() const
{
    return GetSize();
}

//__________________________________________________________________________
List&
List::
operator << (String item)
{
    Add(val(item));
    return *this;
}
//این عملگر برای اضافه کردن عضو جدید به کار میرود
    //__________________________________________________________________________
List&
List::
operator << (List& l)
{
    Unsigned size = l.Size();
    for (Unsigned i = 0; i < size; i++)
        Add(l[i]);
    return *this;
}
//__________________________________________________________________________
List&
List::
operator = (int a)
{
    if (val(a) != 0)
        print_error("List::operator = (int a)\n"
            "a must be 0\n");
    Empty();
    return *this;
}

//__________________________________________________________________________

Item
List::
operator [](Unsigned offset)
{
#ifdef _DEBUG
    Unsigned size = Size();
    if (val(offset) >= val(size))
        print_error("Item List::operator [](Unsigned offset)\n"
            "bad offset for a list object");
#endif
    return Item(this, val(offset));
}
// عنصر در مکان offset را برمیگرداند

Item
List::
operator [](Unsigned offset) const
{
#ifdef _DEBUG
    Unsigned size = Size();
    if (val(offset) >= val(size))
        print_error("Item List::operator [](Unsigned offset)\n"
            "bad offset for a list object");
#endif
    return Item(this, val(offset));
}
//------------------------------------------------------------------------------

Item::
Item(List* list, Unsigned offset)
    : list(val(list)), const_list(val(list)), offset(val(offset))
{}
//constructor

Item::
Item(const List* list, Unsigned offset)
    : list(0), const_list(val(list)), offset(val(offset))
{}
//constructor


//__________________________________________________________________________
Item::
operator String () const
{
    return const_list->GetItem(val(offset));
}
// کمک می کند که Item به String تبدیل بشود
//__________________________________________________________________________
Item&
Item::
operator = (String item)
{
#ifdef _DEBUG
    if (!list)
        print_error("const Item class called a non_const method");
#endif
    *this = 0; // item را حذف میکند.
    list->Insert(val(offset), val(item));
    return *this;
}
// مقدار یک عنصر لیست را با مقدار جدید عوض میکند

//__________________________________________________________________________

Item&
Item::
operator = (List& l)
{
#ifdef _DEBUG
    if (!list)
        print_error("const Item class called a non_const method");
#endif

    Unsigned size = l.Size();
    for (Unsigned i = 0; i < size; i++)
        list->Insert(offset + i, l[i]);
    return *this;
}

//__________________________________________________________________________
Item&
Item::
operator = (int a)
{
#ifdef _DEBUG
    if (!list)
        print_error("const Item class called a non_const method");
#endif

    if (val(a) != 0)
        print_error("Item::operator = (int a)\n"
            "a must be 0\n");
    list->DeleteItem(val(offset));
    return *this;
}
// وقتی a صفر باشد عنصر زا حذف میکند

//__________________________________________________________________________
void
Item::
operator ^ (Item item)
{
#ifdef _DEBUG
    if (!list)
        print_error("const Item class called a non_const method");
#endif

    if (val(list) != val(item.list))
        print_error("Item::operator^(Item item)\n"
            "lists must be the same");
    list->Swap(val(offset), val(item.offset));
}
// جای دو عنصر را در لیست عوض میکند



ostream& operator << (ostream&, Item item)
{
    cout << (String)val(item);
    return cout;
}
//برای پرینت کردن آیتمها
