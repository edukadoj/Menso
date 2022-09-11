#ifndef ClipboardH
#define ClipboardH

#include "General.h"
#undef UNICODE
#define UNICODE
#include "WString.h"
//#include <windows.h>


class Clipboard
{
public:

    void Empty()
    {
        EmptyClipboard();
    }

    void Copy(WString wstr)
    {
        if (wstr == L"")
            return;
        Empty();
        Unsigned n = wstr.Size();
        void* global_data = GlobalAlloc(GMEM_MOVEABLE, (n + 1) * sizeof(wchar_t));
        if (global_data == 0)
            print_error(L"Clipboard::Copy()");
        wchar_t* str_t = (wchar_t*)GlobalLock(global_data);
        for (Unsigned i = 0; i < n; i++)
            str_t[i] = wstr[i];
        str_t[n] = 0;
        GlobalUnlock(global_data);
        SetClipboardData(CF_UNICODETEXT, global_data);
    }

    WString Paste()
    {
        wchar_t empty_wstring[] = L"";
        wchar_t* str_t = empty_wstring;
        void* hglb = GetClipboardData(CF_UNICODETEXT);
        SIZE_T size = 0;
        if (hglb != 0)
        {
            str_t = (wchar_t*)GlobalLock(hglb);
            size = GlobalSize(hglb) / sizeof(wchar_t);
            if (str_t != 0)
                GlobalUnlock(hglb);
        }
        return WString(str_t, size - 1);
    }

    Clipboard()
    {
        int n = 10;
        while (OpenClipboard(0) == 0)
        {
            n--;
            if (n < 0)
            {
                print_error(L"ClipBoard constructor: Could not open clipboard");
                return;
            }
        }
    }

    ~Clipboard()
    {
        CloseClipboard();
    }
};


#endif
