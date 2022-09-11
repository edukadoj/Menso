// this file decomposes the text to decos for showing place of the error

#ifndef GetNthDecoH
#define GetNthDecoH
#include "Common Files\\EditBox.h"
//#include "WFile.h"


class GetNthDeco
{
public:
	Unsigned x;
	Unsigned y; // line
	String nthDeco;

	Unsigned n;

private:

	EditBox* peb;
	WString spaces;
	WString operator_chars;


	WString GetMainPart(WString LINE,Unsigned& current)
	{
		Unsigned length = LINE.Size();
		if(current + 1 > length)// if we are at the end of the file
			return L"";
		Unsigned i;
		for(i = 0; current + i < length; i++)
		{
			wchar_t wch = LINE[current + i];
			WString WCH = wch;
			if(i == 0)
			{
				if(spaces.IsExactSuperstring(WCH)) // if it's a space char
				{
					current = current + 1;
					return GetMainPart(LINE,current);
				}
				if(operator_chars.IsExactSuperstring(WCH))// if it's an operator
				{
					WString ret = LINE[current][current];
					current = current + 1;
					return ret;
				}
			}
			else
			{
				if(spaces.IsExactSuperstring(WCH)) // if it's a space char
				{
					WString ret = LINE[current][current + i - 1];
					current = current + i + 1;
					return ret;
				}
				if(operator_chars.IsExactSuperstring(WCH))// if it's an operator
				{
					WString ret = LINE[current][current + i - 1];
					current = current + i;
					return ret;
				}
			}
		}
		WString ret = LINE[current][current + i - 1];
		current = current + i;
		return ret;
	}

	void Run()
	{
		Unsigned sz = peb -> GetNumberOfLines();

		Unsigned m = 0;
		WString LINE;
		for(Unsigned i = 1; i <= sz; i++)
		{
			LINE = peb -> GetLine(i);
			LINE = LINE.CutOff(L"//");
			Unsigned current = 0;

			while(true)
			{
				WString deco = GetMainPart(LINE,current);
				if(deco == L"")
					break;
				m++;
				if(m == n)
				{
					y = i;
					x = current;
					nthDeco = WtoS(deco);
					return;
				}
			}
		}
		if(m + 1 == n)
		{
			y = sz;
			x = LINE.Size();
		}
		else
		{
			y = MAXUnsigned;
			x = MAXUnsigned;
		}
		nthDeco = L"";
	}

public:
	GetNthDeco(EditBox* p, Unsigned n)
			: peb(p),n(n)
	{
		spaces = L" 	\n\r";
		operator_chars = L"+-*/%\\=.,`;!@#$^&|:<>()[]{}~'\"";
		Run();
	}
};
#endif
