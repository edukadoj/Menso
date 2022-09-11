// WString.h version 2.00
// you can use this header file in non malicious softwares freely
// copyright © 2010, all rights reserved

#ifndef WStringH
#define WStringH
#include "General.h"
#include "String.h"

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#define itow _itow_s
#define _itow _itow_s
#endif






//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline void DataCopy(wchar_t* buffer, const wchar_t* data, Unsigned len)
{
    Unsigned i;
    for (i = 0; i < len; i++)
    {
        try
        {
            buffer[i] = data[i];
        }
        catch (...) { }
    }
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
inline int CompareWStrings(const wchar_t* const& string1, const Unsigned& len1, const wchar_t* const& string2, const Unsigned& len2)
{
    Unsigned len;
    if (len1 < len2)
        len = len1;
    else
        len = len2;
    Unsigned i;
    for (i = 0; i < len; i++)
    {
        if ((Unsigned)string1[i] < (Unsigned)string2[i])
            return -1;
        if ((Unsigned)string1[i] > (Unsigned)string2[i])
            return 1;
    }
    if (len1 < len2)
        return -1;
    if (len1 > len2)
        return 1;
    return 0;    // they are equal
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
inline wchar_t* newWStr(Unsigned length)
{
    // create new string in heap with length 'length'
    // including null. this will be returned:
    wchar_t* ret = new wchar_t[length + 1];
    if (ret == 0)
        print_error(L"Out of Memroy. newWStr()");
    return ret;
}// newWStr(Unsigned length)
/*
    this version of newWStr creates a c style string in the heap with length
    'length'. it gets & returns length+1 bytes for the string & its null.
    Note that newWStr(0) creates 1 byte in heap.
*/

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline wchar_t* newWStr(const wchar_t* c_style_string, Unsigned length)
{
    Unsigned len;
    if (length == MAXUnsigned)
        len = wcslen(c_style_string);// length of string (excluding null).
    else
        len = length;
    wchar_t* ret = newWStr(len);
    DataCopy(ret, c_style_string, len + 1);
    return ret;
}

inline void delWStr(wchar_t* filled_c_style_string_to_be_deleted)
{
    delete[] filled_c_style_string_to_be_deleted;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class WStringBase
{
private:
    Unsigned length; // private
    wchar_t* string; // private

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
protected:
    wchar_t* const& GetWString() const
    {
        return string;
    }
    //--------------------------------------------------------------------------
    inline void MakeLengthInvalid()               //protected
    {
        length = MAXUnsigned;
    }
    //--------------------------------------------------------------------------
    inline void SetWString(wchar_t* c_style_string, Unsigned len)      //protected
    {
        string = c_style_string;
        length = len;
    }
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:
    //--------------------------------------------------------------------------
    inline const Unsigned& Size() const
    {
        return length;
    }

protected:
    WStringBase()
    {}
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class WString : public WStringBase
{
protected:

    class SafeChar;

    friend class SafeChar;

    bool is_memory_allocated;
    //--------------------------------------------------------------------------
    void ChangeWStringInto(wchar_t* c_style_string, Unsigned len) //protected
    {
        wchar_t* aCopy = newWStr(c_style_string, len);
        // note that it is possible that c_style_string==string
        // or c_style_string=string+1. So Destroy may delete c_style_string.
        // this is because we make a copy.
        // string is the class Main WString.
        Destroy();// deletes string. CANNOT BE USED SOONER
        SetWString(aCopy, len);
        is_memory_allocated = true;
    }
    //--------------------------------------------------------------------------
    void Destroy()
    {
        wchar_t* c = GetWString();
        delWStr(c);
        MakeLengthInvalid();
        is_memory_allocated = false;
    }
    //--------------------------------------------------------------------------
    void IfNotConstructedThrow() const
    {
        if (is_memory_allocated == false)
            Throw(L"WString:IfNotConstructedThrow():Use of Not Constructed Or Destroyed WString Object.\n");
    }
    //--------------------------------------------------------------------------
    void Throw(const wchar_t* const& message) const     //protected
    {
        print_error(message);//message);
    }
    //--------------------------------------------------------------------------
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:

    WString CutOff(Unsigned offset)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned len = Size();

        if (len <= offset) // if offset is inappropiate throw
            Throw(L"WString: CutOff1:Bad offset.\n");

        wchar_t* tempCopy = GetCStyle();
        wchar_t* last_part_of_string = &(tempCopy[offset + 1]);

        ChangeWStringInto(last_part_of_string, len - offset - 1);
        tempCopy[offset] = 0;
        WString ret(tempCopy, offset);
        delWStr(tempCopy);
        return ret;
    }
    //--------------------------------------------------------------------------
    WString CutOff(const WString& CharSet)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        // supose '*this' is "hello" & CharSet is "l"

        Unsigned first_part_len = nthOccurrence(CharSet);

        // the length of part that to be cut  (2 for "he");

        if (first_part_len == MAXUnsigned) // if nothing occurred
        {
            WString ret = *this;       // make a copy of this object
            *this = L"";                  // empty this object
            return ret;                  // return the copy
        }

        return CutOff((Unsigned)first_part_len); //cut return the cut part
    }
    //--------------------------------------------------------------------------
    WString CutOff(wchar_t terminating_wchar_t)
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        return CutOff((WString)terminating_wchar_t); //cut return the cut part
    }
    //--------------------------------------------------------------------------
    WString MultiCutOff(const WString& ExSub)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        Unsigned pos1 = MAXUnsigned, pos2;

        wchar_t* c1 = this->GetWString();
        wchar_t* c2 = ExSub.GetWString();

        Unsigned len1 = this->Size();
        Unsigned len2 = ExSub.Size();

        bool is_consecutive = true;

        while (true)// to be breaked
        {
            if (len1 <= i)
                break;
            if (len2 <= j)
                break;

            if (c2[j] == c1[i]) // if c2[j] was found
            {
                if (j == 0)
                {
                    is_consecutive = true;
                    pos1 = i;
                }
                j++;
                i++;
            }
            else // if c2[j] was not found
            {
                if (pos1 != MAXUnsigned)
                {
                    i = pos1;
                    j = 0;
                    pos1 = MAXUnsigned;
                }
                i++;    // search next block to find it
                is_consecutive = false;
                // not consecutive (if j==len2, this line is not reached)
            }
        }// while(1)

        if (is_consecutive == true)
            if (j == len2)   // if all were found
            {
                WString ret = this->CutOff((Unsigned)pos1);
                pos2 = ExSub.Size() - 2;
                if (pos2 != MAXUnsigned)
                    CutOff((Unsigned)pos2);
                return ret;
            }
        // if not found
        WString ret = *this;
        *this = L""; // empty this
        return ret; // cut all
    }
    //--------------------------------------------------------------------------
    wchar_t* GetCStyle() const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        wchar_t* c = GetWString();
        return newWStr(c, Size());
    }

    //--------------------------------------------------------------------------
    Unsigned HowManyOccurrences(const WString& CharSet)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned total_len = Size();// the lenght of 'this->string'
        wchar_t* c = GetWString();
        Unsigned i;
        Unsigned times_found = 0; // shows how many times ch is encountered
        for (i = 0; i < total_len; i++)
        {
            bool is_c_ret_a_wcharacter_in_CharSet = CharSet.IsSuperstring(c[i]);

            if (is_c_ret_a_wcharacter_in_CharSet)
                times_found++; // add 1 since ch is found once more
        }// for
        return times_found;
    }
    //--------------------------------------------------------------------------
    bool IsExactSuperstring(const WString& ExSub) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        wchar_t* c1 = this->GetWString();
        wchar_t* c2 = ExSub.GetWString();

        Unsigned len1 = this->Size();
        Unsigned len2 = ExSub.Size();

        Unsigned pos1 = MAXUnsigned;

        bool is_consecutive = true;

        while (true)// to be breaked
        {
            if (len1 <= i)
                break;
            if (len2 <= j)
                break;

            if (c2[j] == c1[i]) // if c2[j] was found
            {
                if (j == 0)
                {
                    pos1 = i;
                    is_consecutive = true;
                }
                j++;
                i++;
            }
            else // if c2[j] was not found
            {
                if (pos1 != MAXUnsigned)
                {
                    i = pos1;
                    j = 0;
                    pos1 = MAXUnsigned;
                }
                i++;    // search next block to find it
                is_consecutive = false;
                // not consecutive (if j==len2, this line is not reached)
            }
        }// while(1)

        if (is_consecutive == true)
            if (j == len2)   // if all were found
                return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool IsSuperset(const WString& CharSet) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        wchar_t* c1 = this->GetWString();
        wchar_t* c2 = CharSet.GetWString();

        Unsigned len1 = this->Size();
        Unsigned len2 = CharSet.Size();

        while (true)// to be breaked
        {
            if (len1 <= i)
                break;
            if (len2 <= j)
                break;

            if (c2[j] == c1[i]) // if c2[j] was found
            {
                j++;  //search c2[j+1]
                i = 0; //seach from the beginning of c1;
            }
            else        // if c2[j] was not found
                i++;    // search next block to find it
        }// while(1)

        if (j == len2)   // if all were found
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool IsSuperstring(const WString& SubStr) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        wchar_t* c1 = this->GetWString();
        wchar_t* c2 = SubStr.GetWString();

        Unsigned len1 = this->Size();
        Unsigned len2 = SubStr.Size();

        while (true)// to be breaked
        {
            if (len1 <= i)
                break;
            if (len2 <= j)
                break;
            if (c2[j] == c1[i]) // if c2[j] was found
            {
                j++;
                i++;
            }
            else        // if c2[j] was not found
                i++;    // search next block to find it
        } // while(true)

        if (j == len2)   // if all were found
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool Intersect(const WString CharSet)
    {
        Unsigned size = Size();
        for (Unsigned i = 0; i < size; i++)
        {
            if (CharSet.IsExactSuperstring(GetWString()[i]))
                return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------
    Unsigned nthOccurrence(const WString& CharSet, const Unsigned n = 1) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned total_len = Size();// the size of 'this->string'
        wchar_t* c = GetWString();

        Unsigned ret;
        Unsigned times_found = 0; // shows how many times ch is encountered

        for (ret = 0; ret < total_len; ret++)
        {
            bool is_c_ret_a_wcharacter_in_CharSet = CharSet.IsSuperstring(c[ret]);

            if (is_c_ret_a_wcharacter_in_CharSet)
            {
                times_found++; // add 1 since ch is found once more
                if (times_found == n)// if this is the nth occurrence
                    return ret;
            }
        } // for
        return MAXUnsigned; // there is no nth wchar_t
    }
    //--------------------------------------------------------------------------
    void Reverse()
    {
        wchar_t* c = GetWString();
        Unsigned len = Size();
        Unsigned half_len = len / 2;

        Unsigned i;
        for (i = 0; i < half_len; i++)
        {
            wchar_t temp = c[i];
            c[i] = c[len - 1 - i];
            c[len - 1 - i] = temp; // c[i] & c[len - 1 - i] are swapped
        }
    }
    //--------------------------------------------------------------------------
    void ToLower()
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        wchar_t* c = GetWString();
        Unsigned len = Size();
        Unsigned i;
        for (i = 0; i < len; i++)
            if ('A' <= c[i])
                if (c[i] <= 'Z')        // if c[i] is uppercase
                    c[i] = (wchar_t)tolower(c[i]); // make it lower case
    }
    //--------------------------------------------------------------------------
    void ToUpper()
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        wchar_t* c = GetWString();
        Unsigned len = Size();
        Unsigned i;
        for (i = 0; i < len; i++)
            if ('a' <= c[i])
                if (c[i] <= 'z')        // if c[i] is lowercase
                    c[i] = (wchar_t)toupper(c[i]); // make it upper case
    }
    //--------------------------------------------------------------------------
    int Int()
    {
        wchar_t* c = GetWString(); // protecting code
        Unsigned len = Size();    // protecting code
        Unsigned i;
        for (i = 0; i < len; i++)  // protecting code
            if (!(L'0' <= c[i] && c[i] <= L'9'))
            {
                if (i == 0)
                    if (c[i] == '-')
                        continue;
                //print_error("String::Int(): not number");
                throw 1;
            }
        return _wtoi(GetWString());
    }
    //--------------------------------------------------------------------------
    WString& operator = (wchar_t* c_style_string)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        ChangeWStringInto(c_style_string, wcslen(c_style_string));
        return *this;
    }
    //--------------------------------------------------------------------------
    WString& operator = (const WString& Str)
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        wchar_t* c = Str.GetWString();
        //wchar_t* c2 = this->GetWString();
        ChangeWStringInto(c, Str.Size());
        return *this;
    }
    //--------------------------------------------------------------------------
    bool operator == (const WString& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        wchar_t* c1 = this->GetWString();
        Unsigned len1 = this->Size();
        wchar_t* c2 = Str.GetWString();
        Unsigned len2 = Str.Size();

        if (CompareWStrings(c1, len1, c2, len2) == 0)  // if they are equal
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator != (const WString& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const wchar_t* const& c1 = this->GetWString();
        const Unsigned& len1 = this->Size();
        const wchar_t* const& c2 = Str.GetWString();
        const Unsigned& len2 = Str.Size();

        if (CompareWStrings(c1, len1, c2, len2) == 0)  // if they are equal
            return false;
        return true;
    }
    //--------------------------------------------------------------------------
    bool operator < (const WString& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const wchar_t* const& c1 = this->GetWString();
        const Unsigned& len1 = this->Size();
        const wchar_t* const& c2 = after_op.GetWString();
        const Unsigned& len2 = after_op.Size();

        if (CompareWStrings(c1, len1, c2, len2) < 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator > (const WString& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        const wchar_t* const& c1 = this->GetWString();
        const Unsigned& len1 = this->Size();
        const wchar_t* const& c2 = after_op.GetWString();
        const Unsigned& len2 = after_op.Size();

        if (CompareWStrings(c1, len1, c2, len2) > 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator <= (const WString& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const wchar_t* const& c1 = this->GetWString();
        const Unsigned& len1 = this->Size();
        const wchar_t* const& c2 = after_op.GetWString();
        const Unsigned& len2 = after_op.Size();

        if (CompareWStrings(c1, len1, c2, len2) <= 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator >= (const WString& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const wchar_t* const& c1 = this->GetWString();
        const wchar_t* const& c2 = after_op.GetWString();

        if (wcscmp(c1, c2) >= 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    WString::SafeChar operator [] (Unsigned i);
    //--------------------------------------------------------------------------
    WString& operator += (const WString& Str)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const Unsigned& len1 = Size();
        const Unsigned& len2 = Str.Size();

        const Unsigned& total_length = len1 + len2;//the sum of lengths

        wchar_t* sum_string = newWStr(total_length);
        // create the S style sring to contain both.

        wchar_t* c1 = GetWString();
        wchar_t* c2 = Str.GetWString();

        DataCopy(sum_string, c1, len1);
        // add the first string to ret.

        DataCopy(&sum_string[len1], c2, len2 + 1);
        // add the second string (including terminating null)

        Destroy();// deletes string
        SetWString(sum_string, total_length);

        is_memory_allocated = true;// Destroy set it to false
        return *this;
    }
    //--------------------------------------------------------------------------
    WString operator + (const WString& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        WString ret;// an empty WString
        ret += *this; // add hkStr1
        ret += Str; // add hkStr2
        return ret;
    }

    operator String()
    {
        const WString& WStr = *this;
        wchar_t* wc = WStr.GetCStyle();
        Unsigned size = WStr.Size();
        char* c = new char[size + 1];
        for (Unsigned i = 0; i <= size; i++)
            c[i] = (decltype(c[i]))wc[i];
        String ret(c, size);
        delete[] wc;
        delete[] c;
        return ret;
    }
    //--------------------------------------------------------------------------
    WString()
    {
        wchar_t* c = newWStr(L"", 0);
        SetWString(c, 0);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    WString(const wchar_t* c_style_string)
    {
        wchar_t* c = newWStr(c_style_string, wcslen(c_style_string));
        SetWString(c, wcslen(c));
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    WString(const char* normal_string)
    {
        Unsigned len = strlen(normal_string);
        wchar_t* c = new wchar_t[len + 1];
        for (Unsigned i = 0; i <= len; i++)
            c[i] = normal_string[i];
        SetWString(c, len);
        is_memory_allocated = true;
    }
    //--------------------------------------------------------------------------
    WString(int x)
    {
        wchar_t a[50];
        _itow(x, a, 10);
        Unsigned len = wcslen(a);
        wchar_t* c = new wchar_t[len + 1];
#if defined (_MSC_VER) && (_MSC_VER >= 1020)
        wcscpy_s(c, len + 1, a);
#else
        wcscpy(c, a);
#endif
        SetWString(c, len);
        is_memory_allocated = true;
    }

    //--------------------------------------------------------------------------
    WString(wchar_t* data, Unsigned len)
    {
        wchar_t* c = newWStr(len);
        DataCopy(c, data, len);
        c[len] = 0; // null terminate
        SetWString(c, len);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    WString(wchar_t wch)
    {
        wchar_t a[2];
        a[0] = wch;
        a[1] = 0;     // e.g. if ch=='e' then a is "e"
        wchar_t* c = newWStr(a, 1);
        SetWString(c, 1);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    WString(String Str)
    {
        char* c = Str.GetCStyle();
        Unsigned size = Str.Size();
        wchar_t* wc = new wchar_t[size + 1];
        for (Unsigned i = 0; i <= size; i++)
            wc[i] = c[i];
        SetWString(wc, size);
        delete[] c;
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    WString(SafeChar safe_ch);
    //--------------------------------------------------------------------------
    WString(const WString& right_hand_side)
    {
        wchar_t* c = newWStr(L"", 0);
        SetWString(c, 0);
        is_memory_allocated = true; // oblect is created  (required for =)
        *this = right_hand_side;
    }
    //--------------------------------------------------------------------------
    ~WString()
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        Destroy();
    }
};
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class WString::SafeChar
{
private:
    WString* This;
    Unsigned i;
public:
    SafeChar(WString* pointer_to_WStr, Unsigned offset)
    {
        This = pointer_to_WStr;
        i = offset;
    }
    //--------------------------------------------------------------------------
    SafeChar& operator = (wchar_t ch)
    {
        (This->GetWString())[i] = ch;
        return *this;
    }
    //--------------------------------------------------------------------------
    SafeChar& operator = (const WString& WStr)
    {
        *This = (This->CutOff((Unsigned)i)) +
            (WStr)+
            (*This);
        return *this;
    }
    //--------------------------------------------------------------------------
    operator wchar_t()
    {
        return (This->GetWString())[i];
    }
    //--------------------------------------------------------------------------
    void operator delete (void*)
    {
        print_error(L"WString:operator delete (void*):if s is an WString. &(s[i]) cannot be deleted.\n");
    }
    //--------------------------------------------------------------------------
    void operator delete [](void*)
    {
        print_error(L"WString:operator delete [] (void*):if s is an WString. &(s[i]) cannot be delete[]d.\n");
    }
        //--------------------------------------------------------------------------
    WString operator [] (Unsigned j) const
    {
        if ((This->Size() <= j) || (j < i))
            return L"";
        wchar_t* c = This->GetWString();
        return WString(c + i, j - i + 1);
    }
};


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


class WCS
{
public:
    wchar_t* string;
    WCS(const WString& WStr)
    {
        string = WStr.GetCStyle();
    }
    ~WCS()
    {
        delWStr(string);
    }
    operator wchar_t*& ()
    {
        return string;
    }
};


inline WString::WString(SafeChar safe_ch)
{
    wchar_t a[2];
    a[0] = (wchar_t)safe_ch;
    a[1] = 0; // e.g. if ch == 'e' then a is "e"
    wchar_t* c = newWStr(a, 1);
    SetWString(c, 1);
    is_memory_allocated = true; // oblect is created
}

inline WString::SafeChar WString::operator [] (Unsigned i)
{
    if (Size() <= i) // if i is not well defined
        Throw(L"WString:operator [] (Unsigned i):Bad Offset.");
    SafeChar safe_wchar_t(this, i);
    return safe_wchar_t;
}

//======================================

template<typename type>
String ToData(type x)// converts x to String
{
#ifdef __BORLANDC__
#pragma option -w-ccc
#pragma option -w-rch
#endif
    if constexpr(sizeof(type) % sizeof(char) != 0)
        static_assert(L"bad type");
    union
    {
        type typee;
        char a[sizeof(type) / sizeof(char)];
    };
    typee = x;
    return String(a, sizeof(type) / sizeof(char));
}

//--------------------------------------

template<typename type>
type ToType(String data)
{
#ifdef __BORLANDC__
#pragma option -w-ccc
#pragma option -w-rch
#endif
    if constexpr(sizeof(type) % sizeof(char) != 0)
        static_assert(L"bad type");

    union
    {
        type ret;
        char a[sizeof(type) / sizeof(char)];
    };
    for (Unsigned i = 0; i < sizeof(type) / sizeof(char); i++)
        a[i] = data[i];

    return ret;
}

typedef void (*ACTION)(WString data);
inline void ZERO(WString) {}


#endif