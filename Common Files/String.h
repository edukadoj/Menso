#ifndef StringH
#define StringH

#include "General.h"

#include <iostream>
#include <stdlib.h>


#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#define itoa _itoa_s
#endif

/*
inline void print_error1(char* message)
{
#ifdef _DEBUG
    MessageBoxA(0, message,message, MB_OK);
#endif
}
*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
inline void DataCopy(char* buffer, const char* data, Unsigned len)
{
    Unsigned i;
    for (i = 0; i < len; i++)
        buffer[i] = data[i];
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
inline int CompareStrings(const char* string1, Unsigned len1, const char* string2, Unsigned len2)
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
    return 0; // they are equal
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline char* newStr(Unsigned length)
{
    // create new string in heap with length 'length'
    // including null. this will be returned:

    char* ret = 0;
    try
    {
        ret = new char[length + 1];
    }
    catch (...)
    {
        print_error("Memroy Allocation Error. newStr()");
    }
    return ret;
}// newStr(Unsigned length)

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline char* newStr(const char* c_style_string, Unsigned length)
{
    Unsigned len;

    if (length == MAXUnsigned)
        len = strlen(c_style_string);// length of string (excluding null).
    else
        len = length;
    char* ret = newStr(len);
    DataCopy(ret, c_style_string, len + 1);
    return ret;
}

inline void delWStr(char* filled_c_style_string_to_be_deleted)
{
    delete[] filled_c_style_string_to_be_deleted;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class StringBase
{
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
private:
    Unsigned length; // private
#ifdef _DEBUG
protected:
#endif
    char* string; // private (in debug mode protected)

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
protected:
    char* const& GetString() const
    {
        return string;
    }
    //--------------------------------------------------------------------------
    StringBase()
    {}
    //--------------------------------------------------------------------------
    inline void MakeLengthInvalid()               //protected
    {
        length = MAXUnsigned;
    }
    //--------------------------------------------------------------------------
    inline void SetString(char* c_style_string, Unsigned len)      //protected
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
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class String : public StringBase
{
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
private:
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
protected:

    class SafeChar;

    friend class SafeChar;

    bool is_memory_allocated;
    //--------------------------------------------------------------------------
    void ChangeStringInto(char*& c_style_string, Unsigned len) //protected
    {
        char* aCopy = newStr(c_style_string, len);
        // note that it is possible that c_style_string==string
        // or c_style_string=string+1. So Destroy may delete c_style_string.
        // this is because we make a copy.
        // string is the class Main String.
        Destroy();// deletes string. CANNOT BE USED SOONER
        SetString(aCopy, len);
        is_memory_allocated = true;
    }
    //--------------------------------------------------------------------------
    void Destroy()
    {
        char* c = GetString();
        delWStr(c);
        MakeLengthInvalid();
        is_memory_allocated = false;
    }
    //--------------------------------------------------------------------------
    inline void IfNotConstructedThrow() const
    {
        if (is_memory_allocated == false)
            Throw("String:IfNotConstructedThrow() const:Use of Not Constructed Or Destroyed String Object.\n");
    }
    //--------------------------------------------------------------------------
    inline void Throw(const char* const& message) const // protected
    {
        print_error(message);
    }
    //--------------------------------------------------------------------------
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
public:

    String CutOff(Unsigned offset)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        if (offset == MAXUnsigned)
        {
            String ret = *this;       // make a copy of this object
            *this = L"";              // empty this object
            return ret;               // return the copy
        }

        Unsigned len = Size();

        if (len <= offset)   // if offset is inappropiate throw
            Throw("String:CutOff(Unsigned offset): Bad offset in String::CufOff.\n");

        char* tempCopy = GetCStyle();
        char* last_part_of_string = &(tempCopy[offset + 1]);

        ChangeStringInto(last_part_of_string, len - offset - 1);
        tempCopy[offset] = 0;
        String ret(tempCopy, offset);
        delWStr(tempCopy);
        return ret;
    }
    //--------------------------------------------------------------------------
    String CutOff(const String& CharSet)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        // supose '*this' is "hello" & CharSet is "l"

        Unsigned first_part_len = nthOccurrence(CharSet);
        // the length of part that to be cut  (2 for "he");

        if (first_part_len == MAXUnsigned) // if nothing occurred
        {
            String ret = *this;       // make a copy of this object
            *this = L"";              // empty this object
            return ret;               // return the copy
        }

        return CutOff(first_part_len); //cut return the cut part
    }
    //--------------------------------------------------------------------------
    String CutOff(char terminating_char)
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        return CutOff((String)terminating_char); //cut return the cut part
    }
    //--------------------------------------------------------------------------
    String MultiCutOff(const String& ExSub)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        Unsigned pos1 = MAXUnsigned, pos2;

        char* c1 = this->GetString();
        char* c2 = ExSub.GetString();

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
        } // while

        if (is_consecutive == true)
            if (j == len2)   // if all were found
            {
                String ret = this->CutOff(pos1);
                pos2 = ExSub.Size() - 2;
                if (pos2 != MAXUnsigned)
                    CutOff(pos2);
                return ret;
            }
        // if not found
        String ret = *this;
        *this = L""; // empty this
        return ret; // cut all
    }
    //--------------------------------------------------------------------------
    char* GetCStyle() const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        char* c = GetString();
        return newStr(c, Size());
    }
    //--------------------------------------------------------------------------

#ifdef _DEBUG
    char*& _cstring; // this is just to show the string content while step by step debuging
#endif

    String()
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char* c = newStr("", 0);
        SetString(c, 0);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    String(const char* c_style_string)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char* c = newStr(c_style_string, strlen(c_style_string));
        SetString(c, strlen(c));
        is_memory_allocated = true; // oblect is created
#ifdef _DEBUG
        _cstring = c;
#endif
    }
    //--------------------------------------------------------------------------
    String(const wchar_t* normal_string)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        Unsigned len = wcslen(normal_string);
        char* c = new char[len + 1];
        for (Unsigned i = 0; i <= len; i++)
            c[i] = (char)normal_string[i];
        SetString(c, len);
        is_memory_allocated = true;
    }
    //--------------------------------------------------------------------------
    String(int x)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char a[50];
        itoa(x, a, 10);
        Unsigned len = strlen(a);
        char* c = new char[len + 1];
#if defined (_MSC_VER) && (_MSC_VER >= 1020)
        strcpy_s(c, len + 1, a);
#else
        strcpy(c, a);
#endif
        SetString(c, len);
        is_memory_allocated = true;
    }
    //--------------------------------------------------------------------------
    String(char* data, Unsigned len)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char* c = newStr(len);
        DataCopy(c, data, len);
        c[len] = 0;    // null terminate
        SetString(c, len);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    String(char wch)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char a[2];
        a[0] = wch;
        a[1] = 0;     // e.g. if ch=='e' then a is "e"
        char* c = newStr(a, 1);
        SetString(c, 1);
        is_memory_allocated = true; // oblect is created
    }
    //--------------------------------------------------------------------------
    String(SafeChar safe_ch);
    //--------------------------------------------------------------------------
    String(const String& right_hand_side)
#ifdef _DEBUG
        :_cstring(StringBase::string)
#endif
    {
        char* c = newStr("", 0);
        SetString(c, 0);
        is_memory_allocated = true; // oblect is created  (required for =)
        *this = right_hand_side;
    }
    //--------------------------------------------------------------------------
    ~String()
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        Destroy();
    }
    //--------------------------------------------------------------------------
    Unsigned HowManyOccurrences(const String& CharSet)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned total_len = Size();// the lenght of 'this->string'
        char* c = GetString();
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
    bool IsExactSuperstring(const String& ExSub) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        char* c1 = this->GetString();
        char* c2 = ExSub.GetString();

        Unsigned len1 = this->Size();
        Unsigned len2 = ExSub.Size();

        Unsigned pos1 = MAXUnsigned;

        bool is_consecutive = true;

        while (true) // to be breaked
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
            else        // if c2[j] was not found
            {
                if (pos1 != MAXUnsigned)
                {
                    i = pos1;
                    j = 0;
                    pos1 = MAXUnsigned;
                }
                i++; // search next block to find it
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
    bool IsSuperset(const String& CharSet) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        char* c1 = this->GetString();
        char* c2 = CharSet.GetString();

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
    bool IsSuperstring(const String& SubStr) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned i = 0;
        Unsigned j = 0;

        char* c1 = this->GetString();
        char* c2 = SubStr.GetString();

        Unsigned len1 = this->Size();
        Unsigned len2 = SubStr.Size();

        while (1)// to be breaked
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
        }// while(1)

        if (j == len2)   // if all were found
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    Unsigned nthOccurrence(const String& CharSet, const Unsigned n = 1) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        Unsigned total_len = Size();// the size of 'this->string'
        char* c = GetString();

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
        return MAXUnsigned;// there is no nth char
    }
    //--------------------------------------------------------------------------
    void Reverse()
    {
        char* c = GetString();
        Unsigned len = Size();
        Unsigned half_len = len / 2;

        Unsigned i;
        for (i = 0; i < half_len; i++)
        {
            char temp = c[i];
            c[i] = c[len - 1 - i];
            c[len - 1 - i] = temp;       // c[i] & c[len - 1 - i] are swapped
        }
    }
    //--------------------------------------------------------------------------
    void ToLower()
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        char* c = GetString();
        Unsigned len = Size();
        Unsigned i;
        for (i = 0; i < len; i++)
            if ('A' <= c[i])
                if (c[i] <= 'Z')        // if c[i] is uppercase
                    c[i] = (char)tolower(c[i]); // make it lower case
    }
    //--------------------------------------------------------------------------
    void ToUpper()
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        char* c = GetString();
        Unsigned len = Size();
        Unsigned i;
        for (i = 0; i < len; i++)
            if ('a' <= c[i])
                if (c[i] <= 'z')        // if c[i] is lowercase
                    c[i] = (char)toupper(c[i]); // make it upper case
    }
    //--------------------------------------------------------------------------
    int Int()
    {
        const char* const& c = GetString(); // protecting code
        const Unsigned& len = Size(); // protecting code
        Unsigned i;
        for (i = 0; i < len; i++) // protecting code
            if (!('0' <= c[i] && c[i] <= '9'))
            {
                if (i == 0)
                    if (c[i] == '-')
                        continue;
                print_error("String::Int(): not number");
            }
        return atoi(GetString());
    }

    //--------------------------------------------------------------------------
    String& operator = (char* c_style_string)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        ChangeStringInto(c_style_string, strlen(c_style_string));
        return *this;
    }
    //--------------------------------------------------------------------------
    String& operator = (const String& Str)
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        char* c = Str.GetString();
        ChangeStringInto(c, Str.Size());
        return *this;
    }
    //--------------------------------------------------------------------------
    bool operator == (const String& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const char* const& c1 = this->GetString();
        const Unsigned& len1 = this->Size();
        const char* const& c2 = Str.GetString();
        const Unsigned& len2 = Str.Size();

        if (CompareStrings(c1, len1, c2, len2) == 0)  // if they are equal
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator != (const String& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const char* const& c1 = this->GetString();
        const Unsigned& len1 = this->Size();
        const char* const& c2 = Str.GetString();
        const Unsigned& len2 = Str.Size();

        if (CompareStrings(c1, len1, c2, len2) == 0)  // if they are equal
            return false;
        return true;
    }
    //--------------------------------------------------------------------------
    bool operator < (const String& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const char* const& c1 = this->GetString();
        const Unsigned& len1 = this->Size();
        const char* const& c2 = after_op.GetString();
        const Unsigned& len2 = after_op.Size();

        if (CompareStrings(c1, len1, c2, len2) < 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator > (const String& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed
        const char* const& c1 = this->GetString();
        const Unsigned& len1 = this->Size();
        const char* const& c2 = after_op.GetString();
        const Unsigned& len2 = after_op.Size();

        if (CompareStrings(c1, len1, c2, len2) > 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator <= (const String& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const char* const& c1 = this->GetString();
        const Unsigned& len1 = this->Size();
        const char* const& c2 = after_op.GetString();
        const Unsigned& len2 = after_op.Size();

        if (CompareStrings(c1, len1, c2, len2) <= 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    bool operator >= (const String& after_op) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const char* const& c1 = this->GetString();
        const char* const& c2 = after_op.GetString();

        if (strcmp(c1, c2) >= 0)
            return true;
        return false;
    }
    //--------------------------------------------------------------------------
    String::SafeChar operator [] (Unsigned i);

    String::SafeChar operator [] (Unsigned i) const;
    //--------------------------------------------------------------------------
    String& operator += (const String& Str)
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        const Unsigned& len1 = Size();
        const Unsigned& len2 = Str.Size();

        const Unsigned& total_length = len1 + len2;//the sum of lengths

        char* sum_string = newStr(total_length);
        // create the S style sring to contain both.

        const char* const& c1 = GetString();
        const char* const& c2 = Str.GetString();

        DataCopy(sum_string, c1, len1);
        // add the first string to ret.

        DataCopy(&sum_string[len1], c2, len2 + 1);
        // add the second string (including terminating null)

        Destroy();// deletes string
        SetString(sum_string, total_length);

        is_memory_allocated = true;// Destroy set it to false
        return *this;
    }
    //--------------------------------------------------------------------------
    String operator + (const String& Str) const
    {
        IfNotConstructedThrow(); // check this object is not destroyed

        String ret;// an empty String
        ret += *this; // add hkStr1
        ret += Str; // add hkStr2
        return ret;
    }
};
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class String::SafeChar
{
private:
    String* This;
    const String* const_This;
    Unsigned i;
public:
    SafeChar(String* pointer_to_Str, Unsigned offset)
        : This(pointer_to_Str), const_This(pointer_to_Str), i(offset)
    {}

    SafeChar(const String* pointer_to_Str, Unsigned offset)
        : This(0), const_This(pointer_to_Str), i(offset)
    {}
    //--------------------------------------------------------------------------
    SafeChar& operator = (char ch)
    {
#ifdef _DEBUG
        if (!This)
            print_error("const SafeChar called non_const method, SafeChar& SafeChar::operator = (char ch)");
#endif
        (This->GetString())[i] = ch;
        return *this;
    }
    //--------------------------------------------------------------------------
    SafeChar& operator = (const String& Str)
    {
#ifdef _DEBUG
        if (!This)
            print_error("const SafeChar called non_const method, SafeChar& SafeChar::operator = (const String& Str)");
#endif
        *This = (This->CutOff(i)) + (Str)+ (*This);
        return *this;
    }
    //--------------------------------------------------------------------------
    operator char() const
    {
        return (const_This->GetString())[i];
    }
    //--------------------------------------------------------------------------
    void operator delete (void*)
    {
        print_error("String:operator delete (void*):if s is an String. &(s[i]) cannot be deleted.\n");
    }
    //--------------------------------------------------------------------------
    void operator delete [](void*)
    {
        print_error("String:operator delete [] (void*):if s is an String. &(s[i]) cannot be delete[]d.\n");
    }
    //--------------------------------------------------------------------------
    String operator [] (Unsigned j) const
    {
        if ((const_This->Size() <= j) || (j < i))
            return L"";
        char* c = const_This->GetString();
        return String(c + i, j - i + 1);
    }
};


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


class CS
{
public:
    char* string;
    CS(const String& Str)
    {
        string = Str.GetCStyle();
    }
    ~CS()
    {
        delWStr(string);
    }
    operator char*& ()
    {
        return string;
    }
};


inline String::String(SafeChar safe_ch)
#ifdef _DEBUG
    :_cstring(StringBase::string)
#endif
{
    char a[2];
    a[0] = (char)safe_ch;
    a[1] = 0;     // e.g. if ch=='e' then a is "e"
    char* c = newStr(a, 1);
    SetString(c, 1);
    is_memory_allocated = true; // oblect is created
#ifdef _DEBUG
    _cstring = c;
#endif
}

inline String::SafeChar String::operator [] (Unsigned i)
{
    if (Size() <= i) // if i is not well defined
        Throw("String::operator []:Bad Offset.");
    SafeChar safe_char(this, i);
    return safe_char;
}

inline String::SafeChar String::operator [] (Unsigned i) const
{
    if (Size() <= i) // if i is not well defined
        Throw("String::operator []:Bad Offset.");
    SafeChar safe_char(this, i);
    return safe_char;
}

inline std::ostream& operator << (std::ostream& before_op, const String& after_op)
{
    Unsigned len = after_op.Size();
    char* str = after_op.GetCStyle();
    Unsigned i;
    for (i = 0; i < len; i++)
        before_op << (char)str[i];   // print str[i]
    delWStr(str);
    return before_op;
}

inline std::wostream& operator << (std::wostream& before_op, const String& after_op)
{
    Unsigned len = after_op.Size();
    char* str = after_op.GetCStyle();
    Unsigned i;
    for (i = 0; i < len; i++)
        before_op << (wchar_t)str[i];   // print str[i]
    delWStr(str);
    return before_op;
}


extern "C++" inline String SpaceSpace(Unsigned space_num)
{
    if (space_num > 10000)
#ifdef __BORLANDC__
        throw "extern inline String SpaceSpace(Unsigned space_num) :space_num out of range";
#else
        print_error("extern inline String SpaceSpace(Unsigned space_num) :space_num out of range");
#endif
    char* c = new char[space_num + 1];
    for (Unsigned i = 0; i < space_num; i++)
        c[i] = ' ';
    c[space_num] = 0;
    String ret(c);
    return ret;
}

#endif
