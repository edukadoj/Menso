#pragma hdrstop
#include "Decompose.h"
#include "Common Files\\WindowsPaths.h"

String
Decompose::
GetMainPart(SimpleFile& file)
{
    Unsigned current = file.GetPointerPosition();
    Unsigned length = file.GetFileLength();
    if (current + 1 > length)// if we are at the end of the file
        return "";
    Unsigned i;
    for (i = 0; current + i < length; i++)
    {
        char ch = file[current + i];
        String CH = ch;
        if (i == 0)
        {
            if (spaces.IsExactSuperstring(CH)) // if it's a space char
            {
                file.SetPointerPosition(current + 1);
                return GetMainPart(file);
            }
            if (operator_chars.IsExactSuperstring(CH))// if it's an operator
            {
                String ret = file[current][current];
                file.SetPointerPosition(current + 1);
                return ret;
            }
        }
        else
        {
            if (spaces.IsExactSuperstring(CH)) // if it's a space char
            {
                String ret = file[current][current + i - 1];
                file.SetPointerPosition(current + i + 1);
                return ret;
            }
            if (operator_chars.IsExactSuperstring(CH))// if it's an operator
            {
                String ret = file[current][current + i - 1];
                file.SetPointerPosition(current + i);
                return ret;
            }
        }
    }
    String ret = file[current][current + i - 1];
    file.SetPointerPosition(current + i);
    return ret;
}

Decompose::
Decompose(WString simple_file_address)
    : pack(GetAppDataPath() + L"pack.txt")
{
    spaces = String(" 	\n\r");
    operator_chars = "+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"";
    pack.Empty();
    SimpleFile file(simple_file_address);
    String I;
    do
    {
        I = GetMainPart(file);
        pack << I;
    } while (I != ""); // the last pack will be ""
}
