// decomposes a menso to decos & replaces a deco with another for other deco services

#ifndef ReplaceH
#define ReplaceH

#include "List.h"


class Replace
{
public:
    String text; // text a base of replaceing


    bool Contains(String deco)
    {
        Unsigned size = list.Size();
        for (Unsigned i = 0; i < size; i++)
            if ((String)list[i] == deco)
                return true;
        return false;
    }

    String Substitute(String old_deco, String new_deco)
    {
        Unsigned size = list.Size();
        for (Unsigned i = 0; i < size; i++)
            if ((String)list[i] == old_deco)
                list[i] = new_deco;
        String result;
        for (Unsigned i = 0; i < size; i++)
            result += (String)list[i];
        return result;
    }

    String NewTempName()
    {
        String ret = "_t1";
        int n = 1;
        while (Contains(ret))
        {
            n++;
            ret = ret[0][1] + n;
        }
        return ret;
    }

    struct Off
    {
        Replace* This;
        Unsigned offset;

        operator String()
        {
            return This->list[offset];
        }

        Off operator = (String new_deco)
        {
            This->list[offset] = new_deco;
            return *this;
        }

        Off(Replace* This, Unsigned offset)
            :This(This), offset(offset)
        {
        }
    };

    friend struct Off;

    Off operator [](Unsigned offset)
    {
        Off ret(this, offset);
        return ret;
    }

    Unsigned Size()
    {
        return list.Size();
    }

private:
    List list;
    String spaces;
    String operator_chars;

    String GetMainPart(Unsigned& current)
    {
        Unsigned length = text.Size();
        if (current + 1 > length)// if we are at the end of the file
            return "";
        Unsigned i;
        for (i = 0; current + i < length; i++)
        {
            char ch = text[current + i];
            String CH = ch;
            if (i == 0)
            {
                if (spaces.IsExactSuperstring(CH)) // if it's a space char
                {
                    //current = current + 1;
                    current++;
                    return GetMainPart(current);
                }
                if (operator_chars.IsExactSuperstring(CH))// if it's an operator
                {
                    String ret = text[current][current];
                    //current = current + 1;
                    current++;
                    return ret;
                }
            }
            else
            {
                if (spaces.IsExactSuperstring(CH)) // if it's a space char
                {
                    String ret = text[current][current + i - 1];
                    current = current + i + 1;
                    return ret;
                }
                if (operator_chars.IsExactSuperstring(CH))// if it's an operator
                {
                    String ret = text[current][current + i - 1];
                    current = current + i;
                    return ret;
                }
            }
        }
        String ret = text[current][current + i - 1];
        current = current + i;
        return ret;
    }

    void FillDecos()
    {
        Unsigned current = 0;
        while (true)
        {
            String deco = GetMainPart(current);
            if (deco == "")
                break;
            list << deco;
        }
    }

    void Run()
    {
        FillDecos();
    }

public:
    Replace(String text)
        : text(text)
    {
        spaces = ""; // nothing is space
        operator_chars = "+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"" " 	\n\r";
        Run();
    }
};


#endif
